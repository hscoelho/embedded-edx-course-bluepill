extern "C" {
#include "stm32f1xx_hal.h"
void SysTick_Handler();
}

#define ALARM_PORT  GPIOA
#define ALARM_PIN   GPIO_PIN_7
#define TOGGLE_PORT GPIOA
#define TOGGLE_PIN GPIO_PIN_8
#define SENSOR1_PORT GPIOA
#define SENSOR1_PIN GPIO_PIN_9
#define SENSOR2_PORT GPIOA
#define SENSOR2_PIN GPIO_PIN_10

void SystemClock_Config();
void GPIO_Init();

int main(void) {
    HAL_Init();
    SystemClock_Config();
    GPIO_Init();


    while (1) {
        if ((TOGGLE_PORT->IDR & TOGGLE_PIN) &&
            (!(SENSOR1_PORT->IDR & SENSOR1_PIN) ||
             !(SENSOR2_PORT->IDR & SENSOR2_PIN))) {
            ALARM_PORT->ODR ^= ALARM_PIN;
            HAL_Delay(100);
        } else {
            ALARM_PORT->ODR &= ~ALARM_PIN;
        }
    }
}

void GPIO_Init() {
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_AlarmInit;
    GPIO_AlarmInit.Pin = ALARM_PIN;
    GPIO_AlarmInit.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_AlarmInit.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(ALARM_PORT, &GPIO_AlarmInit);

    GPIO_InitTypeDef GPIO_ButtonInit;
    GPIO_ButtonInit.Pin = TOGGLE_PIN;
    GPIO_ButtonInit.Mode = GPIO_MODE_INPUT;
    GPIO_ButtonInit.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(TOGGLE_PORT, &GPIO_ButtonInit);

    GPIO_ButtonInit.Pin = SENSOR1_PIN;
    HAL_GPIO_Init(SENSOR1_PORT, &GPIO_ButtonInit);

    GPIO_ButtonInit.Pin = SENSOR2_PIN;
    HAL_GPIO_Init(SENSOR2_PORT, &GPIO_ButtonInit);

}

void SystemClock_Config() {
    /* Configure the system clock to 64 MHz */

    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        while (1) { }  // ERROR
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        while (1) { }  // ERROR
    }
}

void SysTick_Handler() {
    HAL_IncTick();
}

