/* LAB 09 
 * Requirements:
 * LED flash at 10Hz(^= every 0.05s)
 * Flashes if SW1 or SW2 are pressed
 * Record led,sw1 and sw2 data every time the input or output changes
 */

extern "C" {
#include "stm32f1xx_hal.h"
void SysTick_Handler();
}

#define LED_PIN  GPIO_PIN_7
#define LED_PORT GPIOA
#define SW1_PIN  GPIO_PIN_9
#define SW1_PORT GPIOA
#define SW2_PIN  GPIO_PIN_10
#define SW2_PORT GPIOA

void SystemClock_Config();
void GPIO_Init();

/* decltype(GPIOA->IDR) Data[50]; */
uint16_t Data[50];

int main(void) {
    HAL_Init();
    SystemClock_Config();
    GPIO_Init();

    unsigned int i = 0;
    LED_PORT->ODR &= ~LED_PIN;
    while (1) {
        if ((SW1_PORT->IDR & SW1_PIN) ||
            (SW2_PORT->IDR & SW2_PIN)) {
            LED_PORT->ODR ^= LED_PIN;
            if (i < 50) {
                Data[i] = (LED_PORT->IDR & LED_PIN) |
                          (SW1_PORT->IDR & SW1_PIN) |
                          (SW2_PORT->IDR & SW2_PIN);
                i++;
            }

        } else {
            if (LED_PORT->IDR & LED_PIN) {
                LED_PORT->ODR &= ~LED_PIN;
                if (i < 50) {
                    Data[i] = (LED_PORT->IDR & LED_PIN) |
                              (SW1_PORT->IDR & SW1_PIN) |
                              (SW2_PORT->IDR & SW2_PIN);
                    i++;
                }
            }
        }
        HAL_Delay(100);
    }
}

void GPIO_Init() {
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_LedInit;
    GPIO_LedInit.Pin = LED_PIN;
    GPIO_LedInit.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_LedInit.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(LED_PORT, &GPIO_LedInit);

    GPIO_InitTypeDef GPIO_ButtonInit;
    GPIO_ButtonInit.Pin = SW1_PIN;
    GPIO_ButtonInit.Mode = GPIO_MODE_INPUT;
    GPIO_ButtonInit.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(SW1_PORT, &GPIO_ButtonInit);

    GPIO_ButtonInit.Pin = SW2_PIN;
    HAL_GPIO_Init(SW2_PORT, &GPIO_ButtonInit);
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
