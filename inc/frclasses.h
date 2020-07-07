#ifndef FRCLASSES_H_
#define FRCLASSES_H_

#include "frwrapper.h"
#include "timer3.h"

class TimerSingle1s:public OS_timer
{
public:
    TimerSingle1s(const char* timName, const TickType_t period,const UBaseType_t AutoReload,
            void * const pvTimerID=nullptr):OS_timer(timName,period,AutoReload,this)
    {}
    void run()override
    {
        singleShot=true;       
        //start(1);
    }    
};
//bool TimerSingle1s::singleShot = false;

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
    LCD_FR(OS_timer* nTim=nullptr){tim = nTim;}
    ~LCD_FR(){}
    void run() override
    {
        SpiLcd lcd;
        lcd.fillScreen(0xff00);
        int x=0;
        Font_16x16 fontSec;        
        tim->start(1);
        while(1)
        {
            if(tim->singleShot)
            {
                x++;
                fontSec.intToChar(x);
                fontSec.print(100,100,0x00ff,fontSec.arr,0);
                if(checkFlag==false)
                {
                    tim->singleShot=false;                
                }                
            }    
            if(tim->singleShot && checkFlag) //check button
            {
                //fontSec.arr = {'',};
                fontSec.print(10,150,0xffff,fontSec.arr,0);
                tim->singleShot=false;
                checkFlag = false;                
            } 
            fontSec.intToChar(freq);
            fontSec.print(10,50,0xffff,fontSec.arr,8);
			Timers::timerSecFlag=false;
            OS::sleep(200);
        }
    }  
    static bool checkFlag;
    OS_timer* tim;  
};
bool LCD_FR::checkFlag=false;

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
                calFlag = false;
            }
            OS::sleep(100);
        }
    }
    static bool calFlag;
    OS_timer* os_timer{nullptr};
};
bool Calibration::calFlag = false;

class MeasureL: public iTaskFR
{
public:
    MeasureL(OS_timer* Ntim = nullptr){tim = Ntim;}
    void run() override
    {
        while(1)
        {
            if(tim->singleShot&&Lflag)
            {
                Calibration::calFlag = false;
                Lflag = false;
            }
        }        
    }
    static bool Lflag;
private:    
    OS_timer* tim{nullptr};
};
bool MeasureL::Lflag=false;

class MeasureC: public iTaskFR
{
public:
    MeasureC(OS_timer* Ntim=nullptr){tim = Ntim;}
    void run() override
    {
        while(1)
        {
            if(tim->singleShot&&Cflag)
            {
                Calibration::calFlag = false;
                Cflag = false;
            }
        }
    }    
    static bool Cflag;
private:
    OS_timer* tim{nullptr};
};
bool MeasureC::Cflag=false;


#endif //FRCLASSES_H_