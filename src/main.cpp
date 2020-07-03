#include "main.h"
void *__dso_handle = nullptr; // dummy
void (*InterruptManager::IsrV[88])()={nullptr}; //! fill array with nullptrs
uint16_t low=0;
uint16_t high=0;

//!*************************************  Global classes needed because startScheduler rewrite main stack
//										so need to do global classes or implement classes in heap by operator new
TimerSingle1s singleTimer1("1",1000,pdFALSE); //! set single shot timer on 3 seconds
LCD_FR lcd;
BlinkFR blink;
Calibration calTask(&singleTimer1);
//!*************************************

int main()
{
	RCCini rcc;	//! 128 MHz
	//SpiLcd lcd;	//! init IPS display
	//lcd.fillScreen(0xf800);
	singleTimer1.start(100);
	OS::taskCreate(&blink,"blink",10,2);
	OS::taskCreate(&lcd,"LCD",400,1);
	//lcd.fillScreen(0xff00);
	LED13 led;	
	Font_16x16 font16;
	/*!<Init hardware timers>!*/
	Timers t1(1); 
	Timers tLow(2);
	Timers tHigh(3);
	Timers tSec(4);
	__enable_irq();
	uint32_t x=0;

	OS::startScheduler(); //! перетирает стэк 

	while(1)
	{
		//font16.intToChar(x);
		//font16.print(20,20,0x00ff,font16.arr, 1);
		//x++;
		//font16.delay(1280);
		
		if(Timers::timerSecFlag) // every second interrupt till this second ends must draw freq count on screen  
		{
			led.toggle();
			led.toggle();
			led.toggle();
			led.toggle();
			led.toggle();
			led.toggle();
			led.toggle();
			led.toggle();// 3 counts
			low=TIM2->CNT;
			high=TIM3->CNT;
			TIM3->CNT=0;
			TIM2->CNT=0;
			Timers::timerSecFlag=false;
			//lcd.fillScreen(y);
			//y++;

			x=(high<<16)|(low);
			font16.intToChar(x);
			font16.print(20,20,0x00ff,font16.arr, 10);
			//font16.print(20,20,0x00ff,"1234567890", 5);
			//x=0;
		}		
	}
    return 0;
}
