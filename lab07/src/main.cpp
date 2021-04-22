extern "C" {
#include "stm32f1xx_hal.h"
void SysTick_Handler();
}

#define READY_LED_PORT  GPIOA
#define READY_LED_PIN   GPIO_PIN_6
#define ACTION_LED_PORT GPIOA
#define ACTION_LED_PIN  GPIO_PIN_7
#define BUTTON_PORT     GPIOA
#define BUTTON_PIN      GPIO_PIN_8

void SystemClock_Config();
void GPIO_Init();
inline bool isButtonPressed();
inline void waitButtonPress();
inline void waitButtonRelease();
inline void setReady();
inline void clearReady();
inline void setAction();
inline void clearAction();


int main(void) {
    HAL_Init();
    SystemClock_Config();
    GPIO_Init();


    while (1) {
        setReady();
        waitButtonPress();
        clearReady();
        HAL_Delay(10);
        waitButtonRelease();
        HAL_Delay(250);
        setAction();
        HAL_Delay(250);
        clearAction();
    }
}

void GPIO_Init() {
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_ReadyInit;
    GPIO_ReadyInit.Pin = READY_LED_PIN;
    GPIO_ReadyInit.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_ReadyInit.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(READY_LED_PORT, &GPIO_ReadyInit);

    GPIO_InitTypeDef GPIO_ActionInit;
    GPIO_ActionInit.Pin = ACTION_LED_PIN;
    GPIO_ActionInit.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_ActionInit.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(ACTION_LED_PORT, &GPIO_ActionInit);

    GPIO_InitTypeDef GPIO_ButtonInit;
    GPIO_ButtonInit.Pin = BUTTON_PIN;
    GPIO_ButtonInit.Mode = GPIO_MODE_INPUT;
    GPIO_ButtonInit.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(BUTTON_PORT, &GPIO_ButtonInit);
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


bool isButtonPressed() {
    return !(BUTTON_PORT->IDR & BUTTON_PIN);
}

void waitButtonPress() {
    while (!isButtonPressed()) { }
}

void waitButtonRelease() {
    while (isButtonPressed()) { }
}

void setReady() {
    READY_LED_PORT->ODR |= READY_LED_PIN;
}

void clearReady() {
    READY_LED_PORT->ODR &= ~READY_LED_PIN;
}

void setAction() {
    ACTION_LED_PORT->ODR |= ACTION_LED_PIN;
}

void clearAction() {
    ACTION_LED_PORT->ODR &= ~ACTION_LED_PIN;
}
