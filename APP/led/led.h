#ifndef _led_H
#define _led_H

#include "system.h" // Assuming system.h includes necessary STM32 headers and bit-band definitions

/*-- LED1 Definitions (GPIOB, Pin 5) --*/
#define LED1_PORT           GPIOB
#define LED1_PIN            GPIO_Pin_5
#define LED1_PORT_RCC       RCC_APB2Periph_GPIOB

/*-- LED2 Definitions (GPIOE, Pin 5) --*/
#define LED2_PORT           GPIOE
#define LED2_PIN            GPIO_Pin_5
#define LED2_PORT_RCC       RCC_APB2Periph_GPIOE

/*-- LED3 Definitions (GPIOC, Pin 1) --*/
#define LED3_PORT           GPIOC               // New LED3 Port
#define LED3_PIN            GPIO_Pin_1          // New LED3 Pin
#define LED3_PORT_RCC       RCC_APB2Periph_GPIOC  // New LED3 Port Clock

/*-- LED4 Definitions (GPIOC, Pin 3) --*/
#define LED4_PORT           GPIOC               // New LED4 Port
#define LED4_PIN            GPIO_Pin_3          // New LED4 Pin
#define LED4_PORT_RCC       RCC_APB2Periph_GPIOC  // New LED4 Port Clock (Same as LED3)


/*-- LED Control Macros (using bit-band aliases - requires system.h support) --*/
// Existing Macros
#define LED1 PBout(5)       // Control LED1 on PB5
#define LED2 PEout(5)       // Control LED2 on PE5

// New Macros
#define LED3 PCout(1)       // Control LED3 on PC1
#define LED4 PCout(3)       // Control LED4 on PC3
// Note: Ensure PCout(x) macro is defined correctly in your system.h or equivalent


// --- Function Prototypes ---
void LED_Init(void);


#endif // _led_H