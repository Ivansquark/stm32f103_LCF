#include "main.h"

void (*InterruptManager::IsrV[88])()={nullptr}; //! fill array with nullptrs
uint16_t low=0;
uint16_t high=0;
bool timerSecFlag=0;

//--------------------------------------------------------------------------------------------------------
extern "C" void TIM4_IRQHandler(void) //обработчик прерывания раз в 1 с
{
	TIM4->SR &=~ TIM_SR_UIF; //скидываем флаг прерывания
	timerSecFlag=true;
	NVIC_ClearPendingIRQ(TIM4_IRQn);
}
//--------------------------------------------------------------------------------------------------------

int main()
{
	RCCini rcc;	
	SpiLcd lcd;
	lcd.fillScreen(0xf800);
	//lcd.fillScreen(0xff00);
	LED13 led;
	timerSecFlag=0;
	Font_16x16 font16;
	Timers tLow(2);
	Timers tHigh(3);
	Timers tSec(4);
	__enable_irq();
	uint32_t x=0;
	while(1)
	{
		//font16.intToChar(x);
		//font16.print(20,20,0x00ff,font16.arr, 1);
		//x++;
		//font16.delay(1280);
		
		if(timerSecFlag) // every second interrupt till this second ends must draw freq count on screen  
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
			timerSecFlag=false;
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
