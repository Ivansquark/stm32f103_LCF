#ifndef FRCLASSES_H_
#define FRCLASSES_H_

#include "frwrapper.h"
#include "timer3.h"

/*!
    \brief classes thats inherits from frWrappers classes
    these classes realizes 
*/
//-----------------------------------------------------------------------------
class TimerSingle3s:public OS_timer
{
public:
    TimerSingle3s(const char* timName, const TickType_t period,const UBaseType_t AutoReload,
            void * const pvTimerID=nullptr):OS_timer(timName,period,AutoReload,this)
    {}
    void run()override
    {
        singleShot=true;  //!< software timer counts to end     
        //start(1);
    }    
};
//bool TimerSingle1s::singleShot = false;
//-----------------------------------------------------------------------------
//!< debug class that blinking diod on PC13
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
//---------------------------------------------------------------------------------
//!< \brief LCD class that send information on screen
class LCD_FR: public iTaskFR
{
public:
    LCD_FR(OS_timer* nTim=nullptr){tim = nTim;}
    ~LCD_FR(){}
    void run() override
    {
        SpiLcd lcd; //!< create object in freeRTOS stack (which in heap)
        lcd.fillScreen(0xff00); //!< yellow
        int x=0;
        Font_16x16 fontSec;  //!< object that draw 16x16 figures      
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
//-----------------------------------------------------------------------------------------------
/*! \brief calibration task class*/
class Calibration: public iTaskFR
{
public:
    Calibration(OS_timer* Ntim){tim = Ntim; reed_Cal_ini();}
    void run() override
    {
        while(1)
        {
            if(calStarts==true)
            {
                reedSwithCal();
                tim->start(10);                                
            }
            else if(calStarts==true && tim->singleShot==true)
            {
                tim->singleShot==false; calEnds=true;  calStarts=false;
                {} //!< TODO here frequency must accounting                
            }
            OS::sleep(100);
        }
    }
    static bool calStarts;
    static bool calEnds;
private:
    OS_timer* tim{nullptr};
    void reed_Cal_ini()
    {}
    void reedSwithCal() //!< swithes reed relay in calibration mode
    {}
};
bool Calibration::calStarts = false;
bool Calibration::calEnds = false;
//-------------------------------------------------------------------------------------------------
/*! \brief measuring L task class*/
class MeasureL: public iTaskFR
{
public:
    MeasureL(OS_timer* Ntim = nullptr){tim = Ntim; reed_ports_ini();}
    void run() override
    {
        while(1)
        {
            if(Calibration::calEnds && Lflag)
            {reedSwitchL(); tim->start(1);} //!< starts singleshot timer again
            if(tim->singleShot && Lflag && Calibration::calEnds)
            {   
                tim->singleShot=false;         
                Lflag = false;
                Calibration::calEnds = false;
                {} //!< TODO here L must accounting                    
            }
        }        
    }
    static bool Lflag;
private:    
    OS_timer* tim{nullptr};
    void reed_ports_ini()
    {
        
    }
    void reedSwitchL() //!< switch reed relay on L measurement
    {}
};
bool MeasureL::Lflag=false;
//-----------------------------------------------------------------------------------------
/*! \brief measuring C task class*/
class MeasureC: public iTaskFR
{
public:
    MeasureC(OS_timer* Ntim=nullptr){tim = Ntim; reed_ports_ini();}
    void run() override
    {
        while(1)
        {
            if(Calibration::calEnds && Cflag)
            {reedSwitchC(); tim->start(1);} //!< starts singleshot timer again
            if(tim->singleShot && Cflag && Calibration::calEnds)
            {    
                Calibration::calStarts = false;
                tim->singleShot=false;
                Cflag = false;        
                {} //!< TODO here C must accounting                
            }
        }
    }    
    static bool Cflag;
private:
    OS_timer* tim{nullptr};
    void reed_ports_ini()
    {

    }
    void reedSwitchC() //!< switch reed relay on L measurement
    {}
};
bool MeasureC::Cflag=false;


#endif //FRCLASSES_H_