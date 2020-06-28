#ifndef BUTTON_H
#define BUTTON_H

#include "main.h"

class Button
{
public:
    Button()
    {
        but_ini();
    }
private:
    void but_ini()
    {
        RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;
        //GPIOA->CRL=0;
        //GPIOA->CRH=0;
        GPIOA->ODR=0;
        //GPIOA->CRL|=GPIO_CRL_CNF0_1; // CNF0[1:0] 1:0 - input mode
        //GPIOA->CRL&=~GPIO_CRL_MODE0; // MODE = 0:0; - input mode with pull-up / pull-down

        GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
        GPIOA->CRL|=GPIO_CRL_CNF0_1; // CNF0[1:0] 1:0 - input mode
        
        //GPIOA->CRL |= (0x02 << GPIO_CRL_CNF0); //Вход Pull Up/Pull Down

        GPIOA->ODR |= (1 << 0); //Подтяжка вверх

        //AFIO->EXTICR[0] &= ~(AFIO_EXTICR1_EXTI0); //Нулевой канал EXTI подключен к порту PA0
        EXTI->RTSR=0;
        //EXTI->RTSR &=~ EXTI_RTSR_TR0; //Прерывание по нарастанию импульса отключено
        EXTI->FTSR |= EXTI_FTSR_TR0; //Прерывание по спаду импульса

        EXTI->PR = EXTI_PR_PR0;      //Сбрасываем флаг прерывания 
                                     //перед включением самого прерывания
        EXTI->IMR |= EXTI_IMR_MR0;   //Включаем прерывание 0-го канала EXTI

        NVIC_EnableIRQ(EXTI0_IRQn);  //Разрешаем прерывание в контроллере прерываний
    }
};




#endif //BUTTON_H