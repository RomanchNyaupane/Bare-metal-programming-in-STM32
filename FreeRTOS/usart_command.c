#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stm32l4xx_hal_usart.h"
#include "semphr.h"


UART_HandleTypeDef huart2;
SemaphoreHandle_t xMutex;
SemaphoreHandle_t xSemaphore;
QueueHandle_t xQueue;
TaskHandle_t xTask;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);


void LED_Init(void) {
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;     
    GPIOA->MODER &= ~(3U << (5 * 2));
    GPIOA->MODER |=  (1U << (5 * 2));
}

void led_task(void *pvParameter){
		uint32_t cmd;
	  uint8_t msg[] = "chintu chhatu";
			for (;;)
			{
					xTaskNotifyWait(0, 0xFFFFFFFF, &cmd, portMAX_DELAY);
					switch(cmd)
					{
							case 0x00: GPIOA -> ODR |= (1U << 5);  break;
							case 0x01: GPIOA -> ODR &= ~(1U << 5); break;
							case 0x02: GPIOA -> ODR ^= (1U << 5);  break;
							case 0x03: HAL_UART_Transmit(&huart2, msg, sizeof(msg), HAL_MAX_DELAY); break;
						//default : GPIOA -> ODR |= (1U << 5);
					}
			}
}
void uart_rx_task(void *pvParameter){
    uint8_t cmd;
    for (;;)
    {
        if (xQueueReceive(xQueue, &cmd, portMAX_DELAY) == pdTRUE)
        {
            switch(cmd)
            {
                case 0: xTaskNotify(xTask, 0x00, eSetValueWithOverwrite); break;
                case 1: xTaskNotify(xTask, 0x01, eSetValueWithOverwrite); break;
                case 2: xTaskNotify(xTask, 0x02, eSetValueWithOverwrite); break;
                case 3: xTaskNotify(xTask, 0x03, eSetValueWithOverwrite); break;
            }
        }
    }
}

uint8_t rx_byte;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart2)
{
    if (huart2 ->Instance == USART2)			
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE; //set initially
        xQueueSendFromISR(xQueue, &rx_byte, &xHigherPriorityTaskWoken); //changes xHigherPriorityTaskWoken to pdTRUE
				HAL_UART_Receive_IT(huart2, &rx_byte, 1);
        //portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //switches to higher priority task after completing isr
    }
}


int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    
    LED_Init();
    
    xQueue = xQueueCreate(1, sizeof(uint8_t));
    

    xTaskCreate(uart_rx_task, "UARTRX", 500, NULL, 1, NULL);
    xTaskCreate(led_task, "LED", 500, NULL, 1, &xTask);
    
    HAL_UART_Receive_IT(&huart2, &rx_byte, sizeof(uint8_t));
    
    vTaskStartScheduler();
    
    while(1);
}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }


}

static void MX_GPIO_Init(void)
{

  __HAL_RCC_GPIOA_CLK_ENABLE();

}


void Error_Handler(void)
{

  __disable_irq();
  while (1)
  {
  }
}
#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif