#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

#include "i2c_driver.h"

typedef I2C_Config_t LCD_Config_t;

void lcd_init();
void lcd_deinit();
void lcd_print_string(char *);
void lcd_end();
void lcd_set_cursor_position(uint8_t row, uint8_t col);
void lcd_clear_screen();
void lcd_reset_cursor_position();
void lcd_set_contrast(uint8_t);
void lcd_shift_display_left(uint8_t);
void lcd_shift_display_right(uint8_t);

extern LCD_Config_t lcd_config;
#endif /* LCD_DRIVER_H_ */