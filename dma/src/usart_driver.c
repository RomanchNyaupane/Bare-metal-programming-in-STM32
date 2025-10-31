//contains function definitions

#include "usart_driver.h"

void usart_init(USART_Config_t *usartConfig){
	system_setup(usartConfig);
    (*usartConfig).Instance -> CR1 &= 0xFFFFC000;//clears CR1 register
	(*usartConfig).Instance -> CR1 |= USART_CR1_UE;

    //(*usartConfig).Instance -> CR1 is equivalent to: for example- USART2 -> CR1
	(*usartConfig).Instance -> CR1 |= (*usartConfig).WordLength == 8 ? 0x00000000: (*usartConfig).WordLength == 9 ? 0x00001000 : 0x00000000;//sets UE and M bit. default to 8 bit mode
	//in this case, the wordlength is 8 bit and only transmission is enabled

    (*usartConfig).Instance -> CR1 |= (*usartConfig).Parity == 0 ? 0x00000000 : (*usartConfig).Parity == 1 ? 0x00000100 : 0x00000000; //no parity by default, even parity if 1, odd parity if 2

	//setting up usart for asynchronous mode
	(*usartConfig).Instance -> CR2 &= ((*usartConfig).StopBits == 1)?(0xFFFF8000):(0xFFFF0000);//1 stop bit else 0 stop bits

	(*usartConfig).Instance -> CR3 &= 0xFFFFF800;//no change in CR3 register disable every feature
	(*usartConfig).Instance -> CR3 |= USART_CR3_DMAT; //enable dma at transmission

	//set baudrate. for 8mhz and 9600 baudrate, mantissa=52 and fraction=8
	(*usartConfig).Instance -> BRR &= 0xFFFF0000;//clear BRR
	(*usartConfig).Instance -> BRR |= (*usartConfig).BaudRate == 9600 ? 0xFFFF0341 : 0xFFFF0000; //only 9600 baudrate is configured here

//    (*usartConfig).Instance->CR1 |= USART_CR1_RXNEIE; //interrupt enable
//
//    if ((*usartConfig).Instance == USART1)
//        NVIC_EnableIRQ(USART1_IRQn);
//    else if ((*usartConfig).Instance == USART2)
//        NVIC_EnableIRQ(USART2_IRQn);


    (*usartConfig).Instance -> CR1 |=  USART_CR1_TE ; //enable usart
    //(*usartConfig).Instance -> CR1 |=  USART_CR1_RE;
}

void system_setup(USART_Config_t *usartConfig){
    RCC -> CR |= RCC_CR_HSION; //8mhz internal clock
    while(!(RCC->CR & RCC_CR_HSIRDY));

    //configure prescalers to divide clock by 1
    RCC -> CFGR |= RCC_CFGR_HPRE_DIV1; //AHB clock undivided
    RCC -> CFGR |= RCC_CFGR_PPRE1_DIV1; //APB1 clock undivided
    RCC -> CFGR |= RCC_CFGR_PPRE2_DIV1; //APB2 clock undivided


    if((*usartConfig).Instance == USART1){
        RCC -> APB2ENR |= RCC_APB2ENR_USART1EN;
        GPIOA -> CRL &= ~((GPIO_CRH_MODE9 | GPIO_CRH_CNF9) | (GPIO_CRH_MODE10 | GPIO_CRH_CNF10));
        GPIOA -> CRL |= (GPIO_CRH_MODE9_1 | GPIO_CRH_CNF9_1 | GPIO_CRH_CNF10_1);
    	RCC -> APB2ENR |= RCC_APB2ENR_IOPAEN;//not sure if this corresponds to pins or usart 1



    	RCC -> APB2ENR |= RCC_APB2ENR_IOPCEN;
    	GPIOC -> CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
    	GPIOC->CRH |= GPIO_CRH_MODE13_1;
    	GPIOC->BSRR = GPIO_BSRR_BS13;  // Set high (LED OFF if active-low)
    }
    if((*usartConfig).Instance == USART2){
        RCC -> APB1ENR |= RCC_APB1ENR_USART2EN;
    	RCC -> APB2ENR |= RCC_APB2ENR_IOPAEN;
        GPIOA -> CRL &= ~((GPIO_CRL_MODE2 | GPIO_CRL_CNF2) | (GPIO_CRL_MODE3 | GPIO_CRL_CNF3));
        GPIOA -> CRL |= (GPIO_CRL_MODE2_1 | GPIO_CRL_CNF2_1 | GPIO_CRL_CNF3_1);	//mode for pa3 is already 00 indicating input so only CNF is changed got input pull-up/pull-down
    	AFIO -> MAPR &= ~AFIO_MAPR_USART2_REMAP; //disables alternate function mapping



	    RCC -> APB2ENR |= RCC_APB2ENR_IOPCEN;
	    GPIOC -> CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
	    GPIOC->CRH |= GPIO_CRH_MODE13_1;
	    GPIOC->BSRR = GPIO_BSRR_BS13;  // Set high (LED OFF if active-low)
    }
}

void usart_tx(char *str){ //exposed to user
	usart_tx_main(str, &USART_Config);
}
void usart_tx_main(char *str, USART_Config_t *USART_Config){ //internal function that provides usart port name and data to tx module
    while(*str){
		while (!((*USART_Config).Instance->SR & USART_SR_TXE));
		(*USART_Config).Instance -> DR = *str;
		while (!((*USART_Config).Instance->SR & USART_SR_TC));
		str++;
	}
}


//void USART2_IRQHandler(void) {
//    if (USART2->SR & USART_SR_RXNE) {
//        uint8_t data = USART2->DR;   // reading clears RXNE flag
//
//        if (data == 0x38) {          // if '8' received
//            //usart_tx("received");
//            //GPIOC->ODR ^= GPIO_ODR_ODR13; // toggle LED
//        }
//        //else usart_tx("wrong input hehe");
//    }
//}
