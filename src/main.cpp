/*!
 * \file main file with main function from which program is started after initialization procedure in startup.cpp
 * 
 */
#include "main.h"
void *__dso_handle = nullptr; // dummy "guard" that is used to identify dynamic shared objects during global destruction. (in fini in startup.cpp)
void (*InterruptManager::IsrV[88])()={nullptr}; //! fill array with nullptrs

//!******  Global classes needed because startScheduler rewrite main stack
//******* so need to do global classes or implement classes in user heap by operator new
///*!	\brief other global objects*/
Button but;
ButtonC butC;
ButtonL butL;

/*!	\brief freeRTOS classes*/
QueueOS<float,1> queueFloat; //!< create queue of float_32 (in bss near FreeRTOS heap2)
TimerSingle3s singleTimer1("1",8000,pdFALSE); //!< set single shot timer on 7 seconds for pause before calibration and measuring results will counts
LCD_FR lcd(&queueFloat,&singleTimer1); //!< set LCD object and 
//BlinkFR blink;
Calibration calTask(&singleTimer1);
MeasureL measureL(&queueFloat,&singleTimer1);
MeasureC measureC(&queueFloat,&singleTimer1);

/*!<Init hardware timers>!*/
Timers t1(1);   //!< antirattle timer 100ms 
Timers tLow(2);	//!< master timer
Timers tHigh(5);//!< slave timer
Timers tSec(4); //!< precision one second timer
Timers encoder(3); //!< encoder
//!*************************************

int main()
{	
	RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;
	AFIO->MAPR|=AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
	RCCini rcc;	//! 72 MHz
	SpiLcd lcd1;
	lcd1.fillScreen(0xffff);
	//LED13 led;
	//TimerSingle1s* singleTimer1 = new TimerSingle1s("1",1000,pdTRUE); //! set single shot timer on 1 seconds
	//LCD_FR* lcd=new LCD_FR(singleTimer1); //!< implement two objects in user heap	
	__enable_irq();
	OS::taskCreate(&measureL,"measureL",100,1);
	OS::taskCreate(&measureC,"measureC",100,1);
	OS::taskCreate(&calTask,"calibration",100,1);
	//OS::taskCreate(&blink,"blink",100,2); //! when stack size is not enough its goes in hardfault
	OS::taskCreate(&lcd,"LCD",400,1);
	OS::startScheduler(); //! перетирает стэк in main	
	
	int x=0;
	RCC->APB2ENR|=RCC_APB2ENR_IOPEEN;
	GPIOE->CRL&=~GPIO_CRL_CNF0;
	GPIOE->CRL|=GPIO_CRL_MODE0;
	while(1)
	{	
		x++;
		GPIOE->ODR^=GPIO_ODR_ODR0;
		for(uint8_t i=0;i<255;i++)
		{lcd1.fillScreen(i);}
	}
    return 0;
}
