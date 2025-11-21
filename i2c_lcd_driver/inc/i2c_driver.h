//I2C driver version 1.0
#ifndef I2C_DRIVER_H_
#define I2C_DRIVER_H_
#include "stm32l4xx.h"

/*
    Parameters required for I2C configuration (only master transmitter mode, 7 bit address for v1)
    1. I2C Instance (I2C1, I2C2 etc.)
    2. I2C Speed Mode value 
    3. Slave address
*/


typedef struct
{
    I2C_TypeDef *Instance;
    uint32_t speedMode; //I2C speed mode
    uint8_t slaveAddress; //7 bit slave address
} I2C_Config_t;

void I2C_init (I2C_Config_t *);
void I2C_deinit (I2C_Config_t *);
void I2C_transfer (I2C_Config_t *, uint8_t *, uint8_t);
void I2C_transfer_command (I2C_Config_t *, uint8_t, uint8_t);
//peripheral setup function will be called inside init function. so not exposed to user and not declared here

extern I2C_Config_t I2C_Config;
#endif /* I2C_DRIVER_H_ */