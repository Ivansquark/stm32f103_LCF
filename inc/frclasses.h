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

/*! \brief calibration task class*/
class Calibration: public iTaskFR
{
public:
    Calibration(OS_timer* Ntim=nullptr){tim = Ntim; reed_Cal_ini(); L_cal=6.8;}
    void run() override
    {
        while(1)
        {
            if(calStarts==true)
            {
                reedSwithCal();
                tim->start(10); //!< start timer first time to get calEnds     
                calStarts = false;                          
                calStartEnds=true;
            }
            else if(calStartEnds==true && tim->singleShot==true)
            {
                calStartEnds=false; tim->singleShot=false; calEnds=true;  calStarts=false;
                //tim->start(10); //!< start timer second time to get calEnds     
                {
                    if (freq!=0)
                    {
                        C_cal = 1/(freq*freq*4*3.14*3.14*L_cal);
                    }
                    else C_cal=0;                    
                } //!< TODO here frequency must accounting                
            }
            OS::sleep(100);
        }
    }
    static bool calStarts;
    static bool calEnds;
    static float C_cal;
    static float L_cal; //!< 6.8 mH
private:
    bool calStartEnds{false};
    OS_timer* tim{nullptr};
    void reed_Cal_ini()
    {                   //!< 1-pb3 2-pb4 3-pb5 
        RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
        GPIOB->CRL|=GPIO_CRL_MODE3; //!< 1:1 50 MHz
        GPIOB->CRL&=~GPIO_CRL_CNF3; //!< 0:0 push-pull
        GPIOB->CRL|=GPIO_CRL_MODE4; //!< 1:1 50 MHz
        GPIOB->CRL&=~GPIO_CRL_CNF4; //!< 0:0 push-pull
        GPIOB->CRL|=GPIO_CRL_MODE5; //!< 1:1 50 MHz
        GPIOB->CRL&=~GPIO_CRL_CNF5; //!< 0:0 push-pull
        GPIOB->BSRR|=GPIO_BSRR_BS3;
        GPIOB->BSRR|=GPIO_BSRR_BS4;
        GPIOB->BSRR|=GPIO_BSRR_BS5; //!< sets to 1 thats block relay coil
    }
    void reedSwithCal() //!< swithes reed relay in calibration mode (1:0:1)
    {
        GPIOB->BSRR|=GPIO_BSRR_BS3;
        GPIOB->BSRR|=GPIO_BSRR_BR4; //!< to zero that turn on reed
        GPIOB->BSRR|=GPIO_BSRR_BS5;
    }
};
bool Calibration::calStarts = false;
bool Calibration::calEnds = false;
float Calibration::C_cal = 0;
float Calibration::L_cal = 0;
//-------------------------------------------------------------------------------------------------
/*! \brief measuring L task class*/
class MeasureL: public iTaskFR
{
public:
    MeasureL(QueueOS<float,1>* NqueueFloat = nullptr, OS_timer* Ntim = nullptr)
    {queueFloat = NqueueFloat,tim = Ntim; reed_ports_ini();}
    void run() override
    {
        float L=0;
        while(1)
        {
            if(Calibration::calEnds && Lflag) //!< TODO here mistake!!!
            {reedSwitchL(); tim->start(1);} //!< starts singleshot timer again
            if(tim->singleShot && Lflag && Calibration::calEnds)
            {   
                tim->singleShot=false;         
                Lflag = false;
                Calibration::calEnds = false;
                { //!< TODO here L must accounting
                    L = 1/(freq*freq*4*3.14*3.14*Calibration::C_cal)-Calibration::L_cal;
                    queueFloat->queueFrom(L,10);
                }                     
            }
        }        
    }
    static bool Lflag;
private:    
    OS_timer* tim{nullptr};
    QueueOS<float,1>* queueFloat{nullptr};
    void reed_ports_ini()
    {
        RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
        GPIOB->CRL|=GPIO_CRL_MODE3; //!< 1:1 50 MHz
        GPIOB->CRL&=~GPIO_CRL_CNF3; //!< 0:0 push-pull
        GPIOB->CRL|=GPIO_CRL_MODE3; //!< 1:1 50 MHz
        GPIOB->CRL&=~GPIO_CRL_CNF3; //!< 0:0 push-pull
        GPIOB->CRL|=GPIO_CRL_MODE3; //!< 1:1 50 MHz
        GPIOB->CRL&=~GPIO_CRL_CNF3; //!< 0:0 push-pull
        GPIOB->BSRR|=GPIO_BSRR_BS3;
        GPIOB->BSRR|=GPIO_BSRR_BS4;
        GPIOB->BSRR|=GPIO_BSRR_BS5; //!< sets to 1 thats block relay coil
    }
    void reedSwitchL() //!< switch reed relay on L measurement (1:1:0)
    {
        GPIOB->BSRR|=GPIO_BSRR_BS3;
        GPIOB->BSRR|=GPIO_BSRR_BS4;
        GPIOB->BSRR|=GPIO_BSRR_BR5;    }
    
};
bool MeasureL::Lflag=false;
//-----------------------------------------------------------------------------------------
/*! \brief measuring C task class*/
class MeasureC: public iTaskFR
{
public:
    MeasureC(QueueOS<float,1>* NqueueFloat = nullptr, OS_timer* Ntim=nullptr)
    {tim = Ntim; queueFloat = NqueueFloat, reed_ports_ini();}
    void run() override
    {
        float C=0;
        while(1)
        {
            if(Calibration::calEnds && Cflag)
            {reedSwitchC(); tim->start(1);} //!< starts singleshot timer again
            if(tim->singleShot && Cflag && Calibration::calEnds)
            {    
                Calibration::calEnds = false;
                tim->singleShot=false;
                Cflag = false;        
                {
                    C = 1/(freq*freq*4*3.14*3.14*Calibration::L_cal)-Calibration::C_cal;
                    queueFloat->queueFrom(C,10);
                } //!< TODO here C must accounting                
            }
        }
    }    
    static bool Cflag;
private:
    OS_timer* tim{nullptr};
    QueueOS<float,1>* queueFloat{nullptr};
    void reed_ports_ini()
    {
        RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
        GPIOB->CRL|=GPIO_CRL_MODE3; //!< 1:1 50 MHz
        GPIOB->CRL&=~GPIO_CRL_CNF3; //!< 0:0 push-pull
        GPIOB->CRL|=GPIO_CRL_MODE3; //!< 1:1 50 MHz
        GPIOB->CRL&=~GPIO_CRL_CNF3; //!< 0:0 push-pull
        GPIOB->CRL|=GPIO_CRL_MODE3; //!< 1:1 50 MHz
        GPIOB->CRL&=~GPIO_CRL_CNF3; //!< 0:0 push-pull
        GPIOB->BSRR|=GPIO_BSRR_BS3;
        GPIOB->BSRR|=GPIO_BSRR_BS4;
        GPIOB->BSRR|=GPIO_BSRR_BS5;
    }
    void reedSwitchC() //!< switch reed relay on L measurement (0:0:1)
    {
        GPIOB->BSRR|=GPIO_BSRR_BR3;
        GPIOB->BSRR|=GPIO_BSRR_BR4;
        GPIOB->BSRR|=GPIO_BSRR_BS5; //!< sets to 1 thats block relay coil
    }
};
bool MeasureC::Cflag=false;
//---------------------------------------------------------------------------------
//!< \brief LCD class that send information on screen
class LCD_FR: public iTaskFR
{
public:
    LCD_FR(QueueOS<float,1>* NqueueFloat=nullptr, OS_timer* nTim=nullptr)
    {queueFloat = NqueueFloat, tim = nTim;}
    ~LCD_FR(){}
    void run() override
    {
        SpiLcd lcd; //!< create object in freeRTOS stack (which in heap)
        lcd.fillScreen(0xff00); //!< yellow
        int x=0;
        Font_16x16 fontSec;  //!< object that draw 16x16 figures      
        tim->start(1);
        float C=0;
        float L=0;
        while(1)
        {
            if(tim->singleShot)
            {
                x++;
                fontSec.intToChar(x);
                fontSec.print(150,220,0x00ff,fontSec.arr,0);
                if(checkFlag==false)
                {
                    tim->singleShot=false;                
                }                
            }    
            if(tim->singleShot && checkFlag) //check button
            {
                fontSec.print(150,200,0xffff,fontSec.arr,0);
                tim->singleShot=false;
                checkFlag = false;                
            } 
            fontSec.intToChar(freq); //!< shows everytime
            fontSec.print(5,10,0x0fff,fontSec.arr,8);
            if(Calibration::calEnds)
            {C=Calibration::C_cal;}
            if(MeasureC::Cflag){queueFloat->queueRecieve(C,1);}
            fontSec.floatTochar(C); //!< shows everytime
            fontSec.print(10,100,0xf00f,fontSec.arrFloat,6);
            if(MeasureL::Lflag){queueFloat->queueRecieve(L,1);}
            fontSec.floatTochar(L); //!< shows everytime
            fontSec.print(10,150,0x0ff0,fontSec.arrFloat,6);
			Timers::timerSecFlag=false;
            OS::sleep(500);
        }
    }  
    static bool checkFlag;
    OS_timer* tim;
    QueueOS<float,1>* queueFloat{nullptr};  
};
bool LCD_FR::checkFlag=false;
//-----------------------------------------------------------------------------------------------

#endif //FRCLASSES_H_