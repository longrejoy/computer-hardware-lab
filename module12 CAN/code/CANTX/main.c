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
// 一个计数器，用于记录所发送的数据包数
volatile uint32_t g_ulMsgCount_TX = 0;
// 用于指示在传输过程中是否出现错误以及错误类型
volatile uint32_t g_bErrFlag = 0;


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
    // 检查是否是由message object 1引起的发送中断
    else if(ulStatus == 1)
    {
        // 数据包发送已经完成，清除中断
        CANIntClear(CAN0_BASE, 1);

		 //发送完一个数据包，计数器g_ulMsgCount_TX增加
        g_ulMsgCount_TX++;
        // 发送已经完成，清除所有的错误信息.
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

    // 定义CAN的发送对象
    tCANMsgObject sCANMessage;

    // 定义发送数据存储区
    uint8_t ucMsgData[4];

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

    //设置可以引起CAN中断的中断源
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);

    // 使能CAN0模块的中断
    IntEnable(INT_CAN0);



    //初始化配置message object，即待发送的报文对象
    //配置好报文对象后调用CANMessageSet函数进行设置，
    //这样报文就可以自动被发送出去

    // 待发送的数据为0
    *(uint32_t *)ucMsgData = 0;
    // CAN message ID:使用1作为报文的ID
    sCANMessage.ui32MsgID = 1;
    // 没有MASK屏蔽
    sCANMessage.ui32MsgIDMask = 0;
    //使能发送中断
    sCANMessage.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
    //发送数据包大小为4个字节
    sCANMessage.ui32MsgLen = sizeof(ucMsgData);
    // 指向发送数据的指针
    sCANMessage.pui8MsgData = ucMsgData;

    // 使能UART模块
    InitConsole();
    //初始化LED
    InitLED();
    //使能中断
    IntMasterEnable();

    // 开始进入发送数据包的循环， 每秒将发送一个数据包。
    while(1)
    {
        // 将待发送的数据包内容通过UART传输并显示出来
        UARTprintf("Sending msg: 0x%02X %02X %02X %02X",
             ucMsgData[0], ucMsgData[1], ucMsgData[2], ucMsgData[3]);

       // 将待发送的报文配置到message object 1中。
        CANMessageSet(CAN0_BASE, 1, &sCANMessage, MSG_OBJ_TYPE_TX);

        // 开始发送，PB3置高以点亮LED_TX表示正在发送
        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_3,0x08);
        // 等待1s
        SimpleDelay();
        // 通过g_bErrFlag变量查看是否出错
        if(g_bErrFlag)
        {
            UARTprintf(" error - cable connected?\n");
        }
        else
        {
            // 如果没有出错，则显示已经发送的报文数目
            UARTprintf(" total count Transmit = %u\n", g_ulMsgCount_TX);
        }
        // 每次发送完毕，都将报文中的数据内容+1
        (*(uint32_t *)ucMsgData)++;
        //发送结束，PB3置低以关闭LED_TX表示不在发送
        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_3,0x00);
    }
    //return(0);
}

