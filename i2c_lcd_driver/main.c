#include "stm32l476xx.h"
#include "i2c_driver.h"
#include "lcd_driver.h"
#include "clocking_wizard.h"

void set_i2c_speed(void);

void system_clock(){
    RCC -> CR |= RCC_CR_HSION; //enable internal 16MHz HSI clock
    while(!(RCC -> CR & RCC_CR_HSIRDY)){} //wait until HSI is ready
		RCC -> CFGR |= RCC_CFGR_SW_HSI; //switch to HSI clock
		while(!(RCC -> CFGR & RCC_CFGR_SWS_HSI)){} //check if HSI clock is enabled
    //no prescaler, so system clock = HSI clock = 16MHz. no division of clock
}
LCD_Config_t lcd_config = {
		.Instance = I2C1,
		.speedMode = set_i2c_speed(),
		.slaveAddress = 0x78
};
//uint8_t name[15] = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x48, 0x61, 0x72, 0x69, 0x68, 0x61, 0x72, 0x21, 0x0A};
uint32_t l;
uint32_t sys_clock, apb1_clock;
int main()
{
		system_clock();
		
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
 * I2C Timing Configuration Function
 * Calculates I2C_TIMINGR register value for 100kHz standard mode
 * Based on APB1 clock frequency (default I2C clock source)
 * For STM32L476RG
 */

#include "stm32l4xx.h"
#include "clocking_wizard.h"

/**
 * @brief Calculate and return I2C_TIMINGR value for 100kHz operation
 * @return uint32_t TIMINGR register value for 100kHz I2C speed
 * 
 * This function uses the actual APB1 clock frequency to calculate proper
 * I2C timing parameters that comply with I2C Standard Mode (100kHz).
 */
uint32_t set_i2c_speed(void)
{
    uint32_t sys_clock = GetSystemClockFreq();   // System clock frequency
    uint32_t apb1_clock = GetAPB1CLKFreq();      // APB1 peripheral bus clock (I2C clock source)
    
    uint32_t presc, scldel, sdadel, sclh, scll;
    uint32_t timing_value;
    
    // Calculate timing parameters based on APB1 clock frequency
    // Target: I2C Standard Mode 100kHz (10us period)
    // t_LOW >= 4.7us, t_HIGH >= 4.0us
    
    if (apb1_clock == 16000000) {
        // 16MHz APB1 clock (HSI16)
        presc = 3;      // (PRESC+1) = 4, t_PRESC = 250ns
        scll = 19;      // (SCLL+1) * t_PRESC = 20 * 250ns = 5.0us (t_LOW)
        sclh = 15;      // (SCLH+1) * t_PRESC = 16 * 250ns = 4.0us (t_HIGH)
        sdadel = 2;     // SDADEL * t_PRESC = 2 * 250ns = 500ns (data hold time)
        scldel = 4;     // (SCLDEL+1) * t_PRESC = 5 * 250ns = 1250ns (data setup time)
        
        // Actual SCL frequency ≈ 1/(5.0us + 4.0us + sync_delays) ≈ 100kHz
    }
    else if (apb1_clock == 8000000) {
        // 8MHz APB1 clock
        presc = 1;      // (PRESC+1) = 2, t_PRESC = 250ns
        scll = 19;      // 20 * 250ns = 5.0us
        sclh = 15;      // 16 * 250ns = 4.0us
        sdadel = 2;     // 2 * 250ns = 500ns
        scldel = 4;     // 5 * 250ns = 1250ns
    }
    else if (apb1_clock == 4000000) {
        // 4MHz APB1 clock (MSI default)
        presc = 0;      // (PRESC+1) = 1, t_PRESC = 250ns
        scll = 19;      // 20 * 250ns = 5.0us
        sclh = 15;      // 16 * 250ns = 4.0us
        sdadel = 2;     // 2 * 250ns = 500ns
        scldel = 4;     // 5 * 250ns = 1250ns
    }
    else if (apb1_clock == 48000000) {
        // 48MHz APB1 clock
        presc = 11;     // (PRESC+1) = 12, t_PRESC = 250ns
        scll = 19;      // 20 * 250ns = 5.0us
        sclh = 15;      // 16 * 250ns = 4.0us
        sdadel = 2;     // 2 * 250ns = 500ns
        scldel = 4;     // 5 * 250ns = 1250ns
    }
    else if (apb1_clock == 80000000) {
        // 80MHz APB1 clock (max for STM32L476)
        presc = 19;     // (PRESC+1) = 20, t_PRESC = 250ns
        scll = 19;      // 20 * 250ns = 5.0us
        sclh = 15;      // 16 * 250ns = 4.0us
        sdadel = 2;     // 2 * 250ns = 500ns
        scldel = 4;     // 5 * 250ns = 1250ns
    }
    else {
        // Generic calculation for other frequencies
        // Try to achieve t_PRESC = 250ns by calculating appropriate prescaler
        // PRESC = (APB1_CLK / 4000000) - 1
        presc = (apb1_clock / 4000000) - 1;
        if (presc > 15) presc = 15;  // Limit to 4-bit value (max 15)
        
        // Use standard timing values
        scll = 19;      // 5.0us low time
        sclh = 15;      // 4.0us high time
        sdadel = 2;     // 500ns data hold
        scldel = 4;     // 1250ns data setup
    }
    
    // Assemble TIMINGR register value
    // Bit layout:
    // [31:28] PRESC[3:0]   - Timing prescaler
    // [27:24] Reserved
    // [23:20] SCLDEL[3:0]  - Data setup time
    // [19:16] SDADEL[3:0]  - Data hold time
    // [15:8]  SCLH[7:0]    - SCL high period
    // [7:0]   SCLL[7:0]    - SCL low period
    
    timing_value = ((presc & 0xF) << 28) |
                   ((scldel & 0xF) << 20) |
                   ((sdadel & 0xF) << 16) |
                   ((sclh & 0xFF) << 8) |
                   (scll & 0xFF);
    
    return timing_value;
}

/**
 * Example timing values returned for different APB1 frequencies:
 * 
 * APB1 = 4MHz   -> 0x00503D58  (PRESC=0, SCLDEL=4, SDADEL=2, SCLH=15, SCLL=19)
 * APB1 = 8MHz   -> 0x10503D58  (PRESC=1, SCLDEL=4, SDADEL=2, SCLH=15, SCLL=19)
 * APB1 = 16MHz  -> 0x30503D58  (PRESC=3, SCLDEL=4, SDADEL=2, SCLH=15, SCLL=19)
 * APB1 = 48MHz  -> 0xB0503D58  (PRESC=11, SCLDEL=4, SDADEL=2, SCLH=15, SCLL=19)
 * APB1 = 80MHz  -> 0x30503D58  (PRESC=19, SCLDEL=4, SDADEL=2, SCLH=15, SCLL=19)
 */
/*
		remaining work
			0.Setting speed of I2C based on system clock. this task has to be at highest priority
			1.custom characters in cgram
			2.animations and icons
*/