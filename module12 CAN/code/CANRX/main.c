#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "driverlib/can.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "utils/uartstdio.h"
#include "driverlib/rom.h"
#include "driverlib/fpu.h"
#include "driverlib/pin_map.h"
#include "led.h"


// 变量定义部分
// 一个计数器，用于记录所接收的数据包数
volatile uint32_t g_ulMsgCount_RX = 0;
// 用于指示在传输过程中是否出现错误以及错误类型
volatile uint32_t g_bErrFlag = 0;
//用于指示是否接收到数据包
volatile uint32_t g_bRXFlag = 0;

// 函数定义部分
// UART0的配置及初始化部分。*
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

// 延时1s的函数
void SimpleDelay(void)
{
	//由于ROM_SysCtlDelay函数延时3个时钟周期，而ROM_SysCtlClockGet
	//函数返回系统时钟的频率，因此最终的结果就是延时1s
    ROM_SysCtlDelay(ROM_SysCtlClockGet()/3);
}

// CAN的中断函数
// 该函数寻找产生中断的原因，并且计算已发送/接收的数据包数目。
//由于要使用CAN0模块的中断函数，所以需要在startup_ccs.c文件的
//vector table（中断函数列表）中声明CANIntHandler。
void CANIntHandler(void)
{
	uint32_t ulStatus;

    // 通过CANIntStatus函数读取中断的状态
    ulStatus = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);

    // 如果是控制器状态中断，则说明出现了某种错误
    if(ulStatus == CAN_INT_INTID_STATUS)
    {
		//读取CAN模块所处的状态，并自动清除中断。
        ulStatus = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
       //将g_bErrFlag这个状态指示变量置1，以指示有错误发生
        g_bErrFlag = 1;
    }
    // 检查是否是由于message object 2引起的接收中断
    else if(ulStatus==2)
    {
        // 数据包接收已经完成，清除中断
        CANIntClear(CAN0_BASE, 2);
		// 接收完一个数据包，计数器g_ulMsgCount_RX增加
		g_ulMsgCount_RX++;
		// 设置Flag说明接收到的数据包正在等待处理
        g_bRXFlag = 1;
        // 接收已经完成，清除所有的错误信息.
		g_bErrFlag = 0;
    }
}

// main 函数
// 配置CAN模块，循环发送CAN格式数据包并通过LOOPBACK模式接收。
// 运行过程中的信息通过UART向计算机机传输。
int main(void)
{
	//使能FPU
	FPUEnable();
    FPULazyStackingEnable();

    // 定义CAN的接收对象
    tCANMsgObject srCANMessage;

    // 定义发送接收数据存储区
    unsigned int uIdx;//一个循环语句使用的变量
    uint8_t ucrMsgData[8];

    // 禁用中断。在进行中断配置时要保证中断没被使用。
    IntMasterDisable();

    // 设置系统时钟为40MHz
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // 由于CAN0使用PE4、PE5两个引脚，需要使能GPIOE对应的时钟
    //信号
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // 使能CAN0模块的时钟信号
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);

    //将PE4和PE5两个引脚的功能选择为执行CAN0模块的功能
    GPIOPinConfigure(GPIO_PE4_CAN0RX);
    GPIOPinConfigure(GPIO_PE5_CAN0TX);

    // 对PE4和PE5两个引脚做有关CAN功能的配置
    GPIOPinTypeCAN(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    // CAN控制器的初始化
    CANInit(CAN0_BASE);
    // 利用CANBitRateSet函数将CAN的传输速率设置为1MHz,且位速率最高为1Mbps（小于40m）。500m时为125Kbps，系统时钟为40MHz
    CANBitRateSet(CAN0_BASE, SysCtlClockGet(), 1000000);

    // 使能CAN0模块
    CANEnable(CAN0_BASE);

    // 使能CAN0模块的中断
    IntEnable(INT_CAN0);

    //设置可以引起CAN中断的中断源
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);

    // 初始化配置用于接收的报文对象
    // 同样也是用CANMessageSet函数可以将某个message object（报文对象）
    //设置为如下的配置
   // 可以接收ID为1的报文对象
    srCANMessage.ui32MsgID = 1;
    // 没有屏蔽
    srCANMessage.ui32MsgIDMask = 0;
    // 使能接收中断和ID过滤
    srCANMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE|MSG_OBJ_USE_ID_FILTER;
    // 允许最多8个字节的数据
    srCANMessage.ui32MsgLen = 8;

    //将message object 2设置为接收报文对象
    ROM_CANMessageSet(CAN0_BASE,2,&srCANMessage,MSG_OBJ_TYPE_RX);

    // 使能UART模块
    InitConsole();
    //初始化LED
    InitLED();
    //使能中断
    IntMasterEnable();

    // 开始进入发送数据包的循环， 每秒将发送一个数据包。
    while(1)
    {
        // 接收报文程序
        // 通过g_bRXFlag判断是否有已经接收到是数据包
        if(g_bRXFlag)
        {
        	//开始接收，置高PD6以点亮LED_RX表示正在接收数据
        	GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_6,0x40);
        	// 建立指向报文数据的缓存区
        	srCANMessage.pui8MsgData = ucrMsgData;

        	// 读取接收到的报文对象。
        	// 将message object中的信息读取到srCANMessage接收对象中
        	CANMessageGet(CAN0_BASE,2,&srCANMessage,0);

        	// 将g_bRXFlag置为0。
          //等到下个报文到来时，中断函数会再次将它置1的
        	g_bRXFlag=0;
        	// 如果出现数据丢失等错误，则输出提示信息
        	if(srCANMessage.ui32Flags & MSG_OBJ_DATA_LOST)
        	{
        		UARTprintf("CAN message loss detected\n");
        	}

        	// 通过UART输出接收到的报文信息
        	UARTprintf("Receive Msg ID=0x%08X len=%u data=0x",srCANMessage.ui32MsgID,srCANMessage.ui32MsgLen);

        	// 输出报文中的数据内容
        	for(uIdx=0;uIdx<srCANMessage.ui32MsgLen;uIdx++)
        	{
        		UARTprintf("%02X",ucrMsgData[uIdx]);
        	}

        	// 输出所有已收到的报文数目
        	UARTprintf(" total count Received = %u\n", g_ulMsgCount_RX);
        	//结束接收，置低PD6以熄灭LED_RX表示不在接收数据
        	GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_6,0x00);
        }
    }
    //return(0);
}

