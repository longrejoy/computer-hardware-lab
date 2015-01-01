#define PART_TM4C123GH6PM
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

/*
 * ������Ĳ��������һ����һ�������ǣ��벽������5.625��/64
*/
int main(void)
{
	uint8_t cc_code[8]={0x90,0x10,0x30,0x20,0x60,0x40,0xC0,0x80};	//˳ʱ��clockwise����
	uint8_t ccw_code[8]={0x80,0xC0,0x40,0x60,0x20,0x30,0x10,0x90};	//��ʱ��counter-clockwise����
	uint8_t i=0;
	uint16_t j=0;
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

	while (1)
	{
		//˳ʱ��תһȦ
		for (j=0;j<512;j++)
			for (i=0;i<8;i++)
			{
				GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, cc_code[i]);
				SysCtlDelay(26667);//��ʱ����ת�٣��˴���ʱ2ms��ת��Ϊ7.324r/min,��8.192s/r
			}
		//��ʱ��תһȦ
		for (j=0;j<512;j++)
			for (i=0;i<8;i++)
			{
				GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, ccw_code[i]);
				SysCtlDelay(26667);//��ʱ����ת�٣��˴���ʱ2ms��ת��Ϊ7.324r/min,��8.192s/r
			}
	}
	//return 0;
}
