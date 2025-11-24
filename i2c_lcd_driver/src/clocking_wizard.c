/*
Clocking wizard version 1.0
    -> Provides functions to get system clock and peripheral clock frequencies
    -> Supports MSI, HSI16, HSE and PLL as system clock sources
    -> For STM32L4 series microcontrollers
*/

#include "stm32l4xx.h"
//function to get the system clock frequency in Hz
uint32_t GetSystemClockFreq(void)
{
    uint32_t sysclk = 0;
    uint32_t msirange = 0;
    uint32_t pllvco = 0;
    uint32_t pllsource = 0;
    uint32_t pllm = 0;
    uint32_t plln = 0;
    uint32_t pllr = 0;
    
    //read the clock source from RCC_CFGR register (bits 3:2 - SWS)
    uint32_t clk_source = (RCC->CFGR >> 2) & 0x3;
    switch(clk_source)
    {
        case 0x00:  //MSI used as system clock
            //get MSI range from RCC_CR register (bits 7:4 - MSIRANGE)
            msirange = (RCC->CR >> 4) & 0xF;
            //MSI frequency table
            const uint32_t MSIRangeTable[12] = {
                100000,    // Range 0: 100 kHz
                200000,    // Range 1: 200 kHz
                400000,    // Range 2: 400 kHz
                800000,    // Range 3: 800 kHz
                1000000,   // Range 4: 1 MHz
                2000000,   // Range 5: 2 MHz
                4000000,   // Range 6: 4 MHz (default after reset)
                8000000,   // Range 7: 8 MHz
                16000000,  // Range 8: 16 MHz
                24000000,  // Range 9: 24 MHz
                32000000,  // Range 10: 32 MHz
                48000000   // Range 11: 48 MHz
            };
            
            sysclk = MSIRangeTable[msirange];
            break;
            
        case 0x01:  //HSI16 used as system clock
            sysclk = 16000000;  //16 MHz
            break;
            
        case 0x02:  //HSE used as system clock
            sysclk = 8000000;   //8 MHz (external crystal on Nucleo board)
            break;
            
        case 0x03:  //PLL used as system clock
            //get PLL source from RCC_PLLCFGR register (bits 1:0 - PLLSRC)
            pllsource = RCC->PLLCFGR & 0x3;
            
            //get PLLM divider (bits 6:4) - values 0-7 map to divisors 1-8
            pllm = ((RCC->PLLCFGR >> 4) & 0x7) + 1;
            
            //get PLLN multiplier (bits 14:8) - values 8-86
            plln = (RCC->PLLCFGR >> 8) & 0x7F;
            
            //get PLLR divider (bits 26:25) - values 0,1,2,3 map to divisors 2,4,6,8
            pllr = ((RCC->PLLCFGR >> 25) & 0x3);
            uint32_t pllr_div = (pllr + 1) * 2;
            
            //determine PLL input clock
            uint32_t pll_input = 0;
            switch(pllsource)
            {
                case 0x00:  //no clock selected
                    pll_input = 0;
                    break;
                case 0x01:  //MSI
                    msirange = (RCC->CR >> 4) & 0xF;
                    const uint32_t MSITable[12] = {
                        100000, 200000, 400000, 800000, 1000000, 2000000,
                        4000000, 8000000, 16000000, 24000000, 32000000, 48000000
                    };
                    pll_input = MSITable[msirange];
                    break;
                case 0x02:  //HSI16
                    pll_input = 16000000;
                    break;
                case 0x03:  //HSE
                    pll_input = 8000000;
                    break;
            }
            
            //PLL formula: VCO = (Input / PLLM) * PLLN
            //SYSCLK = VCO / PLLR
            pllvco = (pll_input / pllm) * plln;
            sysclk = pllvco / pllr_div;
            break;
    }
    
    return sysclk;
}

//function to get AHB1 clock (HCLK) frequency
uint32_t GetAHBCLKFreq(void)
{
    uint32_t sysclk = GetSystemClockFreq();
    //get AHB prescaler from RCC_CFGR register (bits 7:4 - HPRE)
    uint32_t ahb_prescaler = (RCC->CFGR >> 4) & 0xF;
    //AHB prescaler table
    const uint32_t AHBPrescTable[16] = {
        1, 1, 1, 1, 1, 1, 1, 1,  // 0-7: no division
        2, 4, 8, 16, 64, 128, 256, 512  // 8-15: divisions
    };
    
    return sysclk / AHBPrescTable[ahb_prescaler];
}

//function to get APB1 clock (PCLK1) frequency
uint32_t GetAPB1CLKFreq(void)
{
    uint32_t hclk = GetAHBCLKFreq();
    //gset APB1 prescaler from RCC_CFGR register (bits 10:8 - PPRE1)
    uint32_t apb1_prescaler = (RCC->CFGR >> 8) & 0x7;
    // APB prescaler table
    const uint32_t APBPrescTable[8] = {
        1, 1, 1, 1, 2, 4, 8, 16  // 0-3: no division, 4-7: divisions
    };
    return hclk / APBPrescTable[apb1_prescaler];
}

//function to get APB2 clock (PCLK2) frequency
uint32_t GetAPB2CLKFreq(void)
{
    uint32_t hclk = GetAHBCLKFreq();
    //get APB2 prescaler from RCC_CFGR register (bits 13:11 - PPRE2)
    uint32_t apb2_prescaler = (RCC->CFGR >> 11) & 0x7;
    //APB prescaler table
    const uint32_t APBPrescTable[8] = {
        1, 1, 1, 1, 2, 4, 8, 16
    };
    return hclk / APBPrescTable[apb2_prescaler];
}