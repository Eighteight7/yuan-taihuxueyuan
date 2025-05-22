#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f10x.h"

// --- DHT11 Pin Definition ---
#define DHT11_GPIO_PORT         GPIOG
#define DHT11_GPIO_PIN          GPIO_Pin_11
#define DHT11_GPIO_RCC          RCC_APB2Periph_GPIOG

// --- Return Status ---
typedef enum {
    DHT11_OK = 0,
    DHT11_ERR_CHECKSUM = 1,
    DHT11_ERR_TIMEOUT = 2,
    DHT11_ERR_NO_RESPONSE = 3
} DHT11_StatusTypeDef;


// --- Function Prototypes ---
void DHT11_Init(void);
DHT11_StatusTypeDef DHT11_Read_Data(uint8_t *temperature, uint8_t *humidity);

#endif /* __DHT11_H */