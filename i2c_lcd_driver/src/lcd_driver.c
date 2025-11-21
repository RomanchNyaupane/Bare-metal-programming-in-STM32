#include "stm32l4xx.h"
#include "i2c_driver.h"
#include "lcd_driver.h"
#include "string.h"

/* Private macro */
/* Private data structures */
/* Private funcions declarations */

/*****************Public Function Declarations ******************** */
uint8_t init_data[10] = {0x00, 0x38, 0x39, 0x14, 0x78, 0x5E, 0x6D, 0x0C, 0x01, 0x06};
uint8_t i, command;
void lcd_init(){
    I2C_init(&lcd_config);
    I2C_transfer(&lcd_config, init_data,10);
}

void lcd_print_string(char *string) {
    uint8_t length = strlen(string);
		I2C_transfer_command(&lcd_config, 0x40, length);
    I2C_transfer(&lcd_config, (uint8_t*)string, length);
}

void lcd_end(){
    I2C_deinit(&lcd_config);
}