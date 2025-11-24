// LCD driver source file version 1.4
// version 1.4 changes: added functions to set i2c speed based on APB1 clock frequency
// version 1.3 changes: added functions to set contrast and shift display contents
// version 1.2 changes: added function to reset cursor position
// version 1.1 changes: added function to set cursor position and clear screen
// initial version: 1.0
#include "stm32l4xx.h"
#include "i2c_driver.h"
#include "clocking_wizard.h"
#include "lcd_driver.h"
#include "string.h"

/*pixel mapping of 20x2 lcd display
            col0                                                    col10
             ||                                                       ||
             \/                                                       \/  
           _______________________________________________________________
   row_0=> | 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 |
   row_1=> | 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 |
           |_____________________________________________________________|

  DDRAM address mapping(hexadecimal representation)
            col0                                                    col10
             ||                                                       ||
             \/                                                       \/  
           _______________________________________________________________
   row_0=> | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 |
   row_1=> | 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 53 |
           |_____________________________________________________________|   
           
    control word to set DDRAM address: 1xxxxxxx where xxxxxxx is the address
    example: to set cursor at beginning of second row, send command by OR-ing with 0x80( 0x10000000 | command)
    i.e. 0x80 | 0x40 = 0xC0
*/

/* Private macro */
/* Private data structures */
/* Private funcions declarations */

/***************** Public Function Declarations ******************** */
uint8_t init_data[10] = {0x00, 0x38, 0x39, 0x14, 0x78, 0x5E, 0x6D, 0x0C, 0x01, 0x06};
uint8_t i, command, data;	//data always follows command. if command indicates control mode, data following it is contorl signal.
													//if command indicates data mode, data following it is data stream
void lcd_init()
{
    I2C_init(&lcd_config);
    I2C_transfer_data(&lcd_config, init_data,10); //sends command and data in a single data stream. no use f I2C_transfer_command() though possible
		command  = 0x40;	//setting command to 0x40 as a default command to the lcd display. there will be related lcd functions that will modify this command
}
void lcd_deinit()
{
	I2C_deinit(&lcd_config);
}

void lcd_print_string(char *string) 
{
		command = 0x40;
    uint8_t length = strlen(string); //length of data following command. total no. of bytes = 1 + length. this addition is done by the function
		I2C_transfer_command(&lcd_config, command, length);	
    I2C_transfer_data(&lcd_config, (uint8_t*)string, length);
}

void lcd_end()
{
    I2C_deinit(&lcd_config);
}
void lcd_set_cursor_position(uint8_t row, uint8_t col)
{
    if(row == 0){
        command = 0x00;	//control mode
        data = 0x80 | 0x00 | col; //first row (ddram address)
        I2C_transfer_command(&lcd_config, command, 1);
        I2C_transfer_data(&lcd_config, &data, 1);
    }
    else if(row == 1){
        command = 0x00;	//control mode
        data = 0x80 | 0x40 | col; //second row (ddram address)
        I2C_transfer_command(&lcd_config, command, 1);
        I2C_transfer_data(&lcd_config, &data, 1);
    }
}

void lcd_clear_screen()
{
    command = 0x00;
    data = 0x01;
    I2C_transfer_command(&lcd_config, command, 1);
    I2C_transfer_data(&lcd_config, &data, 1);
    
    for(volatile int i = 0; i < 10000; i++); //short delay
}

void lcd_reset_cursor_position()
{
    command = 0x00;
    data = 0x02;
    I2C_transfer_command(&lcd_config, command, 1);
    I2C_transfer_data(&lcd_config, &data, 1);
    
    for(volatile int i = 0; i < 10000; i++); //~2ms delay at 16MHz
}

void lcd_set_contrast(uint8_t contrast_level)
{
    command = 0x00;
    data = 0x39; //use instruction table 1 for contrast
    I2C_transfer_command(&lcd_config, command, 1);
    I2C_transfer_data(&lcd_config, &data, 1);
    
    data = 0x70 | (contrast_level & 0x0F); //set contrast value
    I2C_transfer_command(&lcd_config, command, 1);
    I2C_transfer_data(&lcd_config, &data, 1);
    
    data = 0x38; // return to set instruction table 0
    I2C_transfer_command(&lcd_config, command, 1);
    I2C_transfer_data(&lcd_config, &data, 1);
}

void lcd_shift_display_left(uint8_t positions)
{
    command = 0x00;
    for(uint8_t i = 0; i < positions; i++) {
        data = 0x18; //shift entire display left
        I2C_transfer_command(&lcd_config, command, 1);
        I2C_transfer_data(&lcd_config, &data, 1);
    }
}

