#include "stm32l4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

SemaphoreHandle_t ledMutex;
SemaphoreHandle_t buttonSemaphore;

void LED_Init(void) {
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    GPIOA->MODER &= ~(3U << (5 * 2));
    GPIOA->MODER |=  (1U << (5 * 2));
}

void Button_Init(void) {
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
    GPIOC->MODER &= ~(3U << (13 * 2)); // input
    GPIOC->PUPDR &= ~(3U << (13 * 2));
    GPIOC->PUPDR |=  (1U << (13 * 2)); // pull-up
}

uint8_t Button_Read(void) {
    return ((GPIOC->IDR & (1U << 13)) == 0U); // pressed = 1
}

void button_task(void *pvParameters) {
    uint8_t prev_state = 0;
    while (1) {
        uint8_t current = Button_Read();
        if (current && !prev_state) { // rising edge
            xSemaphoreGive(buttonSemaphore);
        }
        prev_state = current;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void led_fast(void *pvParameters) {
    while (1) {
        if (xSemaphoreTake(buttonSemaphore, portMAX_DELAY) == pdTRUE) {
            if (xSemaphoreTake(ledMutex, portMAX_DELAY) == pdTRUE) {
                for (int i = 0; i < 5; i++) {
                    GPIOA->ODR ^= (1U << 5);
                    vTaskDelay(pdMS_TO_TICKS(100));
                }
                xSemaphoreGive(ledMutex);
            }
            xSemaphoreGive(buttonSemaphore);
        }
    }
}

void led_slow(void *pvParameters) {
    while (1) {
        if (xSemaphoreTake(buttonSemaphore, portMAX_DELAY) == pdTRUE) {
            if (xSemaphoreTake(ledMutex, portMAX_DELAY) == pdTRUE) {
                for (int i = 0; i < 3; i++) {
                    GPIOA->ODR ^= (1U << 5);
                    vTaskDelay(pdMS_TO_TICKS(500));
                }
                xSemaphoreGive(ledMutex);
            }
        }
    }
}

int main(void) {
    LED_Init();
    Button_Init();

    ledMutex = xSemaphoreCreateMutex();
    buttonSemaphore = xSemaphoreCreateBinary();

    xTaskCreate(button_task, "Button", 128, NULL, 2, NULL);
    xTaskCreate(led_fast, "Fast LED", 128, NULL, 1, NULL);
    xTaskCreate(led_slow, "Slow LED", 128, NULL, 1, NULL);

    vTaskStartScheduler();
    while (1);
}
