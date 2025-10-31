#include "stm32f1xx.h"
#include "usart_driver.h"
uint8_t data_buffer[5] = {0x10,0x20,0x30,0x40,0x50};

USART_Config_t USART_Config = {
	.Instance    = USART2,
	.BaudRate    = 9600,
	.WordLength  = 8,
	.StopBits    = 1,
	.Parity      = 0    // 0 = none
};
void main(){
	usart_init(&USART_Config);
	dma_config();
	while(1){};
}


//dma configuration
void dma_config(){
    RCC -> AHBENR |= RCC_AHBENR_DMA1EN; //enable clock for dma1
//-----------------------------------------------------------------//
//-->>>    //EACH CHANNEL HAS ITS OWN SET OF REGISTERS        <<<--//
//-----------------------------------------------------------------//
    //channel configuration register - usart is at channel 7 so channel 7 registers should be configured
    DMA1_Channel7 -> CCR |= DMA_CCR_MINC; //memory increment mode
    DMA1_Channel7 -> CCR |= DMA_CCR_DIR; //read from memory and write to peripheral
    DMA1_Channel7 -> CCR |= DMA_CCR_TEIE //transfer error interrupt enable
                            | DMA_CCR_TCIE  //transfer complete interrupt enable
                            | DMA_CCR_HTIE; //half transfer interrupt enable


    //number of data register
    uint16_t data_length = 5; //number of data to be transferred
    //after enabling the channel, this register is read only and decremented after each data transfer
    DMA1_Channel7 -> CNDTR = data_length;

    //peripheral address register
    uint32_t peripheral_address = (uint32_t)(&(USART2 -> DR)); //or (uint32_t)((*usart2).DR) or 0x40004404 directly
    DMA1_Channel7 -> CPAR = peripheral_address;

    //memory address register
    uint32_t *memory_address = &data_buffer[0]; // or *memory address = data_buffer; instead
    DMA1_Channel7 -> CMAR = (uint32_t)memory_address;


    NVIC_SetPriority(DMA1_Channel7_IRQn, 1);
    NVIC_EnableIRQ(DMA1_Channel7_IRQn);

    DMA1_Channel7 -> CCR |= DMA_CCR_EN; //enable dma channel
}

//dma1 channel1 interrupt handler

void DMA1_Channel7_IRQHandler(void) {
    // Clear ALL possible flags at once to prevent repeated interrupts
    DMA1->IFCR = DMA_IFCR_CGIF7 | DMA_IFCR_CTCIF7 | DMA_IFCR_CHTIF7 | DMA_IFCR_CTEIF7;

    // Simple LED toggle to confirm ISR execution
    GPIOC->ODR ^= GPIO_ODR_ODR13;

    // Optional: Disable DMA after completion to prevent repeated transfers
    // DMA1_Channel7->CCR &= ~DMA_CCR_EN;
}
