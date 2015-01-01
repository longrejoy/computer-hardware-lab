#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"

#define NUM_BUTTONS             2
#define LEFT_BUTTON             GPIO_PIN_4
#define RIGHT_BUTTON            GPIO_PIN_0

#define ALL_BUTTONS             (LEFT_BUTTON | RIGHT_BUTTON)

#define SWITCH1		GPIOPinRead(GPIO_PORTF_BASE , GPIO_PIN_4)
#define SWITCH2		GPIOPinRead(GPIO_PORTF_BASE , GPIO_PIN_0)


//
// Functions exported from buttons.c
//
//*****************************************************************************
void Buttons_Init(void);
uint32_t ButtonsState(uint8_t);



#endif // __BUTTONS_H__
