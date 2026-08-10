#include "gpio.h"
#include "core_cm4.h"

void gpio_init(void){
    RCC->AHB2ENR |= (1 << 2); //gpioc enabled

    //setting PC2 and PC3 as inputs
    GPIOC->MODER &= ~(1 << 4); //PC2
    GPIOC->MODER &= ~(1 << 5);
    GPIOC->MODER &= ~(1 << 6); //PC3
    GPIOC->MODER &= ~(1 << 7);

    //setting PC2 and PC3 as pullup
    GPIOC->PUPDR |= (1 << 4); //PC2 set
    GPIOC->PUPDR &= ~(1 << 5);
    GPIOC->PUPDR |= (1 << 6); //PC3 set
    GPIOC->PUPDR &= ~(1 << 7);
}

//interrupt functions
void button_interrupt_init(void) {
    RCC->AHB2ENR |= (1 << 2);  // GPIOC clock (if not already enabled elsewhere)

    // route EXTI2 -> PC2 (bits 11:8 = 0010 = "C")
    SYSCFG->EXTICR1 &= ~(1 << 11);
    SYSCFG->EXTICR1 &= ~(1 << 10);
    SYSCFG->EXTICR1 |=  (1 << 9);
    SYSCFG->EXTICR1 &= ~(1 << 8);

    // route EXTI3 -> PC3 (bits 15:12 = 0010 = "C")
    SYSCFG->EXTICR1 &= ~(1 << 15);
    SYSCFG->EXTICR1 &= ~(1 << 14);
    SYSCFG->EXTICR1 |=  (1 << 13);
    SYSCFG->EXTICR1 &= ~(1 << 12);

    // falling edge trigger (pull-up buttons: press = HIGH->LOW)
    EXTI->FTSR1 |= (1 << 2);
    EXTI->FTSR1 |= (1 << 3);

    // unmask interrupts
    EXTI->IMR1 |= (1 << 2);
    EXTI->IMR1 |= (1 << 3);

    //NVIC
    NVIC->ISER[0] |= (1 << 8);   // enable IRQ8 = EXTI2
    NVIC->ISER[0] |= (1 << 9);   // enable IRQ9 = EXTI3
}

void power_btn_pressed(void) {
     RCC->AHB2ENR |= (1 << 2); //gpioc enabled
}

void distress_btn_pressed(void) {
    RCC->AHB2ENR |= (1 << 2); //gpioc enabled
}

// interrupt handlers
void EXTI3_IRQHandler(void){
    // clear the pending flag first
    EXTI->PR1 = (1 << 3);
    power_btn_pressed();
}
void EXTI2_IRQHandler(void){
    // clear the pending flag first
   EXTI->PR1 = (1 << 2);
   distress_btn_pressed();
}

