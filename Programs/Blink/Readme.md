Blink is Hello world of embedded systems. This is an efficient way of getting grasp on IO and underlying registers controlling them.
The implementation is done using GPIO, Timers and Counters. Lets take a look at architecture of the board
used.
The board in use is STM32F103CBT6 (commonly known as blue pill). Taking a look at datasheet of STM32F103xx,
the architecture is represented by block diagram shown below.

![image](https://github.com/user-attachments/assets/d72cdacb-6168-43aa-867d-49f0161045a3)

One can observe the placement of peripherals (GPIOx, Timers, USARTx, SPIx, CANx etc) and categories of bus associated with them. They are:
1. APB2 (Advanced Peripheral Bus, high speed)
2. APB1 (Advanced Peripheral Bus, low speed)
3. AHB (Advanced High speed bus)

To control a peripheral, it is necessary to setup bus for that specific peripheral.

I. Simple blink program

For the simple Blink program, the approach taken listed below:

  a. Enable clock for GPIO peripheral
  
  b. Configure specific pin as general purpose output push-pull
  
  c. Toggle the led and apply a delay function inside a loop

To enable clock for GPIOA (or any other peripheral), the RCC (Reset and Clock Control) register should be used.
Since GPIOA is connected to APB2 bus, clock for the GPIOA can be triggered using APB2 peripheral clock enable register (RCC_APB2ENR).

![image](https://github.com/user-attachments/assets/a7a44e9c-6a58-4277-9c6d-6850adb111b1)

The IOPAEN ( I/O port A clock enable) bit on the register should be set to enable clock for GPIOA. This looks like "RCC->APB2ENR |= RCC_APB2ENR_IOPAEN" in the code.


Now that the port is set up, the GPIOA can be configured. This is done using GPIO registers. The behaviour of register (Input, Output, Floating, Push-Pull, Open drain, Frequency)
is defined using Port configuration register low (GPIOx_CRL) and Port configuration register high (GPIOx_CRH). For pin PA0, GPIOx_CRL should be used.

![image](https://github.com/user-attachments/assets/61e712cb-e39b-4c74-8929-5ed9383c7b3a)

Since pin PA0 operates in output mode (General purpose output push-pull(CNF) and 10Mhz(Mode)), the bits in register
is CNF0[1:0] = 00, and MODE0[1:0] = 01. These bits can be set as:

  GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0); //clears MODE0 and CNF0 bits and remaining bits are unchanged
  
  GPIOA->CRL |= GPIO_CRL_MODE0_0; //since MODE[0] is only bit containing 1, this line is sufficient to define output behavior of PA0

 Now the delay function can be defined and called after toggling PA0 inside a loop.

 while(1){
		GPIOA->ODR ^= GPIO_ODR_ODR0;  //toggles ODR.
		delay(500)
	}

The toggling can be done using Output Data Register (GPIOA_ODR) or using Bit Set/Reset Register (GPIOA_BSRR). 

II. Blink using built in timers

Algorithm:
1. Setup internal HSI clock.
   
2. Configure timer and timer registers.
   
3. Configure GPIO registers.

To setup internal HSI clock, use Clock configuration registers in Reset and Clock Control block

   
