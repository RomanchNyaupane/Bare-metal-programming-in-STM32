#include "stm32l4xx.h"                  // Device header
#include "Board_LED.h"

#define LED_NUM                      (1)

const unsigned long led_mask[] = {1ul << 5};

/**
  \fn          int32_t LED_Initialize (void)
  \brief       Initialize LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Initialize (void) {

  RCC->AHB2ENR |=  (1ul << 0);                  /* Enable GPIOA clock         */

  /* Configure LED (PA.5) pins as push-pull outputs, No pull-up, pull-down */
  GPIOA->MODER   &= ~((3ul << 2*5));
  GPIOA->MODER   |=  ((1ul << 2*5));
  GPIOA->OTYPER  &= ~((1ul <<   5));
  GPIOA->OSPEEDR &= ~((3ul << 2*5));
  GPIOA->OSPEEDR |=  ((1ul << 2*5));
  GPIOA->PUPDR   &= ~((3ul << 2*5));
  return (0);
}


/**
  \fn          int32_t LED_Uninitialize (void)
  \brief       De-initialize I/O interface for LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Uninitialize (void) {

  return (0);
}


/**
  \fn          int32_t LED_On (uint32_t num)
  \brief       Turn on requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_On (uint32_t num) {
  int32_t retCode = 0;
  if (num < LED_NUM) {
    GPIOA->BSRR = (led_mask[num]);
  } else {
    retCode = -1;
  }
  return retCode;
}


/**
  \fn          int32_t LED_Off (uint32_t num)
  \brief       Turn off requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Off (uint32_t num) {
  int32_t retCode = 0;
  if (num < LED_NUM) {
    GPIOA->BSRR = (led_mask[num] << 16);
  } else {
    retCode = -1;
  }
  return retCode;
}


/**
  \fn          int32_t LED_SetOut (uint32_t val)
  \brief       Write value to LEDs
  \param[in]   val  value to be displayed on LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_SetOut (uint32_t val) {
  int i;

  for (i = 0; i < LED_NUM; i++) {
    if (val & (1<<i)) {
      LED_On (i);
    } else {
      LED_Off(i);
    }
  }
  return (0);
}


/**
  \fn          uint32_t LED_GetCount (void)
  \brief       Get number of LEDs
  \return      Number of available LEDs
*/
uint32_t LED_GetCount (void) {
  return LED_NUM;
}
