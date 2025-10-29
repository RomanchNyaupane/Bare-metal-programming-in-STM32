#ifndef USART_DRIVER_H
#define USART_DRIVER_H

#include <stm32f1xx.h>

typedef struct{
    USART_TypeDef *Instance;
    uint32_t BaudRate; //baud rate is not a uint8_t as it can take large values
    uint8_t WordLength;
    uint8_t StopBits;
    uint8_t Parity;
}USART_Config_t;

void system_setup(USART_Config_t *usartConfig);
void usart_init(USART_Config_t *usartConfig);
void usart_tx(char *str); //exposed to user
void usart_tx_main(char *str, USART_Config_t *usartConfig); //internal function that provides usart port name and data to tx module
uint8_t usart_rx(void); //exposed to user
uint8_t usart_rx_main(USART_Config_t *usartConfig); //internal function that provides usart port name to receive data from

extern USART_Config_t USART_Config;
#endif
