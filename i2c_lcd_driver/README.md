# I2C LCD Driver Library for STM32L4xx

A lightweight I2C-based LCD driver library for STM32L4xx microcontrollers, supporting 20x2 character LCD displays with I2C interface.

## Features

### I2C Driver (v1.0)
- Master transmitter mode with 7-bit slave addressing
- Configurable I2C instances (I2C1, I2C2, I2C3)
- Adjustable speed modes via TIMINGR register
- Automatic GPIO configuration (I2C1: PB8/PB9, I2C2: PB10/PB11, I2C3: PC0/PC1)
- Data and command transfer functions

### LCD Driver (v1.3)
- **Display Control**: Initialize, clear screen, and deinitialize LCD
- **Text Output**: Print strings at current or specified cursor positions
- **Cursor Management**: Set cursor position (row, column) and reset to home
- **Display Shifting**: Shift entire display content left or right
- **Contrast Adjustment**: Set display contrast (0-15 levels)
- **20x2 Display Support**: Optimized for 20 columns × 2 rows character LCDs

## Hardware Requirements

- STM32L476xx microcontroller (or compatible L4 series)
- I2C-compatible 20x2 LCD display
- Default slave address: 0x78 (configurable)
- Pull-up resistors on I2C lines (enabled internally)

## Usage Example

```c
#include "lcd_driver.h"

// Configure LCD with I2C1, custom speed, and slave address
LCD_Config_t lcd_config = {
    .Instance = I2C1,
    .speedMode = 0x00503D58,  // Timing value for desired speed
    .slaveAddress = 0x78
};

int main() {
    system_clock();
    lcd_init();
    
    lcd_clear_screen();
    lcd_set_cursor_position(0, 0);  // Row 0, Column 0
    lcd_print_string("Hello World!");
    
    lcd_set_cursor_position(1, 5);  // Row 1, Column 5
    lcd_print_string("STM32L4");
    
    lcd_set_contrast(10);
    lcd_shift_display_right(2);
    
    // Clean up
    lcd_deinit();
}
```

## API Reference

### LCD Functions
- `lcd_init()` - Initialize LCD with predefined configuration
- `lcd_deinit()` - Deinitialize LCD and I2C peripheral
- `lcd_print_string(char*)` - Print string at current cursor position
- `lcd_set_cursor_position(row, col)` - Position cursor (row: 0-1, col: 0-19)
- `lcd_clear_screen()` - Clear entire display
- `lcd_reset_cursor_position()` - Return cursor to home (0,0)
- `lcd_set_contrast(level)` - Adjust contrast (0-15)
- `lcd_shift_display_left(positions)` - Shift display content left
- `lcd_shift_display_right(positions)` - Shift display content right

### I2C Functions
- `I2C_init(I2C_Config_t*)` - Initialize I2C peripheral
- `I2C_deinit(I2C_Config_t*)` - Reset I2C peripheral
- `I2C_transfer_data(config, data, size)` - Transfer data bytes
- `I2C_transfer_command(config, cmd, length)` - Send command with data length

## Known Limitations

1. **Speed Configuration**: I2C timing values are hardcoded and not automatically calculated based on system clock frequency (noted as highest priority work item)

2. **Display Size**: Only supports 20x2 LCD displays; DDRAM addressing is hardcoded for this format

3. **I2C Mode**: Master transmitter mode only; no slave or receiver modes

4. **Address Width**: 7-bit addressing only; 10-bit addressing not supported

5. **GPIO Limitation**: I2C3 implementation has bugs - uses GPIOB registers instead of GPIOC for PC0/PC1 configuration

6. **Missing Features**:
   - No custom character creation (CGRAM programming)
   - No animation or icon support
   - No error handling or timeout mechanisms
   - No backlight control
   - No read operations from LCD

7. **Blocking Operations**: All I2C transfers use polling (busy-wait) without DMA or interrupt support

8. **Fixed Delays**: Uses crude delay loops instead of timer-based delays

9. **Limited Documentation**: No detailed timing specifications or hardware connection diagrams

## System Requirements

- System clock: 16MHz HSI (as implemented in example)
- Proper I2C timing calculation needed for different clock speeds
- External pull-up resistors recommended despite internal pull-ups

## Future Improvements

- Auto-calculate I2C timing based on system clock
- CGRAM support for custom characters
- Interrupt/DMA-based transfers
- Multi-size display support
- Error handling and recovery
- Backlight PWM control

## License

Version 1.3 - Basic functionality for embedded LCD control applications.
