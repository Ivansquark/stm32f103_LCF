#ifndef FRCLASSES_H_
#define FRCLASSES_H_

#include "frwrapper.h"

class TimerSingle1s:public OS_timer
{
public:
    TimerSingle1s(const char* timName, const TickType_t period,const UBaseType_t AutoReload,
            void * const pvTimerID=nullptr):OS_timer(timName,period,AutoReload)
    {}
    void run()override
    {
        singleShot=true;
        start(10);
    }
    static bool singleShot;
};
bool TimerSingle1s::singleShot = false;

class BlinkFR:public iTaskFR
{
public:
    BlinkFR(){leds_ini();}
    ~BlinkFR()override{}
    void run() override
    {
        blink13();
    }
private:
    void blink13()
    {
        while(1)
        {
            GPIOC->ODR^=GPIO_ODR_ODR13;
            OS::sleep(200);
        }        
    }
    void leds_ini()
    {
        RCC->APB2ENR|=RCC_APB2ENR_IOPCEN;
	    GPIOC->CRH&=~GPIO_CRH_CNF13;
	    GPIOC->CRH|=GPIO_CRH_MODE13;//50MHz
    }
};
class LCD_FR: public iTaskFR
{
public:
    LCD_FR(){}
    ~LCD_FR(){}
    void run() override
    {
        SpiLcd lcd;
        lcd.fillScreen(0xff00);
        int x=0;
        Font_16x16 fontSec;
        while(1)
        {
            if(TimerSingle1s::singleShot)
            {
                x++;
                fontSec.intToChar(x);
                fontSec.print(100,100,0x00ff,fontSec.arr,1);
                TimerSingle1s::singleShot=false;
            }
        }
    }    
};
class MeasureL: public iTaskFR
{
public:
    MeasureL(){}
    void run() override
    {
        while(1)
        {}
    }    
};

class MeasureC: public iTaskFR
{
public:
    MeasureC(){}
    void run() override
    {
        while(1)
        {}
    }
};

class Calibration: public iTaskFR
{
public:
    Calibration(OS_timer* tim){os_timer = tim;}
    void run() override
    {
        while(1)
        {
            if(calFlag==true)
            {
                os_timer->start(10);
            }
        }
    }
    static bool calFlag;
    OS_timer* os_timer{nullptr};
};
bool Calibration::calFlag = false;
#endif //FRCLASSES_H_