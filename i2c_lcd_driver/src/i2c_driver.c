#include "stm32l4xx.h"
#include "i2c_driver.h"

/* Private macro */

/* Private data structures */

/* Private funcions declarations */
static void _peripheral_setup (I2C_Config_t *i2cConfig);
void _I2C_transfer_init(I2C_Config_t *, uint8_t);
void I2C_transfer_command(I2C_Config_t *, uint8_t, uint8_t);
/* Private variables */
static uint8_t transfer_start_flag = 0;
//void i2c_init ( i2c_confif_t * ps_i2c_cfg )
/****************************************************** Public function definitions  ********************************************/
void I2C_init (I2C_Config_t *i2cConfig)
{
    _peripheral_setup(i2cConfig);
    (*i2cConfig).Instance -> CR1 &= ~I2C_CR1_PE;
        //master at default is at write request mode
    (*i2cConfig).Instance -> TIMINGR = (*i2cConfig).speedMode; //set speed mode
    (*i2cConfig).Instance -> CR1 |= I2C_CR1_PE; //enable I2C peripheral
}

void I2C_deinit(I2C_Config_t *i2cConfig)
{
    (*i2cConfig).Instance-> CR1 &= ~I2C_CR1_PE; //disable I2C peripheral
    if((*i2cConfig).Instance == I2C1){
        RCC -> APB1RSTR1 |= RCC_APB1RSTR1_I2C1RST; //reset i2c1 peripheral
        RCC -> APB1RSTR1 &= ~RCC_APB1RSTR1_I2C1RST; //release reset
        RCC -> AHB2RSTR |= RCC_AHB2RSTR_GPIOBRST; //reset GPIOB peripheral
        RCC -> AHB2RSTR &= ~RCC_AHB2RSTR_GPIOBRST; //release reset
    }
    else if((*i2cConfig).Instance == I2C2){
        RCC -> APB1RSTR1 |= RCC_APB1RSTR1_I2C2RST; //reset i2c2 peripheral
        RCC -> APB1RSTR1 &= ~RCC_APB1RSTR1_I2C2RST; //release reset
        RCC -> AHB2RSTR |= RCC_AHB2RSTR_GPIOBRST; //reset GPIOB peripheral
        RCC -> AHB2RSTR &= ~RCC_AHB2RSTR_GPIOBRST; //release reset
    }
    else if((*i2cConfig).Instance == I2C3){
        RCC -> APB1RSTR1 |= RCC_APB1RSTR1_I2C3RST; //reset i2c3 peripheral
        RCC -> APB1RSTR1 &= ~RCC_APB1RSTR1_I2C3RST; //release reset
        RCC -> AHB2RSTR |= RCC_AHB2RSTR_GPIOCRST; //reset GPIOC peripheral
        RCC -> AHB2RSTR &= ~RCC_AHB2RSTR_GPIOCRST; //release reset
    }
}

//if data stream follows a command, data size should be provided in command because command transmit function also initializes
//the NBYTES register during i2c setup. after providing command, the command does the i2c transaction setup for the rest of
//following data stream. If data stream follows a command, datasize parameter in I2C_transfer() does not matter
void I2C_transfer(I2C_Config_t *i2cConfig, uint8_t *data, uint8_t dataSize)
{
		if(transfer_start_flag == 0){	//to separate the case when the data transfer is independent and the case when slave requires a command before main data stream
			_I2C_transfer_init(i2cConfig, dataSize);	//if init was previously done, then reinit may corrupt the existing setup
		}
    for(uint8_t i = 0; i < dataSize; i++){
        while(!((*i2cConfig).Instance -> ISR & I2C_ISR_TXIS)); //wait until TXIS flag is set
        (*i2cConfig).Instance -> TXDR = data[i]; //send data byte
    }

    while(!((*i2cConfig).Instance -> ISR & I2C_ISR_TC)); //wait until transfer complete flag is set
    (*i2cConfig).Instance -> CR2 |= I2C_CR2_STOP; //generate stop condition

    while(!((*i2cConfig).Instance -> ISR & I2C_ISR_STOPF)); //wait until STOP flag is set
    (*i2cConfig).Instance -> ICR |= I2C_ICR_STOPCF; //clear STOP flag
		
		transfer_start_flag = 0;
}
void I2C_transfer_command(I2C_Config_t *i2cConfig, uint8_t command, uint8_t length)
{
		_I2C_transfer_init(i2cConfig, length + 1);
		while(!((*i2cConfig).Instance -> ISR & I2C_ISR_TXIS)){} //wait until TXIS flag is set
        (*i2cConfig).Instance -> TXDR = command; //send data byte
}





    //RCC -> AHB2ENR |= RCC_AHB2ENR_GPIOBEN; //enable GPIOB peripheral clock
    //pins PB6 and PB8 both can be used for scl and PB7 and PB9 both can be used for SDA. all pins are under AF4
    //two pins exist for scl and and two for sda
    //choosing PB8 for SCL and PB9 for SDA
    //GPIOB -> MODER |= GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1; //set PB8,PB9 to alternate function mode <- it took me nine hours of debugging to figure out this caused pb3 and pb4 in alternate function instead of pb8 and pb9
	// GPIOB -> MODER  = 0xFFFAFEBF;
    // GPIOB-> OTYPER |= GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9; //set open drain
	// 	GPIOB -> OSPEEDR |= GPIO_OSPEEDR_OSPEED8_1
    //                   | GPIO_OSPEEDR_OSPEED8_0
    //                   | GPIO_OSPEEDR_OSPEED9_1
    //                   | GPIO_OSPEEDR_OSPEED9_0; //set high speed for PB8 and PB9
    // GPIOB ->PUPDR |= GPIO_PUPDR_PUPD8_0 | GPIO_PUPDR_PUPD9_0; //enable pull up resistor for PB8
    // GPIOB -> AFR[1] |= GPIO_AFRH_AFSEL8_2 | GPIO_AFRH_AFSEL9_2; //set AF4 for PB8,PB9
    
