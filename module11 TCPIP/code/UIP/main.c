#include "led.h"
#include "enc28j60.h"
#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "timer.h"				   
#include "math.h" 	
#include "string.h"
#include "utils/uartstdio.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "inc/hw_ints.h"
#include "buttons.h"

void uip_polling(void);	 												
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

bool D0;	//指示D0	LED0	PB3的FLAG，0为低电平，灭；1为高电平，亮
bool D1;	//指示D1	LED1	PD6的FLAG，0为低电平，灭；1为高电平，亮

/*
 * 新版本为
 *
bool D0;	//指示D0	LED0	PB2的FLAG，0为低电平，灭；1为高电平，亮
bool D1;	//指示D1	LED1	PE0的FLAG，0为低电平，灭；1为高电平，亮

 */

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

void TIME_Init(void)
{
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    ROM_IntMasterEnable();
    ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, ROM_SysCtlClockGet()/100);
    ROM_IntEnable(INT_TIMER0A);
    ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    ROM_TimerEnable(TIMER0_BASE, TIMER_A);
}

 int main(void)
 {	 
	u8 tcnt=0;
	u8 tcp_server_tsta=0XFF;
	u8 tcp_client_tsta=0XFF;
 	uip_ipaddr_t ipaddr;
 	u32 ui32Buttons=0xff;

    // 设置系统时钟为40MHz
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

 	InitConsole();
 	LED_Init();			     //LED端口初始化
 	Buttons_Init();			 //初始化按键
	TIME_Init();

	UARTprintf("Tiva\n");
	UARTprintf("ENC28J60 TEST\n");
 	while(tapdev_init())	//初始化ENC28J60错误
	{								   
 		UARTprintf("ENC28J60 Init Error!\n");
 		SysCtlDelay(2666667); //延时200ms

	};		
	uip_init();				//uIP初始化	  
	UARTprintf("SWITCH1(LEFT ):TIVA works as Server and sends Msg\n");
	UARTprintf("SWITCH2(RIGHT):TIVA works as Client and sends Msg\n");
	UARTprintf("TIVA's IP:192.168.1.16\n");
	UARTprintf("MASK:255.255.255.0\n");
	UARTprintf("GATEWAY:192.168.1.1\n\n");
	
 	uip_ipaddr(ipaddr, 192,168,1,16);	//设置本地设置IP地址
	uip_sethostaddr(ipaddr);					    
	uip_ipaddr(ipaddr, 192,168,1,1); 	//设置网关IP地址(其实就是你路由器的IP地址)
	uip_setdraddr(ipaddr);						 
	uip_ipaddr(ipaddr, 255,255,255,0);	//设置网络掩码
	uip_setnetmask(ipaddr);

	uip_listen(HTONS(1200));			//监听1200端口,用于TCP Server
	uip_listen(HTONS(80));				//监听80端口,用于Web Server
  	tcp_client_reconnect();	   		    //尝试连接到TCP Server端,用于TCP Client
	while (1)
	{
		uip_polling();	//处理uip事件，必须插入到用户程序的循环体中  

		ui32Buttons = ButtonsState(0);

		if(tcp_server_tsta!=tcp_server_sta)//TCP Server状态改变
		{															 
			if(tcp_server_sta&(1<<7))
				UARTprintf("TCP Server(TIVA) Connected\n\n");
			else
				UARTprintf("TCP Server(TIVA) Disconnected\n\n");
 			if(tcp_server_sta&(1<<6))	//收到新数据
			{
 				UARTprintf("TCP Server(TIVA) RX:%s\r\n",tcp_server_databuf);
				tcp_server_sta&=~(1<<6);		//标记数据已经被处理	
			}
			tcp_server_tsta=tcp_server_sta;
		}
		if((ui32Buttons & ALL_BUTTONS) == LEFT_BUTTON)//TCP Server 请求发送数据
		{
			if(tcp_server_sta&(1<<7))	//连接还存在
			{
				UARTprintf("TCP Server(TIVA) OK %d\r\n",tcnt);//显示当前发送数据
				tcp_server_sta|=1<<5;//标记有数据需要发送
				tcnt++;
			}
		}

		if(tcp_client_tsta!=tcp_client_sta)//TCP Client状态改变
		{															 
			if(tcp_client_sta&(1<<7))
				UARTprintf("TCP Client(TIVA) Connected\n\n");
			else
				UARTprintf("TCP Client(TIVA) Disconnected\n\n");
 			if(tcp_client_sta&(1<<6))	//收到新数据
			{
 				UARTprintf("TCP Client(TIVA) RX:%s\r\n",tcp_client_databuf);
    			//printf("TCP Client(TIVA) RX:%s\r\n",tcp_client_databuf);//打印数据
				tcp_client_sta&=~(1<<6);		//标记数据已经被处理	
			}
			tcp_client_tsta=tcp_client_sta;
		}
		if ((ui32Buttons & ALL_BUTTONS) == RIGHT_BUTTON) //TCP Client 请求发送数据
		{
			if(tcp_client_sta&(1<<7))	//连接还存在
			{
				//sprintf((char*)tcp_client_databuf,"TCP Client(TIVA) OK %d\r\n",tcnt);
				UARTprintf("TCP Client(TIVA) OK %d\r\n",tcnt);//显示当前发送数据
				tcp_client_sta|=1<<5;//标记有数据需要发送
				tcnt++;
			}
		}

		SysCtlDelay(13333); //延时1ms
	}  
} 
//uip事件处理函数
//必须将该函数插入用户主循环,循环调用.
void uip_polling(void)
{
	u8 i;
	static struct timer periodic_timer, arp_timer;
	static u8 timer_ok=0;	 
	if(timer_ok==0)//仅初始化一次
	{
		timer_ok = 1;
		timer_set(&periodic_timer,CLOCK_SECOND/2);  //创建1个0.5秒的定时器 
		timer_set(&arp_timer,CLOCK_SECOND*10);	   	//创建1个10秒的定时器 
	}				 
	uip_len=tapdev_read();	//从网络设备读取一个IP包,得到数据长度.uip_len在uip.c中定义
	if(uip_len>0) 			//有数据
	{   
		//处理IP数据包(只有校验通过的IP包才会被接收) 
		if(BUF->type == htons(UIP_ETHTYPE_IP))//是否是IP包? 
		{
			uip_arp_ipin();	//去除以太网头结构，更新ARP表
			uip_input();   	//IP包处理
			//当上面的函数执行后，如果需要发送数据，则全局变量 uip_len > 0
			//需要发送的数据在uip_buf, 长度是uip_len  (这是2个全局变量)		    
			if(uip_len>0)//需要回应数据
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			}
		}else if (BUF->type==htons(UIP_ETHTYPE_ARP))//处理arp报文,是否是ARP请求包?
		{
			uip_arp_arpin();
 			//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len(这是2个全局变量)
 			if(uip_len>0)tapdev_send();//需要发送数据,则通过tapdev_send发送	 
		}
	}else if(timer_expired(&periodic_timer))	//0.5秒定时器超时
	{
		timer_reset(&periodic_timer);		//复位0.5秒定时器 
		//轮流处理每个TCP连接, UIP_CONNS缺省是40个  
		for(i=0;i<UIP_CONNS;i++)
		{
			uip_periodic(i);	//处理TCP通信事件  
	 		//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len (这是2个全局变量)
	 		if(uip_len>0)
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			}
		}
#if UIP_UDP	//UIP_UDP 
		//轮流处理每个UDP连接, UIP_UDP_CONNS缺省是10个
		for(i=0;i<UIP_UDP_CONNS;i++)
		{
			uip_udp_periodic(i);	//处理UDP通信事件
	 		//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len (这是2个全局变量)
			if(uip_len > 0)
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			}
		}
#endif 
		//每隔10秒调用1次ARP定时器函数 用于定期ARP处理,ARP表10秒更新一次，旧的条目会被抛弃
		if(timer_expired(&arp_timer))
		{
			timer_reset(&arp_timer);
			uip_arp_timer();
		}
	}
}

uint32_t uip_timer=0;
void Timer0IntHandler(void)
{
    //
    // Clear the timer interrupt.
    //
    ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    uip_timer++;
}
