#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "utils/uartstdio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/uart.h"

// UART1的配置及初始化部分。
// UART1模块用于向TC35I模块发送AT指令，主要包括InitConsole
//函数和一些UARTprintf语句。

void InitConsole(void)
{
    // 由于UART1使用PC4,PC5两个引脚，因此需要使能GPIOC模块
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

	// 将PC4和PC5两个引脚的功能选择为执行UART1模块的功能
    GPIOPinConfigure(GPIO_PC4_U1RX);
    GPIOPinConfigure(GPIO_PC5_U1TX);

    // 对PC4和PC5两个引脚配置为UART功能
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    // UART的标准初始化
    UARTStdioConfig(1,115200,40000000);
}

int main(void)
{
	//设置时钟主频为40MHz
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	//初始化UART1
	InitConsole();
	//给PE2(IGBT)发信号以启动TC35I模块
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE,GPIO_PIN_2);
	GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_2,0xFF);
	SysCtlDelay(SysCtlClockGet()/30);//延时100ms
	GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_2,0x00);
	SysCtlDelay(1600000);//延时120ms

	//延时6s以等待SIM卡登录
	SysCtlDelay(SysCtlClockGet());
	SysCtlDelay(SysCtlClockGet());
	SysCtlDelay(SysCtlClockGet());
	SysCtlDelay(SysCtlClockGet());
	SysCtlDelay(SysCtlClockGet());
	SysCtlDelay(SysCtlClockGet());

	//测试连接成功
	UARTprintf("AT\r");
	SysCtlDelay(SysCtlClockGet());
	//设置为Text模式
	UARTprintf("AT+CMGF=1\r");
	SysCtlDelay(SysCtlClockGet());
	//设置短消息中心号码，各地区不一样
	UARTprintf("AT+CSCA=\"+8613800451500\"\r");
	SysCtlDelay(SysCtlClockGet());
	//设置Text模式参数，最后一个参数是数据编码类型
	//0表示默认字符集(GSM)，4表示8位数据
	UARTprintf("AT+CSMP=17,167,0,0\r");
	SysCtlDelay(SysCtlClockGet());
	//设置为GSM字符集
	UARTprintf("AT+CSCS=GSM\r");
	SysCtlDelay(SysCtlClockGet());
	//输入对方电话号码
	UARTprintf("AT+CMGS=180xxxxxxxx\r");
	SysCtlDelay(SysCtlClockGet());
	//写入发送内容，可为英文符号和数字
	UARTprintf("12345\r");
	//发送(发送符，其ASCII值是26，用16进制发送时是1A)
	MAP_UARTCharPut(UART1_BASE,26);
	while(1){;}
	//return 0;
}
