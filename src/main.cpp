#include "main.h"
void *__dso_handle = nullptr; // dummy
void (*InterruptManager::IsrV[88])()={nullptr}; //! fill array with nullptrs

//!*************************************  Global classes needed because startScheduler rewrite main stack
//										so need to do global classes or implement classes in heap by operator new
TimerSingle1s singleTimer1("1",3000,pdFALSE); //! set single shot timer on 1 seconds
LCD_FR lcd(&singleTimer1);

BlinkFR blink;
Calibration calTask(&singleTimer1);
Button but;
Timers t1(1); 
Timers tLow(2);
Timers tHigh(3);
Timers tSec(4);
//!*************************************

int main()
{	
	RCCini rcc;	//! 128 MHz
	LED13 led;
	//TimerSingle1s* singleTimer1 = new TimerSingle1s("1",1000,pdTRUE); //! set single shot timer on 1 seconds
	//LCD_FR* lcd=new LCD_FR(singleTimer1);
	/*!<Init hardware timers>!*/
	//Button* but = new Button;	
	__enable_irq();
	
	OS::taskCreate(&calTask,"calibration",100,1);
	OS::taskCreate(&blink,"blink",100,2); //! when stack size is not enough its goes in hardfault
	OS::taskCreate(&lcd,"LCD",400,1);
	OS::startScheduler(); //! перетирает стэк	
	while(1)
	{				
	}
    return 0;
}
