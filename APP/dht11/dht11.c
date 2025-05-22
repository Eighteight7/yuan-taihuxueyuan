#include "dht11.h"
#include "mq_common.h" // Needs delay_us

// --- Private Function Prototypes ---
static void DHT11_SetPin_Output(void);
static void DHT11_SetPin_Input(void);
static void DHT11_WritePin(uint8_t state); // 0=Low, 1=High
static uint8_t DHT11_ReadPin(void);
static uint8_t DHT11_Read_Byte(void);
static DHT11_StatusTypeDef DHT11_Check_Response(void);

/**
 * @brief Initializes the GPIO pin for DHT11.
 */
void DHT11_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable clock for GPIOG
    RCC_APB2PeriphClockCmd(DHT11_GPIO_RCC, ENABLE);

    // Configure PG11 initially as Output Push-Pull, High state
    GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // Push-Pull Output
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);

    // Set initial state high (idle)
    DHT11_WritePin(1);
}

/**
 * @brief Set PG11 pin mode to Output Push-Pull.
 */
static void DHT11_SetPin_Output(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);
}

/**
 * @brief Set PG11 pin mode to Input Floating.
 */
static void DHT11_SetPin_Input(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // Or Input Pull-up
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);
}

/**
 * @brief Write state to PG11 pin.
 * @param state: 0 for Low, 1 for High.
 */
static void DHT11_WritePin(uint8_t state) {
    if (state) {
        GPIO_SetBits(DHT11_GPIO_PORT, DHT11_GPIO_PIN);
    } else {
        GPIO_ResetBits(DHT11_GPIO_PORT, DHT11_GPIO_PIN);
    }
}

/**
 * @brief Read state from PG11 pin.
 * @retval 0 if Low, 1 if High.
 */
static uint8_t DHT11_ReadPin(void) {
    return GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN);
}

/**
 * @brief Sends the start signal to DHT11.
 */
static void DHT11_Start_Signal(void) {
    DHT11_SetPin_Output();    // Set pin as output
    DHT11_WritePin(0);        // Pull low
    delay_ms(18);             // Keep low for at least 18ms
    DHT11_WritePin(1);        // Pull high
    delay_us(30);             // Keep high for 20-40us
    DHT11_SetPin_Input();     // Set pin as input to read response
}

/**
 * @brief Checks for DHT11 response signal.
 * @retval DHT11_OK if response OK, DHT11_ERR_NO_RESPONSE otherwise.
 */
static DHT11_StatusTypeDef DHT11_Check_Response(void) {
    uint8_t retry = 0;
    // Wait for DHT11 to pull low (max ~80us)
    while (DHT11_ReadPin() && retry < 100) {
        retry++;
        delay_us(1); // Small delay
    }
    if (retry >= 100) return DHT11_ERR_NO_RESPONSE;

    retry = 0;
    // Wait for DHT11 to pull high (max ~80us)
    while (!DHT11_ReadPin() && retry < 100) {
        retry++;
        delay_us(1);
    }
    if (retry >= 100) return DHT11_ERR_NO_RESPONSE;

     retry = 0;
    // Wait for DHT11 to pull low again (start of data transmission)
    while (DHT11_ReadPin() && retry < 100) {
        retry++;
        delay_us(1);
    }
     if (retry >= 100) return DHT11_ERR_NO_RESPONSE;


    return DHT11_OK;
}

/**
 * @brief Reads one byte (8 bits) from DHT11.
 * @retval The byte read.
 */
static uint8_t DHT11_Read_Byte(void) {
    uint8_t i, byte = 0;
    uint8_t retry = 0;

    for (i = 0; i < 8; i++) {
        retry = 0;
        // Wait for pin to go high (start of bit)
        while (!DHT11_ReadPin() && retry < 100) {
             retry++;
             delay_us(1);
        }
         if (retry >= 100) return 0; // Timeout error reading byte

        retry = 0;
        // Wait for pin to go low (end of bit) - length determines 0 or 1
         while (DHT11_ReadPin() && retry < 100) {
             retry++;
             delay_us(1);
        }
         if (retry >= 100) return 0; // Timeout error reading byte


        // If pin was high for > 40us (approx), it's a '1'
        if (retry > 40) { // This threshold might need tuning based on delay_us accuracy
             byte |= (uint8_t)(0x01 << (7 - i));
        }

    }
    return byte;
}

/**
 * @brief Reads Temperature and Humidity from DHT11 sensor.
 * @param temperature: Pointer to store temperature value (integer part).
 * @param humidity: Pointer to store humidity value (integer part).
 * @retval DHT11_StatusTypeDef status code.
 */
DHT11_StatusTypeDef DHT11_Read_Data(uint8_t *temperature, uint8_t *humidity) {
    uint8_t data[5]; // 0: 湿度整数部分, 1: 湿度小数部分, 2: 温度整数部分, 3: 温度小数部分, 4: 校验和
    uint8_t i;
    uint8_t checksum_calc;
    DHT11_StatusTypeDef status;

    DHT11_Start_Signal();
    status = DHT11_Check_Response();

    if (status != DHT11_OK) {
        return status; // 传感器无响应
    }

    // 读取 5 个字节的数据
    for (i = 0; i < 5; i++) {
        data[i] = DHT11_Read_Byte();
        // 简单的检查 Read_Byte 是否超时 (在某些情况下意外返回 0)
        // 一个更好的 Read_Byte 函数会返回状态码或使用中断。
    }

    // 校验和验证
    checksum_calc = data[0] + data[1] + data[2] + data[3];
    if (checksum_calc != data[4]) {
        return DHT11_ERR_CHECKSUM; // 校验和错误
    }

    // 数据正常，赋值
    *humidity = data[0];    // 湿度的整数部分
    *temperature = data[2]; // 温度的整数部分
    // DHT11 的小数部分通常为 0

    return DHT11_OK;
}