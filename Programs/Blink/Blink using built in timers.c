//timer register is used in this case to count timing
#include<stm32f1xx.h>

void delay1(void);
void tim2config(void);
void system_clock_config(void);

int main(void){
	system_clock_config();

	//enable gpioa and gpiob clock
	RCC -> APB2ENR &= 0b11111111111111111010000110001110;
    RCC -> APB2ENR |= 0b00000000000000000000000000001100;

	//use configuration register to set gpioa to output mode and gpiob to input
	GPIOA -> CRL &= 0b00000000000000000000000000000001;	//last 4 bits for PA0
	GPIOA -> CRL |= 0b00000000000000000000000000000001;
	GPIOB -> CRH &= 0b00001000000000000000000000000000;//second 4 bits for PB14
	GPIOB -> CRH |= 0b00001000000000000000000000000000;

	//configure input pull up registers of gpiob
	GPIOB -> ODR &= 0b11111111111111110100000000000000;	//note that OUTPUT data register is used to config pullup/pulldown of input
	GPIOB -> ODR |= 0b00000000000000000100000000000000;

	tim2config();

	while (1) {
        if ((GPIOB->IDR & GPIO_IDR_IDR14) == 0) {  //if button is pressed
            GPIOA->ODR ^= GPIO_ODR_ODR0;           //toggle PA0
            delay1();
        }
    }
}
void delay1(void){
	TIM2->CNT = 0b0000000000000000;
	//check for UIF flag in timx status register
	while((TIM2 -> SR & TIM_SR_UIF) == 0);	//checking for UIF to become 1 as it indicates completion of count
	//loop above generates 1s of delay
    TIM2->SR &= ~TIM_SR_UIF; //reset UIF

}
void system_clock_config(void){
	//enable internal high speed clock
	RCC -> CR &= 0b11000000000000001111111110000111;
	RCC -> CR |= 0b00000000000000000000000000000001; //HSION bit(last bit) in RCC_CR register is set to 1 to enable HSI clock
	//wait for HSI clock to enable
	while((RCC->CR & RCC_CR_HSIRDY) == 0);

	//configure prescalers and clock specification for system clock
	RCC -> CFGR &= 0b11111111111111111111100000001100;	//system clock not divided and Select HSI clock as system clock (enable HSI at first)
	RCC -> CFGR |= 0b00000000000000000000000000000000;
}
void tim2config(void){
	//configuration of timers (we will use TIM2 timer which is a general purpose timer)
	RCC -> APB1ENR &= 0b11000001100000010011011111000001;
	RCC -> APB1ENR |= 0b00000000000000000000000000000001; //last bit is for enabling clock for TIM2

	//configure prescalers for tim2 clock
	TIM2 -> PSC &= 0b0000000000000000;
	TIM2 -> PSC |= 0b0001111100111111;	//clock will be divided by PSC+1. 7999+1=8000 in this case
	//thus, 8mhz/8000 = 1Khz clock is obtained

	//setup autoreload register
	TIM2 -> ARR &= 0b0000000000000000;
	TIM2 -> ARR |= 0b0000001111100111; //value of 999 at autoreload register;

	//enable counter
	TIM2 -> CR1 &= 0b1111111111100101;
	TIM2 -> CR1 |= 0b0000000000000101; //enable counter register
}