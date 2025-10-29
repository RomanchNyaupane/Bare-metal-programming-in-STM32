#include "usart_driver.h"


    USART_Config_t USART_Config = {

    .Instance    = USART2,
    .BaudRate    = 9600,
    .WordLength  = 8,
    .StopBits    = 1,
    .Parity      = 0    // 0 = none
    };

int main(void)
{
    usart_init(&USART_Config);       // initialize USART2

    while (1)
    {
        uint8_t data = usart_rx();    // wait for incoming data

        if (data == 0x38)              // if 'A' received
        {
            usart_tx("Hello");
            GPIOC->ODR ^= GPIO_ODR_ODR13;   // toggle onboard LED
        }
        else usart_tx("system uninitialized");

    }
}
