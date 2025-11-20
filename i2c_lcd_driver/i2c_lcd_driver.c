#include "stm32l4xx.h"

void system_clock(void);
void peripheral_setup(void);
void I2C_init();
void I2C_scan();
void LCD_Init();
void LCD_example();

volatile uint8_t err[128];
volatile int i = 0;


void I2C_delay(){
    for (volatile int i = 0; i < 2000; i++);
}

int main(){

    system_clock();
    peripheral_setup();
    I2C_init();
		
		//I2C_scan();
		LCD_Init();
		LCD_example();
    while (1) {

		}
}

void system_clock(){
    RCC -> CR |= RCC_CR_HSION; //select internal 16MHz HSI clock
    while(!(RCC -> CR & RCC_CR_HSIRDY)); //wait until HSI is ready
    //no prescaler, so system clock = HSI clock = 16MHz. no division of clock
}

//i2c is present on APB1 bus
void peripheral_setup(){
    RCC -> AHB2ENR |= RCC_AHB2ENR_GPIOBEN; //enable GPIOB peripheral clock
    //pins PB6 and PB8 both can be used for scl and PB7 and PB9 both can be used for SDA. all pins are under AF4
    //two pins exist for scl and and two for sda
    //choosing PB8 for SCL and PB9 for SDA
    //GPIOB -> MODER |= GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1; //set PB8,PB9 to alternate function mode <- it took me nine hours of debugging to figure out this caused pb3 and pb4 in alternate function instead of pb8 and pb9
		GPIOB -> MODER  = 0xFFFAFEBF;
    GPIOB-> OTYPER |= GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9; //set open drain
		GPIOB -> OSPEEDR |= GPIO_OSPEEDR_OSPEED8_1
                      | GPIO_OSPEEDR_OSPEED8_0
                      | GPIO_OSPEEDR_OSPEED9_1
                      | GPIO_OSPEEDR_OSPEED9_0; //set high speed for PB8 and PB9
    GPIOB ->PUPDR |= GPIO_PUPDR_PUPD8_0 | GPIO_PUPDR_PUPD9_0; //enable pull up resistor for PB8
    GPIOB -> AFR[1] |= GPIO_AFRH_AFSEL8_2 | GPIO_AFRH_AFSEL9_2; //set AF4 for PB8,PB9
}

void I2C_init(){
	  RCC -> APB1ENR1 |= RCC_APB1ENR1_I2C1EN; //enable i2c1 peripheral clock
	  I2C1 -> CR1 &= ~I2C_CR1_PE;
		//master at default is at write request mode
    I2C1 -> TIMINGR = 0x00503D58; //standard mode 100kHz @16MHz clock
		//I2C1 -> CR2 |= 0x00010000; //set NBYTES = 1 indicating single byte to be transferred
		//I2C1 -> OAR1 |= I2C_OAR1_OA1EN; //own address 1 enable
		//I2C1 -> CR1 |= I2C_CR1_NACKIE; //enable no acnkowledgement interrupt
		//I2C1 -> CR1 |= I2C_CR1_TXIE;
    //the analog noise filter must be enabled and it is enabled by default
    I2C1 -> CR1 |= I2C_CR1_PE;
}


void I2C_scan(){//use this function in debug mode to scan device
		uint8_t addr;
		for (addr = 1; addr < 127; addr++) {

				I2C1->CR2 = 0;
				I2C1->CR2 |= (addr);        // 7-bit address shifted
				I2C1->CR2 |= (0 << 16);          // NBYTES = 0 (address only)
				I2C1->CR2 |= I2C_CR2_AUTOEND;    // Generate STOP automatically
				I2C1->CR2 |= I2C_CR2_START;      // Generate START
				//for(i=0; i<10000;i++){}
				// Wait for either NACK or transfer complete
				while (!(I2C1->ISR & (I2C_ISR_NACKF | I2C_ISR_STOPF))) {}
				
				if (I2C1->ISR & I2C_ISR_NACKF) {
						err[addr] = 0;  // No device at this address
						I2C1->ICR |= I2C_ICR_NACKCF;  // Clear NACK flag
				}
				else {
						err[addr] = 1;  // Device found!
				}
				
				// Wait for STOP to complete
				while (!(I2C1->ISR & I2C_ISR_STOPF));
				I2C1->ICR |= I2C_ICR_STOPCF;  // Clear STOP flag
				
				I2C_delay();  // Small delay between scans
		}
 }

void LCD_Init(){
		I2C1 -> CR2 |= 0x020A0078; //set NBYTES = 2 and autoreload and address = 0x78
    I2C1 -> CR2 |= I2C_CR2_START; //generate start condition
		//txdr_flush = I2C1 -> TXDR;
    while((I2C1 -> ISR & I2C_ISR_NACKF)){} //wait until NACK flag is reset
    while(!(I2C1 -> ISR & I2C_ISR_TXIS)){} //wait until TXIS flag is set
    I2C1 -> TXDR = 0x00; //send control byte
    while(!(I2C1 -> ISR & I2C_ISR_TXIS)){} //wait until TXIS flag is set
    I2C1 -> TXDR = 0x38; //function set:
    while(!(I2C1 -> ISR & I2C_ISR_TXIS)){} //wait until TXIS flag is set
    I2C1 -> TXDR = 0x39; //function set:
    while(!(I2C1 -> ISR & I2C_ISR_TXIS)){} //wait until TXIS flag is set
    I2C1 -> TXDR = 0x14; //function set:
    while(!(I2C1 -> ISR & I2C_ISR_TXIS)){} //wait until TXIS flag is set
    I2C1 -> TXDR = 0x78; //function set:
    while(!(I2C1 -> ISR & I2C_ISR_TXIS)){} //wait until TXIS flag is set
    I2C1 -> TXDR = 0x5E; //function set:
    while(!(I2C1 -> ISR & I2C_ISR_TXIS)){} //wait until TXIS flag is set
    I2C1 -> TXDR = 0x6D; //function set:
    while(!(I2C1 -> ISR & I2C_ISR_TXIS)){} //wait until TXIS flag is set
    I2C1 -> TXDR = 0x0C; //function set:
    while(!(I2C1 -> ISR & I2C_ISR_TXIS)){} //wait until TXIS flag is set
    I2C1 -> TXDR = 0x01; //function set:
    while(!(I2C1 -> ISR & I2C_ISR_TXIS)){} //wait until TXIS flag is set
    I2C1 -> TXDR = 0x06; //function set:
		
		if(I2C1 -> ISR & I2C_ISR_STOPF) I2C1 -> ICR |= I2C_ICR_STOPCF;
}

void LCD_example(){
		
}