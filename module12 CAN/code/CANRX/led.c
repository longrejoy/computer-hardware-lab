#include "led.h"


void InitLED(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_3);
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE,GPIO_PIN_6);
}
