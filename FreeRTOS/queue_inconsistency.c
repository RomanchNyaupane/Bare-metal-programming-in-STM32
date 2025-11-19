// this piece of code demonstrates how queue can fail to synchronize shared resource (LED)
// task1: turn on led for a brief amount of time and then turn off
// task2: turn off led for a brief amount of time and then turn on
// task3: use button to generate a flag to turn on led

// error demonstrated: task1 and task2 and task3 gets scheduled. if led1 was supposed to toggle for fixed amount of time in a critical condition, it would
//                     not matter if flag used to turn on(by button) was captured safely in queue. queue does its job of keeping flag stable but it
//										 does not guarantee stability of task to complete.

#include "stm32l4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void LED_Init(void) {
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;     
    GPIOA->MODER &= ~(3U << (5 * 2));
    GPIOA->MODER |=  (1U << (5 * 2));
}

void Button_Init(void) {
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;     
    GPIOC->MODER &= ~(3U << (13 * 2));
    GPIOC->PUPDR &= ~(3U << (13 * 2));    
    GPIOC->PUPDR |=  (1U << (13 * 2));      
}

uint8_t Button_Read(void) {
    return ((GPIOC->IDR & (1U << 13)) == 0U);   
}

QueueHandle_t buttonQueue;

void read_button(void *pvParameters) {
    (void) pvParameters;

    while (1) {
        if (Button_Read()) {
            uint8_t msg = 1;
            xQueueSend(buttonQueue, &msg, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(50));  // debounce and yield
    }
}

void toggle_leds_slow(void *pvParameters) {
    (void) pvParameters;
    uint8_t msg;

    for (;;) {
        if (xQueueReceive(buttonQueue, &msg, portMAX_DELAY) == pdPASS) {
            for (volatile uint32_t i = 0; i < 800; i++){
								vTaskDelay(300);
								GPIOA->ODR ^= (1U << 5);
						};
        }
				        vTaskDelay(pdMS_TO_TICKS(100));

    }
		
}


void toggle_leds_fast(void *pvParameters) {
    (void) pvParameters;
    uint8_t msg;

    for (;;) {
        if (xQueueReceive(buttonQueue, &msg, portMAX_DELAY) == pdPASS) {
            for (volatile uint32_t i = 0; i < 200; i++){
							vTaskDelay(25);
							GPIOA->ODR ^= (1U << 5);
				};
        }
				        vTaskDelay(pdMS_TO_TICKS(100));

    }
}

int main(void) {
    LED_Init();
    Button_Init();

    buttonQueue = xQueueCreate(10, sizeof(uint8_t));

    xTaskCreate(read_button, "Button Task", 128, NULL, 2, NULL);
    xTaskCreate(toggle_leds_slow, "LED Slow", 128, NULL, 1, NULL);
    xTaskCreate(toggle_leds_fast, "LED Fast", 128, NULL, 1, NULL);

    vTaskStartScheduler();

    while (1);
}
