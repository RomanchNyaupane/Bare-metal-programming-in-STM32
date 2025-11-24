/*
Clocking wizard version 1.0
    -> Provides functions to get system clock and peripheral clock frequencies
    -> Supports MSI, HSI16, HSE and PLL as system clock sources
    -> For STM32L4 series microcontrollers
*/
#ifndef CLOCK_UTILS_H
#define CLOCK_UTILS_H

#include "stm32l4xx.h"

uint32_t GetSystemClockFreq(void);
uint32_t GetAHBCLKFreq(void);
uint32_t GetAPB1CLKFreq(void);
uint32_t GetAPB2CLKFreq(void);

#endif // CLOCK_UTILS_H