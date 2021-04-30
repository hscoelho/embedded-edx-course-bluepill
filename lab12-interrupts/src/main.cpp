extern "C" {
#include "stm32f1xx_hal.h"
void SysTick_Handler();
void EXTI2_IRQHandler();
void HAL_GPIO_EXTI_Callback(uint16_t);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void TIM2_IRQHandler();
}
/* Pins */
#define SWITCH_PORT GPIOA
#define SWITCH_PIN  GPIO_PIN_2
#define OUTPUT_PORT GPIOA
#define OUTPUT_PIN  GPIO_PIN_1
/* global variables */
TIM_HandleTypeDef htim2;
volatile bool enable_sound;
volatile bool output;

/* Functions */
void SystemClock_Config();
void gpioInit();
void timerInit();


int main(void) {
    HAL_Init();
    SystemClock_Config();
    timerInit();
    gpioInit();

    while (1) {
    }
}

void gpioInit() {
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_init;
  // PA0: interrupt switch on rising edge
  GPIO_init.Pin = SWITCH_PIN;
  GPIO_init.Mode = GPIO_MODE_IT_RISING;
  GPIO_init.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(SWITCH_PORT, &GPIO_init);
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  // PA1: 440hz wave output
  GPIO_init.Pin = OUTPUT_PIN;
  GPIO_init.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_init.Pull = GPIO_NOPULL;
  GPIO_init.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(OUTPUT_PORT, &GPIO_init);
}

void EXTI2_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(SWITCH_PIN);  // hal handler
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == SWITCH_PIN) {
        enable_sound = (enable_sound) ? false : true;
    }
}

void timerInit() {
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 1;  // 72Mhz / (1 + 1) = 36Mhz
    htim2.Init.Period = 40908;  // 36Mhz / (40908 + 1) = 880Hz
    /* htim2.Init.CounterMode = TIM_COUNTERMODE_UP; */
    /* htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; */
    /* htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; */
    HAL_TIM_Base_Init(&htim2);

    /* TIM_ClockConfigTypeDef sClockSourceConfig = {0}; */
    /* TIM_MasterConfigTypeDef sMasterConfig = {0}; */
    /* sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL; */
    /* HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) */
    __HAL_RCC_TIM2_CLK_ENABLE();

    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

    HAL_TIM_Base_Start_IT(&htim2);
}

void TIM2_IRQHandler() {
    HAL_TIM_IRQHandler(&htim2);  // pass control to HAL
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        if (enable_sound) {
            HAL_GPIO_TogglePin(OUTPUT_PORT, OUTPUT_PIN);
            output = (output) ? false : true;
        }
    }
}

void SystemClock_Config() {
    /* Configure the system clock to 72 MHz */

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