void lcd_shift_display_right(uint8_t positions)
{
    command = 0x00; 
    for(uint8_t i = 0; i < positions; i++) {
        data = 0x1C; //shift entire display right
        I2C_transfer_command(&lcd_config, command, 1);
        I2C_transfer_data(&lcd_config, &data, 1);
    }
}



uint32_t set_i2c_speed(void)
{
    uint32_t sys_clock = GetSystemClockFreq();
    uint32_t apb1_clock = GetAPB1CLKFreq();// APB1 peripheral bus clock(I2C clock source)
    
    uint32_t presc, scldel, sdadel, sclh, scll;
    uint32_t timing_value;
    //calculate timing parameters based on APB1 clock frequency
    //target: I2C Standard Mode 100kHz (10us period)
    //t_LOW >= 4.7us, t_HIGH >= 4.0us
    
    if (apb1_clock == 16000000) {
        //16MHz APB1 clock (HSI16)
        presc = 0x3;      //(PRESC+1) = 4, t_PRESC = 250ns
        scll = 0x13;      //(SCLL+1) * t_PRESC = 20 * 250ns = 5.0us (t_LOW)
        sclh = 0xF;      //(SCLH+1) * t_PRESC = 16 * 250ns = 4.0us (t_HIGH)
        sdadel = 0x2;     //SDADEL * t_PRESC = 2 * 250ns = 500ns (data hold time)
        scldel = 0x4;     //(SCLDEL+1) * t_PRESC = 5 * 250ns = 1250ns (data setup time)
        
        //actual SCL frequency ˜ 1/(5.0us + 4.0us + sync_delays) ˜ 100kHz
    }
    else if (apb1_clock == 8000000) {
        //8MHz APB1 clock
        presc = 1;      //(PRESC+1) = 2, t_PRESC = 250ns
        scll = 19;      //20 * 250ns = 5.0us
        sclh = 15;      //16 * 250ns = 4.0us
        sdadel = 2;     //2 * 250ns = 500ns
        scldel = 4;     //5 * 250ns = 1250ns
    }
    else if (apb1_clock == 4000000) {
        //4MHz APB1 clock (MSI default)
        presc = 0;      //(PRESC+1) = 1, t_PRESC = 250ns
        scll = 19;      //20 * 250ns = 5.0us
        sclh = 15;      //16 * 250ns = 4.0us
        sdadel = 2;     //2 * 250ns = 500ns
        scldel = 4;     //5 * 250ns = 1250ns
    }
    else if (apb1_clock == 48000000) {
        //48MHz APB1 clock
        presc = 11;     //(PRESC+1) = 12, t_PRESC = 250ns
        scll = 19;      //20 * 250ns = 5.0us
        sclh = 15;      //16 * 250ns = 4.0us
        sdadel = 2;     //2 * 250ns = 500ns
        scldel = 4;     //5 * 250ns = 1250ns
    }
    else if (apb1_clock == 80000000) {
        //80MHz APB1 clock (max for STM32L476)
        presc = 19;     //(PRESC+1) = 20, t_PRESC = 250ns
        scll = 19;      //20 * 250ns = 5.0us
        sclh = 15;      //16 * 250ns = 4.0us
        sdadel = 2;     //2 * 250ns = 500ns
        scldel = 4;     //5 * 250ns = 1250ns
    }
    else {
        //generic calculation for other frequencies
        //try to achieve t_PRESC = 250ns by calculating appropriate prescaler
        //PRESC = (APB1_CLK / 4000000) - 1
        presc = (apb1_clock / 4000000) - 1;
        if (presc > 15) presc = 15;  // Limit to 4-bit value (max 15)
        
        // Use standard timing values
        scll = 19;      // 5.0us low time
        sclh = 15;      // 4.0us high time
        sdadel = 2;     // 500ns data hold
        scldel = 4;     // 1250ns data setup
    }
    
    //TIMINGR register value
    // bit layout:
    // [31:28] PRESC[3:0]   timing prescaler
    // [27:24] Reserved
    // [23:20] SCLDEL[3:0]  data setup time
    // [19:16] SDADEL[3:0]  data hold time
    // [15:8]  SCLH[7:0]    SCL high period
    // [7:0]   SCLL[7:0]    SCL low period
    
    timing_value = ((presc & 0xF) << 28) |
                   ((scldel & 0xF) << 20) |
                   ((sdadel & 0xF) << 16) |
                   ((sclh & 0xFF) << 8) |
                   (scll & 0xFF);
    
    return timing_value;
}