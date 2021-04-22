#include "stm32f1xx_hal.h"
/* Inputs */
/* West, south and pedestrian sensors */
#define W_SENSOR_PIN  GPIO_PIN_8
#define W_SENSOR_PORT GPIOA

#define S_SENSOR_PIN  GPIO_PIN_9
#define S_SENSOR_PORT GPIOA

#define P_SENSOR_PIN  GPIO_PIN_10
#define P_SENSOR_PORT GPIOA

/* Outputs */

/* West lights */
#define LED_WRED_PIN     GPIO_PIN_7
#define LED_WRED_PORT    GPIOA
#define LED_WYELLOW_PIN  GPIO_PIN_6
#define LED_WYELLOW_PORT GPIOA
#define LED_WGREEN_PIN   GPIO_PIN_5
#define LED_WGREEN_PORT  GPIOA

/* South lights */
#define LED_SRED_PIN     GPIO_PIN_4
#define LED_SRED_PORT    GPIOA
#define LED_SYELLOW_PIN  GPIO_PIN_3
#define LED_SYELLOW_PORT GPIOA
#define LED_SGREEN_PIN   GPIO_PIN_2
#define LED_SGREEN_PORT  GPIOA

/* Pedestrian lights */
#define LED_PRED_PIN    GPIO_PIN_1
#define LED_PRED_PORT   GPIOA
#define LED_PGREEN_PIN  GPIO_PIN_0
#define LED_PGREEN_PORT GPIOA

