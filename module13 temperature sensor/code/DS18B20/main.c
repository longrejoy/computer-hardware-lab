#include <stdint.h>
#include <stdbool.h>
#include "DS18B20.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"

// UART0的配置及初始化部分。
// UART0模块用于通过计算机虚拟串口显示过程信息，主要包括InitConsole
//函数和一些UARTprintf语句。
void InitConsole(void)
{
    // 由于UART0使用PA0,PA1两个引脚，因此需要使能GPIOA模块
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	// 将PA0和PA1两个引脚的功能选择为执行UART0模块的功能
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    // 对PA0和PA1两个引脚配置为UART功能
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // UART的标准初始化
    UARTStdioConfig(0,115200,40000000);
}

int main(void)
{
	uint32_t temperature;
	uint8_t temp=0;
	// 设置系统时钟为40MHz
	SysCtlClockSet(SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_SYSDIV_5);
	InitConsole();
	DS18B20_Init();
	UARTprintf("Find DS18B20\n");

	while (1)
	{
		if (temp%10==0)//每300ms读取一次
		{
			temperature=DS18B20_Get_Temp();
			temperature=temperature/10.0;
			UARTprintf("current temperature:%d\n",temperature);
		}
		SysCtlDelay(400000);//延时30ms
		temp++;
		if (temp==20)
		{
			temp=0;
		}

	}
}
