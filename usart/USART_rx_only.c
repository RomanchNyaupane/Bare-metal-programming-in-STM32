#include<stm32f1xx.h>

void system_clock(void);
void USART_en(void);
void peripheral_setup(void);
int main(void){
	system_clock();//system clock configuration
	peripheral_setup();
	USART_en();
	//ensure that the transmit data register is empty before transmitting
	while((USART2 -> SR & USART_SR_TXE) == 0); //TXE = 1 indicates non empty register`
	//write the data on usart data register

	USART2 -> CR1 |= USART_CR1_RE;

	while(1){

	while ((USART2->SR & USART_SR_RXNE) == 0); // Wait for data
	uint8_t data = USART2->DR & 0xFF;          // Read data once

	if (data == 0x38) {
	    // Now keep checking for new data and act accordingly                                // Exit if not 0x38
	    GPIOC->ODR ^= GPIO_ODR_ODR13;              // Toggle LED or whatever
	}
	}
}

void system_clock(){
	//choosing internal HSI clock for clock operation
	RCC -> CR &= ~(RCC_CR_HSION);
	RCC -> CR |= (RCC_CR_HSION);	//enable HSI clock (8MHz)
	//wait for HSI clock to turn on
	while((RCC -> CR & RCC_CR_HSIRDY) == 0);

	//configuring prescalers to scale by 1
	RCC -> CFGR &= ~(RCC_CFGR_SW);	//selects HSI clock
}

void USART_en(){


	USART2 -> CR1 &= 0xFFFFC000;//clears CR1 register
	USART2 -> CR1 |= 0x00002000;//sets UE and M bit
	//in this case, the wordlength is 8 bit and only transmission is enabled
	//there is no parity and interrupt

	//setting up usart for asynchronous mode
	USART2 -> CR2 &= 0xFFFF8000;//1 stop bit
	USART2 -> CR3 &= 0xFFFFF800;//no change in CR3 register disable every feature

	//set baudrate. for 8mhz and 9600 baudrate, mantissa=52 and fraction=8
	USART2 -> BRR &= 0xFFFF0000;//clear BRR
	USART2 -> BRR |= 0xFFFF0341;
}

void peripheral_setup(){
	//enable clock for USART2 and GPIOA peripheral

	RCC -> APB1ENR |= RCC_APB1ENR_USART2EN;
	RCC -> APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC -> APB2ENR |= RCC_APB2ENR_IOPCEN;//setting pc13(built in led pin)
	//select pins of usart2 (pin12->PA2-Tx, pin13->PA3-Rx) to operate in Alternate function IO mode
	//take a look at page number 116/117 of RM0008, tx pin needs to be at alternate function push-pull and rx pin needs to be at Input floating / Input pull-up
	GPIOA -> CRL &= ~((GPIO_CRL_MODE2 | GPIO_CRL_CNF2) | (GPIO_CRL_MODE3 | GPIO_CRL_CNF3));
	//mode of PA2(tx) is set for 2mhz and cnf is set for alternate function push pull
	GPIOA -> CRL |= (GPIO_CRL_MODE2_1 | GPIO_CRL_CNF2_1 | GPIO_CRL_CNF3_1);	//mode for pa3 is already 00 indicating input so only CNF is changed got input pull-up/pull-down

	GPIOC -> CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
	GPIOC->CRH |= GPIO_CRH_MODE13_1;

	//now that we have set pins to work at alternate function mode, we also need to define the alternate function to be USART2
	AFIO -> MAPR &= ~AFIO_MAPR_USART2_REMAP; //disables alternate function mapping

	//using ODR of GPIOA to set input pin PA3 at input pull up
	GPIOA -> ODR &= ~(GPIO_ODR_ODR3);//clearing the ODR3 bit in ODR register
	GPIOA -> ODR |= GPIO_ODR_ODR3;//setting ODR3: 1 for pull-up and 0 for pull down
}
