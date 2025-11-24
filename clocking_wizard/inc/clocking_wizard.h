#ifndef CLOCK_UTILS_H
#define CLOCK_UTILS_H

#include "stm32l4xx.h"

uint32_t GetSystemClockFreq(void);
uint32_t GetAHBCLKFreq(void);
uint32_t GetAPB1CLKFreq(void);
uint32_t GetAPB2CLKFreq(void);

#endif // CLOCK_UTILS_H