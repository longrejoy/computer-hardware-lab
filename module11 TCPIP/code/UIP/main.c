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

bool D0;	//ָʾD0	LED0	PB3��FLAG��0Ϊ�͵�ƽ����1Ϊ�ߵ�ƽ����
bool D1;	//ָʾD1	LED1	PD6��FLAG��0Ϊ�͵�ƽ����1Ϊ�ߵ�ƽ����

/*
 * �°汾Ϊ
 *
bool D0;	//ָʾD0	LED0	PB2��FLAG��0Ϊ�͵�ƽ����1Ϊ�ߵ�ƽ����
bool D1;	//ָʾD1	LED1	PE0��FLAG��0Ϊ�͵�ƽ����1Ϊ�ߵ�ƽ����

 */

// �������岿��
// UART0�����ü���ʼ�����֡�*
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

    // ����ϵͳʱ��Ϊ40MHz
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

 	InitConsole();
 	LED_Init();			     //LED�˿ڳ�ʼ��
 	Buttons_Init();			 //��ʼ������
	TIME_Init();

	UARTprintf("Tiva\n");
	UARTprintf("ENC28J60 TEST\n");
 	while(tapdev_init())	//��ʼ��ENC28J60����
	{								   
 		UARTprintf("ENC28J60 Init Error!\n");
 		SysCtlDelay(2666667); //��ʱ200ms

	};		
	uip_init();				//uIP��ʼ��	  
	UARTprintf("SWITCH1(LEFT ):TIVA works as Server and sends Msg\n");
	UARTprintf("SWITCH2(RIGHT):TIVA works as Client and sends Msg\n");
	UARTprintf("TIVA's IP:192.168.1.16\n");
	UARTprintf("MASK:255.255.255.0\n");
	UARTprintf("GATEWAY:192.168.1.1\n\n");
	
 	uip_ipaddr(ipaddr, 192,168,1,16);	//���ñ�������IP��ַ
	uip_sethostaddr(ipaddr);					    
	uip_ipaddr(ipaddr, 192,168,1,1); 	//��������IP��ַ(��ʵ������·������IP��ַ)
	uip_setdraddr(ipaddr);						 
	uip_ipaddr(ipaddr, 255,255,255,0);	//������������
	uip_setnetmask(ipaddr);

	uip_listen(HTONS(1200));			//����1200�˿�,����TCP Server
	uip_listen(HTONS(80));				//����80�˿�,����Web Server
  	tcp_client_reconnect();	   		    //�������ӵ�TCP Server��,����TCP Client
	while (1)
	{
		uip_polling();	//����uip�¼���������뵽�û������ѭ������  

		ui32Buttons = ButtonsState(0);

		if(tcp_server_tsta!=tcp_server_sta)//TCP Server״̬�ı�
		{															 
			if(tcp_server_sta&(1<<7))
				UARTprintf("TCP Server(TIVA) Connected\n\n");
			else
				UARTprintf("TCP Server(TIVA) Disconnected\n\n");
 			if(tcp_server_sta&(1<<6))	//�յ�������
			{
 				UARTprintf("TCP Server(TIVA) RX:%s\r\n",tcp_server_databuf);
				tcp_server_sta&=~(1<<6);		//��������Ѿ�������	
			}
			tcp_server_tsta=tcp_server_sta;
		}
		if((ui32Buttons & ALL_BUTTONS) == LEFT_BUTTON)//TCP Server ����������
		{
			if(tcp_server_sta&(1<<7))	//���ӻ�����
			{
				UARTprintf("TCP Server(TIVA) OK %d\r\n",tcnt);//��ʾ��ǰ��������
				tcp_server_sta|=1<<5;//�����������Ҫ����
				tcnt++;
			}
		}

		if(tcp_client_tsta!=tcp_client_sta)//TCP Client״̬�ı�
		{															 
			if(tcp_client_sta&(1<<7))
				UARTprintf("TCP Client(TIVA) Connected\n\n");
			else
				UARTprintf("TCP Client(TIVA) Disconnected\n\n");
 			if(tcp_client_sta&(1<<6))	//�յ�������
			{
 				UARTprintf("TCP Client(TIVA) RX:%s\r\n",tcp_client_databuf);
    			//printf("TCP Client(TIVA) RX:%s\r\n",tcp_client_databuf);//��ӡ����
				tcp_client_sta&=~(1<<6);		//��������Ѿ�������	
			}
			tcp_client_tsta=tcp_client_sta;
		}
		if ((ui32Buttons & ALL_BUTTONS) == RIGHT_BUTTON) //TCP Client ����������
		{
			if(tcp_client_sta&(1<<7))	//���ӻ�����
			{
				//sprintf((char*)tcp_client_databuf,"TCP Client(TIVA) OK %d\r\n",tcnt);
				UARTprintf("TCP Client(TIVA) OK %d\r\n",tcnt);//��ʾ��ǰ��������
				tcp_client_sta|=1<<5;//�����������Ҫ����
				tcnt++;
			}
		}

		SysCtlDelay(13333); //��ʱ1ms
	}  
} 
//uip�¼�������
//���뽫�ú��������û���ѭ��,ѭ������.
void uip_polling(void)
{
	u8 i;
	static struct timer periodic_timer, arp_timer;
	static u8 timer_ok=0;	 
	if(timer_ok==0)//����ʼ��һ��
	{
		timer_ok = 1;
		timer_set(&periodic_timer,CLOCK_SECOND/2);  //����1��0.5��Ķ�ʱ�� 
		timer_set(&arp_timer,CLOCK_SECOND*10);	   	//����1��10��Ķ�ʱ�� 
	}				 
	uip_len=tapdev_read();	//�������豸��ȡһ��IP��,�õ����ݳ���.uip_len��uip.c�ж���
	if(uip_len>0) 			//������
	{   
		//����IP���ݰ�(ֻ��У��ͨ����IP���Żᱻ����) 
		if(BUF->type == htons(UIP_ETHTYPE_IP))//�Ƿ���IP��? 
		{
			uip_arp_ipin();	//ȥ����̫��ͷ�ṹ������ARP��
			uip_input();   	//IP������
			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ��� uip_len > 0
			//��Ҫ���͵�������uip_buf, ������uip_len  (����2��ȫ�ֱ���)		    
			if(uip_len>0)//��Ҫ��Ӧ����
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}else if (BUF->type==htons(UIP_ETHTYPE_ARP))//����arp����,�Ƿ���ARP�����?
		{
			uip_arp_arpin();
 			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len(����2��ȫ�ֱ���)
 			if(uip_len>0)tapdev_send();//��Ҫ��������,��ͨ��tapdev_send����	 
		}
	}else if(timer_expired(&periodic_timer))	//0.5�붨ʱ����ʱ
	{
		timer_reset(&periodic_timer);		//��λ0.5�붨ʱ�� 
		//��������ÿ��TCP����, UIP_CONNSȱʡ��40��  
		for(i=0;i<UIP_CONNS;i++)
		{
			uip_periodic(i);	//����TCPͨ���¼�  
	 		//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len (����2��ȫ�ֱ���)
	 		if(uip_len>0)
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}
#if UIP_UDP	//UIP_UDP 
		//��������ÿ��UDP����, UIP_UDP_CONNSȱʡ��10��
		for(i=0;i<UIP_UDP_CONNS;i++)
		{
			uip_udp_periodic(i);	//����UDPͨ���¼�
	 		//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len (����2��ȫ�ֱ���)
			if(uip_len > 0)
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}
#endif 
		//ÿ��10�����1��ARP��ʱ������ ���ڶ���ARP����,ARP��10�����һ�Σ��ɵ���Ŀ�ᱻ����
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
