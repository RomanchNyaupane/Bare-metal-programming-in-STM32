// LCD driver source file version 1.3
// version 1.3 changes: added functions to set contrast and shift display contents
// version 1.2 changes: added function to reset cursor position
// version 1.1 changes: added function to set cursor position and clear screen
// initial version: 1.0
#include "stm32l4xx.h"
#include "i2c_driver.h"
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