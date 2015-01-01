#include <stdint.h>
#include <stdbool.h>
#include "DS18B20.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"

// UART0�����ü���ʼ�����֡�
// UART0ģ������ͨ����������⴮����ʾ������Ϣ����Ҫ����InitConsole
//������һЩUARTprintf��䡣
void InitConsole(void)
{
    // ����UART0ʹ��PA0,PA1�������ţ������Ҫʹ��GPIOAģ��
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	// ��PA0��PA1�������ŵĹ���ѡ��Ϊִ��UART0ģ��Ĺ���
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    // ��PA0��PA1������������ΪUART����
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // UART�ı�׼��ʼ��
    UARTStdioConfig(0,115200,40000000);
}

int main(void)
{
	uint32_t temperature;
	uint8_t temp=0;
	// ����ϵͳʱ��Ϊ40MHz
	SysCtlClockSet(SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_SYSDIV_5);
	InitConsole();
	DS18B20_Init();
	UARTprintf("Find DS18B20\n");

	while (1)
	{
		if (temp%10==0)//ÿ300ms��ȡһ��
		{
			temperature=DS18B20_Get_Temp();
			temperature=temperature/10.0;
			UARTprintf("current temperature:%d\n",temperature);
		}
		SysCtlDelay(400000);//��ʱ30ms
		temp++;
		if (temp==20)
		{
			temp=0;
		}

	}
}
