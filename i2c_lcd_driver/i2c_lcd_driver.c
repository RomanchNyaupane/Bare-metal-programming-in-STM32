#include "stm32l476xx.h"

#include "i2c_driver.h"
#include "lcd_driver.h"

void system_clock(){
    RCC -> CR |= RCC_CR_HSION; //select internal 16MHz HSI clock
    while(!(RCC -> CR & RCC_CR_HSIRDY)); //wait until HSI is ready
    //no prescaler, so system clock = HSI clock = 16MHz. no division of clock
}

LCD_Config_t lcd_config = {
		.Instance = I2C1,
		.speedMode = 0x00503D58,
		.slaveAddress = 0x78
};
//uint8_t name[15] = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x48, 0x61, 0x72, 0x69, 0x68, 0x61, 0x72, 0x21, 0x0A};

int main(){
		system_clock();
		lcd_init();
		lcd_print_string("Hello World!");
}

/*
		remaining work
			0.Setting speed of I2C based on system clock. this task has to be at highest priority
			1.custom characters in cgram
			2.changing cursor position
			3.function to print character at specific areas of display
			3.shifting display contents
			4.animations and icons
*/