/****************************************** Private function definitions  ********************************************/
static void _peripheral_setup (I2C_Config_t *i2cConfig)
{
    if(i2cConfig->Instance == I2C1){
        RCC -> APB1ENR1 |= RCC_APB1ENR1_I2C1EN; //enable i2c1 peripheral clock
        RCC -> AHB2ENR |= RCC_AHB2ENR_GPIOBEN; //enable GPIOB peripheral clock
        GPIOB -> MODER = 0xFFFAFEBF; //set PB8,PB9 to alternate function mode for I2C1
        GPIOB-> OTYPER |= GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9; //set open drain
		GPIOB -> OSPEEDR |= GPIO_OSPEEDR_OSPEED8_1
                      | GPIO_OSPEEDR_OSPEED8_0
                      | GPIO_OSPEEDR_OSPEED9_1
                      | GPIO_OSPEEDR_OSPEED9_0; //set high speed for PB8 and PB9
        GPIOB ->PUPDR |= GPIO_PUPDR_PUPD8_0 | GPIO_PUPDR_PUPD9_0; //enable pull up resistor
        GPIOB -> AFR[1] |= GPIO_AFRH_AFSEL8_2 | GPIO_AFRH_AFSEL9_2; //set AF4 for PB8,PB9

    }
    else if(i2cConfig->Instance == I2C2){
        RCC -> APB1ENR1 |= RCC_APB1ENR1_I2C2EN; //enable i2c2 peripheral clock
        RCC -> AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
        GPIOB -> MODER = 0xFFFAFEBF; //set PB10,PB11 to alternate function mode for I2C2
        GPIOB-> OTYPER |= GPIO_OTYPER_OT10 | GPIO_OTYPER_OT11; //set open drain
		GPIOB -> OSPEEDR |= GPIO_OSPEEDR_OSPEED10_1
                        | GPIO_OSPEEDR_OSPEED10_0
                        | GPIO_OSPEEDR_OSPEED11_1
                        | GPIO_OSPEEDR_OSPEED11_0; //set high speed for PB10 and PB11
        GPIOB ->PUPDR |= GPIO_PUPDR_PUPD10_0 | GPIO_PUPDR_PUPD11_0; //enable pull up resistor
        GPIOB -> AFR[1] |= GPIO_AFRH_AFSEL10_2 | GPIO_AFRH_AFSEL11_2; //set AF4 for PB10,PB11
    }
    else if(i2cConfig->Instance == I2C3){
        RCC -> APB1ENR1 |= RCC_APB1ENR1_I2C3EN; //enable i2c3 peripheral clock
        RCC -> AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
        GPIOC -> MODER = 0xFFFF3FFF; //set PC0,PC1 to alternate function mode for I2C3
        GPIOB -> OSPEEDR |= GPIO_OSPEEDR_OSPEED0_1
                      | GPIO_OSPEEDR_OSPEED0_0
                      | GPIO_OSPEEDR_OSPEED1_1
                      | GPIO_OSPEEDR_OSPEED1_0; //set high speed for PC0 and PC1
        GPIOB ->PUPDR |= GPIO_PUPDR_PUPD0_0 | GPIO_PUPDR_PUPD1_0; //enable pull up resistor
        GPIOB -> AFR[0] |= GPIO_AFRL_AFSEL0_2 | GPIO_AFRL_AFSEL1_2; //set AF4 for PC0,PC1
    }
}

void _I2C_transfer_init(I2C_Config_t *i2cConfig, uint8_t dataSize)
{
		(*i2cConfig).Instance -> CR2 = 0; //clear CR2 register
    (*i2cConfig).Instance -> CR2 |= ((*i2cConfig).slaveAddress); //set slave address
    (*i2cConfig).Instance -> CR2 |= (dataSize << 16); //set NBYTES
    (*i2cConfig).Instance -> CR2 |= I2C_CR2_START; //generate start condition
		transfer_start_flag = 1;
}