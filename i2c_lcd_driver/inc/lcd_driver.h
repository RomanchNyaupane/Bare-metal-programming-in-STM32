#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

#include "i2c_driver.h"

typedef I2C_Config_t LCD_Config_t;

void lcd_init();
void lcd_print_string(char *);
void lcd_end();

extern LCD_Config_t lcd_config;
#endif /* LCD_DRIVER_H_ */