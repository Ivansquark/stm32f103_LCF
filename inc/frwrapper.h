#ifndef FRWRAPPER_H_
#define FRWRAPPER_H_

#include "stdint.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"
#include "portmacro.h"
#include "main.h"

class iTaskFR
{
public: 
    iTaskFR(){}
    virtual ~iTaskFR(){}
    virtual void run()=0;
    TaskHandle_t handle=nullptr;   //pointer to task control structure of freertos
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
class OS
{
public:
/*************************- task -****************************************************************/
    static void taskCreate(iTaskFR* obj, const char *pName,const uint16_t stackDepth,uint8_t priority)
    { obj->handle = (xTaskCreate(Run,pName,stackDepth,obj,priority,&obj->handle)==pdTRUE) ? obj->handle : nullptr;}                 
    //{ xTaskCreate(reinterpret_cast<TaskFunction_t>(Run),pName,stackDepth,&obj,priority,NULL);}
    
/*************************- queue -**************************************************************/    
    static QueueHandle_t queueCreate(uint8_t length, uint8_t size)
    {return xQueueCreate(length,size);}    
    static bool queueSend(QueueHandle_t &queue, const void *pItemToQueue,uint16_t timeout)
    {return (xQueueSend(queue, pItemToQueue, timeout)==pdTRUE);} // оператор == возвращает true, если 1 равно 2, в противном случае - false 
    static bool queueSendISR(QueueHandle_t &queue, const void *pItemToQueue)
    {return (xQueueSendFromISR(queue, pItemToQueue, nullptr)==pdTRUE);}
    static bool queueRecieve(QueueHandle_t &queue,void* pItem, uint32_t timeout)
    {return (xQueueReceive(queue,pItem,timeout)==pdTRUE);}
    static void queueDelete(QueueHandle_t &queue){vQueueDelete(queue);}
/********************************- semaphore -*************************************************/
    static SemaphoreHandle_t semaphoreCreate()
    {return xSemaphoreCreateBinary();}
    static void semFromIsr(SemaphoreHandle_t handle)
    {xSemaphoreGiveFromISR(handle,nullptr);}
    static bool semRecieve(SemaphoreHandle_t handle, uint16_t timeout)
    {return xSemaphoreTake(handle, timeout);}
    static void deleteSemaphore(SemaphoreHandle_t handle)
    {vSemaphoreDelete(handle);}
/********************************- critical section -********************************************/
    static void criticalSectionEnter()
    {
        taskENTER_CRITICAL(); // prohibition interrupts and sysTick
    }
    static void criticalSectionLeave()
    {
        taskEXIT_CRITICAL();
    }
    static void criticalSectionEnterISR()
    {
        taskENTER_CRITICAL_FROM_ISR();
        vTaskSuspendAll();
    }
    static void criticalSectionLeaveISR(uint32_t x)
    {
        taskEXIT_CRITICAL_FROM_ISR(x);
    }
    static void stopScheduller(){vTaskSuspendAll();}
    static void newStartScheduller(){xTaskResumeAll();}
/********************************- scheduller -**************************************************/
    static void startScheduler()
    {vTaskStartScheduler();}
    static void sleep(uint16_t x){vTaskDelay(x);}
private:
    //static TaskFunction_t Run(void* x)  // void(*Run)(void*)    
    static void Run(void* x)  // set object here
    {static_cast<iTaskFR*>(x)->run();}    
};
//////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T,uint16_t length>
class QueueOS
{
public:
    QueueOS(){queueHandle = OS::queueCreate(length,sizeof(T));} //create queue with creation class object
    ~QueueOS(){OS::queueDelete(queueHandle);}
    bool queueFrom(const T &item,uint16_t timeout)
    {
        const void* x = reinterpret_cast<const void*>(&item);
        return OS::queueSend(queueHandle,x,timeout);
    }
    bool queueFromIsr(const T &item){return OS::queueSendISR(queueHandle,item);}
    bool queueRecieve(T &item, uint32_t timeout){return OS::queueRecieve(queueHandle,&item,timeout);}
private:
    QueueHandle_t queueHandle; //указатель на структуру очереди
};
class SemaphoreOS
{
public:
    SemaphoreOS(){semHandle=OS::semaphoreCreate();}
    ~SemaphoreOS(){OS::deleteSemaphore(semHandle);}
    void giveFromIsr(){OS::semFromIsr(semHandle);}
    bool takeSem(uint16_t timeout){return OS::semRecieve(semHandle,timeout);}
private:
    SemaphoreHandle_t semHandle;
    //StaticSemaphore_t mutex;
};

/*!
    \brief abstract FreeRTOS wrapper timer class
*/
class OS_timer
{
public:
    OS_timer(const char* timName, const TickType_t period,const UBaseType_t AutoReload,
            void * const pvTimerID=nullptr) //! period = 500/portTICK_RATE_MS 
    {
        //! AutoReload - pdTRUE - периодический, pdFalse - интервальный
        xTimer = xTimerCreate(timName, period/portTICK_RATE_MS, AutoReload, pvTimerID, timerCallBack); 
        //xTimer->pxCallbackFunction= timerCall; становится так
        timer = this;         
    }
    bool start(portTickType xBlockTime)
    {
        return (xTimerStart(xTimer,xBlockTime)==pdTRUE); //        
    }
    bool stop(portTickType xBlockTime)
    {
        return (xTimerStop(xTimer,xBlockTime)==pdTRUE);
    }
protected:
    virtual void run()=0; //! abstract function that realizes callback, must be initialized in inheritor
private:
    xTimerHandle xTimer; //! pointer on a structure
    static void timerCallBack(xTimerHandle xTimer)
    {       
        timer->run();
        return;
    }
    static OS_timer* timer;
};
OS_timer* OS_timer::timer=nullptr;
/*******************************************************************************************/

#endif //FRWRAPPER_H_
