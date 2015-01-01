#include "LED.h"

void LED_Init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_3);
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE,GPIO_PIN_6);
}
/*
 * �°汾Ϊ
 *
 *
void LED_Init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_2);
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE,GPIO_PIN_0);
}
 */
