#define PART_TM4C123GH6PM
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"


int main(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_5|GPIO_PIN_4);

	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5|GPIO_PIN_4, 0x10);
	//GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0xFF);
	while(1)
	{
		;
	}
	//return 0;
}
