#include "stm32l476xx.h"
#include "i2c_driver.h"
#include "lcd_driver.h"
#include "clocking_wizard.h"

void system_clock(){
    RCC -> CR |= RCC_CR_HSION; //enable internal 16MHz HSI clock
    while(!(RCC -> CR & RCC_CR_HSIRDY)){} //wait until HSI is ready
		RCC -> CFGR |= RCC_CFGR_SW_HSI; //switch to HSI clock
		while(!(RCC -> CFGR & RCC_CFGR_SWS_HSI)){} //check if HSI clock is enabled
    //no prescaler, so system clock = HSI clock = 16MHz. no division of clock
}
LCD_Config_t lcd_config = {
		.Instance = I2C1,
		.speedMode = 0x00503D58,
		.slaveAddress = 0x78
};
//uint8_t name[15] = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x48, 0x61, 0x72, 0x69, 0x68, 0x61, 0x72, 0x21, 0x0A};
uint32_t l;
uint32_t sys_clock, apb1_clock;
int main()
{
		system_clock();
		sys_clock = GetSystemClockFreq();	//gives clock frequency of active clock
		apb1_clock = GetAPB1CLKFreq();	//
		
		lcd_init();
		//lcd_set_cursor_position(0, 4);
		//lcd_print_string("Hello World!");
		//lcd_set_cursor_position(1,2);
		//lcd_print_string("Hello World!");
		//lcd_clear_screen();
		//lcd_reset_cursor_position();
		//lcd_print_string("Hello World!");

		lcd_set_contrast(0);  
		//lcd_shift_display_right(2);
		uint8_t pos;
		int count;
		while(1){
			lcd_print_string("Hello World!");
			for(count = 0; count<222222; count++){} //delay
			for(pos = 0; pos <8; pos++){
				lcd_shift_display_right(1);
				for(count = 0; count<222222; count++){} //delay
			}
			if(pos == 8){ pos = 0; lcd_clear_screen(); lcd_reset_cursor_position();}
		}

}
/*
		remaining work
			0.Setting speed of I2C based on system clock. this task has to be at highest priority
			1.custom characters in cgram
			2.animations and icons
*/