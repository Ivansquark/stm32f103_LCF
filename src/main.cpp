/*!
 * \file main file with main function from which program is started after initialization procedure in startup.cpp
 * 
 */
#include "main.h"
void *__dso_handle = nullptr; // dummy "guard" that is used to identify dynamic shared objects during global destruction. (in fini in startup.cpp)
void (*InterruptManager::IsrV[88])()={nullptr}; //! fill array with nullptrs

//!******  Global classes needed because startScheduler rewrite main stack
//******* so need to do global classes or implement classes in user heap by operator new
/*!	\brief other global objects*/
Button but;
ButtonC butC;
ButtonL butL;

/*!	\brief freeRTOS classes*/
QueueOS<float,1> queueFloat; //!< create queue of float_32 (in bss near FreeRTOS heap2)
TimerSingle3s singleTimer1("1",6000,pdFALSE); //!< set single shot timer on 3 seconds for pause before calibration and measuring results will counts
LCD_FR lcd(&queueFloat,&singleTimer1); //!< set LCD object and 
BlinkFR blink;
Calibration calTask(&singleTimer1);
MeasureL measureL(&queueFloat,&singleTimer1);
MeasureC measureC(&queueFloat,&singleTimer1);

/*!<Init hardware timers>!*/
Timers t1(1);   //!< antirattle timer 100ms 
Timers tLow(2);	//!< master timer
Timers tHigh(3);//!< slave timer
Timers tSec(4); //!< precision one second timer

//!*************************************

int main()
{	
	RCCini rcc;	//! 128 MHz
	LED13 led;
	//TimerSingle1s* singleTimer1 = new TimerSingle1s("1",1000,pdTRUE); //! set single shot timer on 1 seconds
	//LCD_FR* lcd=new LCD_FR(singleTimer1); //!< implement two objects in user heap	
	__enable_irq();
	OS::taskCreate(&measureL,"calibration",100,1);
	OS::taskCreate(&measureC,"calibration",100,1);
	OS::taskCreate(&calTask,"calibration",100,1);
	OS::taskCreate(&blink,"blink",100,2); //! when stack size is not enough its goes in hardfault
	OS::taskCreate(&lcd,"LCD",400,1);
	OS::startScheduler(); //! перетирает стэк in main	

	
	while(1)
	{	

	}
    return 0;
}
