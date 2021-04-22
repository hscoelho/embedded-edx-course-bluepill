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
#include "pins_def.h"

/* FSM */
struct State {
    uint8_t output[3];  // 0: west, 1:south, 2:pedestrian
    uint32_t delay;
    uint8_t input[8];
};
#define RED         0x04
#define YELLOW      0x02
#define GREEN       0x01
#define WALK        0x01
#define NOWALK      0x02
#define OFF         0x00
#define GO_DELAY    2000
#define WAIT_DELAY  1000
#define WALK_DELAY  2000
#define FLASH_DELAY 500

const struct State fsm[9] = {
    {{RED,    GREEN,  NOWALK}, GO_DELAY,    {0, 1, 1, 1, 0, 1, 1, 1}},
    {{RED,    YELLOW, NOWALK}, WAIT_DELAY,  {4, 4, 2, 2, 4, 4, 2, 2}},
    {{GREEN,  RED,    NOWALK}, GO_DELAY,    {2, 3, 2, 3, 3, 3, 3, 3}},
    {{YELLOW, RED,    NOWALK}, WAIT_DELAY,  {4, 4, 0, 4, 0, 4, 0, 4}},
    {{RED,    RED,    WALK},   WALK_DELAY,  {4, 4, 5, 5, 5, 5, 5, 5}},
    {{RED,    RED,    NOWALK}, FLASH_DELAY, {6, 6, 6, 6, 6, 6, 6, 6}},
    {{RED,    RED,    OFF},    FLASH_DELAY, {7, 7, 7, 7, 7, 7, 7, 7}},
    {{RED,    RED,    NOWALK}, FLASH_DELAY, {8, 8, 8, 8, 8, 8, 8, 8}},
    {{RED,    RED,    OFF},    FLASH_DELAY, {0, 0, 2, 2, 0, 0, 0, 0}}
};

/* Functions */
void SystemClock_Config();
void GPIO_Init();
void setOutput(const uint8_t *);
uint8_t getSensor();


int main(void) {
    HAL_Init();
    SystemClock_Config();
    GPIO_Init();

    uint8_t cState = 0;

    while (1) {
        setOutput(fsm[cState].output);
        HAL_Delay(fsm[cState].delay);
        cState = fsm[cState].input[getSensor()];
    }
}

void setOutput(const uint8_t *output) {
    /* NOT PORTABLE */
    GPIOA->ODR = (output[0] << 5)|
                 (output[1] << 2)|
                 (output[2] << 0);
    /* west pa5-7, south pa2-4, pedestrian pa0-1 */
}

uint8_t getSensor() {
    bool s_sensor = (S_SENSOR_PORT->IDR & S_SENSOR_PIN) ? 1 : 0;
    bool w_sensor = (W_SENSOR_PORT->IDR & W_SENSOR_PIN) ? 1 : 0;
    bool p_sensor = (P_SENSOR_PORT->IDR & P_SENSOR_PIN) ? 1 : 0;

    return (s_sensor << 2)|
           (w_sensor << 1)|
           (p_sensor << 0);
}

void GPIO_Init() {
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Outputs */
    GPIO_InitTypeDef GPIO_led_init;
    GPIO_led_init.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_led_init.Speed = GPIO_SPEED_LOW;

    /* West */
    GPIO_led_init.Pin = LED_WRED_PIN;
    HAL_GPIO_Init(LED_WRED_PORT, &GPIO_led_init);
    GPIO_led_init.Pin = LED_WYELLOW_PIN;
    HAL_GPIO_Init(LED_WYELLOW_PORT, &GPIO_led_init);
    GPIO_led_init.Pin = LED_WGREEN_PIN;
    HAL_GPIO_Init(LED_WGREEN_PORT, &GPIO_led_init);
    /* Could use:
    * Pin = LED_WRED_PIN | LED_WYELLOW_PIN | LED_WGREEN_PIN ...
    * However, it's less portable (the pins could be in different ports)
    */


    /* South */
    GPIO_led_init.Pin = LED_SRED_PIN;
    HAL_GPIO_Init(LED_SRED_PORT, &GPIO_led_init);
    GPIO_led_init.Pin = LED_SYELLOW_PIN;
    HAL_GPIO_Init(LED_SYELLOW_PORT, &GPIO_led_init);
    GPIO_led_init.Pin = LED_SGREEN_PIN;
    HAL_GPIO_Init(LED_SGREEN_PORT, &GPIO_led_init);

    /* Pedestrian */
    GPIO_led_init.Pin = LED_PRED_PIN;
    HAL_GPIO_Init(LED_PRED_PORT, &GPIO_led_init);
    GPIO_led_init.Pin = LED_PGREEN_PIN;
    HAL_GPIO_Init(LED_PGREEN_PORT, &GPIO_led_init);

    /* Inputs */
    GPIO_InitTypeDef GPIO_sensor_init;
    GPIO_sensor_init.Mode = GPIO_MODE_INPUT;
    GPIO_sensor_init.Pull = GPIO_PULLDOWN;

    /* West */
    GPIO_sensor_init.Pin = W_SENSOR_PIN;
    HAL_GPIO_Init(W_SENSOR_PORT, &GPIO_sensor_init);

    /* South */
    GPIO_sensor_init.Pin = S_SENSOR_PIN;
    HAL_GPIO_Init(S_SENSOR_PORT, &GPIO_sensor_init);

    /* Pedestrian */
    GPIO_sensor_init.Pin = P_SENSOR_PIN;
    HAL_GPIO_Init(P_SENSOR_PORT, &GPIO_sensor_init);
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
