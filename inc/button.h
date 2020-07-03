#ifndef BUTTON_H
#define BUTTON_H

#include "irq.h"
#include "frclasses.h"

class Button: public Interruptable
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
        GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0); // reset
        GPIOA->CRL|=GPIO_CRL_CNF0_1; // CNF0[1:0] 1:0  - input mode with pull-up / pull-down
        GPIOA->ODR |= (1 << 0); //Подтяжка вверх
        AFIO->EXTICR[0] &= ~(AFIO_EXTICR1_EXTI0); //Нулевой канал EXTI подключен к порту PA0
        EXTI->RTSR=0;
        EXTI->RTSR &=~ EXTI_RTSR_TR0; //Прерывание по нарастанию импульса отключено
        EXTI->FTSR |= EXTI_FTSR_TR0; //Прерывание по спаду импульса
        EXTI->PR = EXTI_PR_PR0;      //Сбрасываем флаг прерывания перед включением самого прерывания
        EXTI->IMR |= EXTI_IMR_MR0;   //Включаем прерывание 0-го канала EXTI
        NVIC_EnableIRQ(EXTI0_IRQn);  //Разрешаем прерывание в контроллере прерываний
    }
};

class ButtonC: public Interruptable
{
public: 
    ButtonC(){butC_ini();}
    static bool ButFlagC;
private:
    void butC_ini()
    {
        RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
        RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;
        GPIOB->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6); // 0:0 input mode
        GPIOB->CRL|=GPIO_CRL_CNF6_1; // CNF0[1:0] 1:0 - input with pull-up pull-down
        GPIOB->ODR |= GPIO_ODR_ODR6; //pull-up
        AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI6_PB; // EXTI on PB6
        EXTI->RTSR &=~ EXTI_RTSR_TR6; //rising trigger disabled       
        EXTI->FTSR |= EXTI_FTSR_TR6; //Прерывание по спаду импульса falling trigger enabled
        EXTI->PR = EXTI_PR_PR6;      //Сбрасываем флаг прерывания //перед включением самого прерывания                                     
        EXTI->IMR |= EXTI_IMR_MR6;   //Включаем прерывание 6-го канала EXTI
        NVIC_EnableIRQ(EXTI9_5_IRQn);  //Разрешаем прерывание в контроллере прерываний
    }
};
bool ButtonC::ButFlagC=false;
/*! \brief < class button for L measurements>!*/
class ButtonL: public Interruptable
{
public: 
    ButtonL(){butL_ini();}
    static bool ButFlagL;
private:
    void butL_ini()
    {
        RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
        RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;
        GPIOB->CRL &= ~(GPIO_CRL_MODE7 | GPIO_CRL_CNF7); // 0:0 input mode
        GPIOB->CRL|=GPIO_CRL_CNF7_1; // CNF0[1:0] 1:0 - input with pull-up pull-down
        GPIOB->ODR |= GPIO_ODR_ODR7; //pull-up
        AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI7_PB; // EXTI on PB6
        EXTI->RTSR &=~ EXTI_RTSR_TR7; //rising trigger disabled       
        EXTI->FTSR |= EXTI_FTSR_TR7; //Прерывание по спаду импульса falling trigger enabled
        EXTI->PR = EXTI_PR_PR7;      //Сбрасываем флаг прерывания //перед включением самого прерывания                                     
        EXTI->IMR |= EXTI_IMR_MR7;   //Включаем прерывание 6-го канала EXTI
        NVIC_EnableIRQ(EXTI9_5_IRQn);  //Разрешаем прерывание в контроллере прерываний
    }
};
bool ButtonL::ButFlagL=false;
extern "C" void EXTI0_IRQHandler()
{
    //for(int i=0;i<7200000;i++) {}  //ждем ~100 мс  от дребезга
    NVIC_DisableIRQ(EXTI0_IRQn);//! отключаем прерывание от кнопки
    TIM1->CR1|=TIM_CR1_CEN; //! включаем таймер1 для отсчета задержки от дребезга по прерыванию которого включаем прерывание от кнопки
    //button_flag=1;
    EXTI->PR = EXTI_PR_PR0; //Сбрасываем флаг прерывания
    LED13::toggle();
}

extern "C" void EXTI9_5_IRQHandler()
{    
    if(EXTI->PR&EXTI_PR_PR6)
    {
        NVIC_DisableIRQ(EXTI9_5_IRQn);//! отключаем прерывание от кнопки
        TIM1->CR1|=TIM_CR1_CEN; //! включаем таймер1 для отсчета задержки от дребезга по прерыванию которого включаем прерывание от кнопки
        //!Сбрасываем флаг прерывания pb6
        EXTI->PR=EXTI_PR_PR6;
        Calibration::calFlag=true;        
    }
    if(EXTI->PR&EXTI_PR_PR7)
    {
        NVIC_DisableIRQ(EXTI9_5_IRQn);//! отключаем прерывание от кнопки
        TIM1->CR1|=TIM_CR1_CEN; //! включаем таймер1 для отсчета задержки от дребезга по прерыванию которого включаем прерывание от кнопки
        //!Сбрасываем флаг прерывания pb7
        EXTI->PR=EXTI_PR_PR7;
        Calibration::calFlag=true;
    }
}

#endif //BUTTON_H