#include "stm32f10xx.h"
void delay_ms*uint32_t delay);
int main(void){
	//enable the clock for GPIOA
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	//configure PA0 as General purpose output push-pull
	GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
	GPIOA->CRL |= GPIO_CRL_MODE0_0;
	
	while(1){
		GPIOA->ODR ^= GPIO_ODR_ODR0;
		delay(500)
	}
}
void delay_ms(uint32_t delay) {
    // Simple loop-based delay
    for (uint32_t i = 0; i < delay * 8000; i++) {
        __asm("nop"); // No Operation
    }
}