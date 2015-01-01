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


// �������岿��
// һ�������������ڼ�¼�����յ����ݰ���
volatile uint32_t g_ulMsgCount_RX = 0;
// ����ָʾ�ڴ���������Ƿ���ִ����Լ���������
volatile uint32_t g_bErrFlag = 0;
//����ָʾ�Ƿ���յ����ݰ�
volatile uint32_t g_bRXFlag = 0;

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

// ��ʱ1s�ĺ���
void SimpleDelay(void)
{
	//����ROM_SysCtlDelay������ʱ3��ʱ�����ڣ���ROM_SysCtlClockGet
	//��������ϵͳʱ�ӵ�Ƶ�ʣ�������յĽ��������ʱ1s
    ROM_SysCtlDelay(ROM_SysCtlClockGet()/3);
}

// CAN���жϺ���
// �ú���Ѱ�Ҳ����жϵ�ԭ�򣬲��Ҽ����ѷ���/���յ����ݰ���Ŀ��
//����Ҫʹ��CAN0ģ����жϺ�����������Ҫ��startup_ccs.c�ļ���
//vector table���жϺ����б�������CANIntHandler��
void CANIntHandler(void)
{
	uint32_t ulStatus;

    // ͨ��CANIntStatus������ȡ�жϵ�״̬
    ulStatus = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);

    // ����ǿ�����״̬�жϣ���˵��������ĳ�ִ���
    if(ulStatus == CAN_INT_INTID_STATUS)
    {
		//��ȡCANģ��������״̬�����Զ�����жϡ�
        ulStatus = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
       //��g_bErrFlag���״ָ̬ʾ������1����ָʾ�д�����
        g_bErrFlag = 1;
    }
    // ����Ƿ�������message object 2����Ľ����ж�
    else if(ulStatus==2)
    {
        // ���ݰ������Ѿ���ɣ�����ж�
        CANIntClear(CAN0_BASE, 2);
		// ������һ�����ݰ���������g_ulMsgCount_RX����
		g_ulMsgCount_RX++;
		// ����Flag˵�����յ������ݰ����ڵȴ�����
        g_bRXFlag = 1;
        // �����Ѿ���ɣ�������еĴ�����Ϣ.
		g_bErrFlag = 0;
    }
}

// main ����
// ����CANģ�飬ѭ������CAN��ʽ���ݰ���ͨ��LOOPBACKģʽ���ա�
// ���й����е���Ϣͨ��UART�����������䡣
int main(void)
{
	//ʹ��FPU
	FPUEnable();
    FPULazyStackingEnable();

    // ����CAN�Ľ��ն���
    tCANMsgObject srCANMessage;

    // ���巢�ͽ������ݴ洢��
    unsigned int uIdx;//һ��ѭ�����ʹ�õı���
    uint8_t ucrMsgData[8];

    // �����жϡ��ڽ����ж�����ʱҪ��֤�ж�û��ʹ�á�
    IntMasterDisable();

    // ����ϵͳʱ��Ϊ40MHz
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // ����CAN0ʹ��PE4��PE5�������ţ���Ҫʹ��GPIOE��Ӧ��ʱ��
    //�ź�
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // ʹ��CAN0ģ���ʱ���ź�
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);

    //��PE4��PE5�������ŵĹ���ѡ��Ϊִ��CAN0ģ��Ĺ���
    GPIOPinConfigure(GPIO_PE4_CAN0RX);
    GPIOPinConfigure(GPIO_PE5_CAN0TX);

    // ��PE4��PE5�����������й�CAN���ܵ�����
    GPIOPinTypeCAN(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    // CAN�������ĳ�ʼ��
    CANInit(CAN0_BASE);
    // ����CANBitRateSet������CAN�Ĵ�����������Ϊ1MHz,��λ�������Ϊ1Mbps��С��40m����500mʱΪ125Kbps��ϵͳʱ��Ϊ40MHz
    CANBitRateSet(CAN0_BASE, SysCtlClockGet(), 1000000);

    // ʹ��CAN0ģ��
    CANEnable(CAN0_BASE);

    // ʹ��CAN0ģ����ж�
    IntEnable(INT_CAN0);

    //���ÿ�������CAN�жϵ��ж�Դ
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);

    // ��ʼ���������ڽ��յı��Ķ���
    // ͬ��Ҳ����CANMessageSet�������Խ�ĳ��message object�����Ķ���
    //����Ϊ���µ�����
   // ���Խ���IDΪ1�ı��Ķ���
    srCANMessage.ui32MsgID = 1;
    // û������
    srCANMessage.ui32MsgIDMask = 0;
    // ʹ�ܽ����жϺ�ID����
    srCANMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE|MSG_OBJ_USE_ID_FILTER;
    // �������8���ֽڵ�����
    srCANMessage.ui32MsgLen = 8;

    //��message object 2����Ϊ���ձ��Ķ���
    ROM_CANMessageSet(CAN0_BASE,2,&srCANMessage,MSG_OBJ_TYPE_RX);

    // ʹ��UARTģ��
    InitConsole();
    //��ʼ��LED
    InitLED();
    //ʹ���ж�
    IntMasterEnable();

    // ��ʼ���뷢�����ݰ���ѭ���� ÿ�뽫����һ�����ݰ���
    while(1)
    {
        // ���ձ��ĳ���
        // ͨ��g_bRXFlag�ж��Ƿ����Ѿ����յ������ݰ�
        if(g_bRXFlag)
        {
        	//��ʼ���գ��ø�PD6�Ե���LED_RX��ʾ���ڽ�������
        	GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_6,0x40);
        	// ����ָ�������ݵĻ�����
        	srCANMessage.pui8MsgData = ucrMsgData;

        	// ��ȡ���յ��ı��Ķ���
        	// ��message object�е���Ϣ��ȡ��srCANMessage���ն�����
        	CANMessageGet(CAN0_BASE,2,&srCANMessage,0);

        	// ��g_bRXFlag��Ϊ0��
          //�ȵ��¸����ĵ���ʱ���жϺ������ٴν�����1��
        	g_bRXFlag=0;
        	// ����������ݶ�ʧ�ȴ����������ʾ��Ϣ
        	if(srCANMessage.ui32Flags & MSG_OBJ_DATA_LOST)
        	{
        		UARTprintf("CAN message loss detected\n");
        	}

        	// ͨ��UART������յ��ı�����Ϣ
        	UARTprintf("Receive Msg ID=0x%08X len=%u data=0x",srCANMessage.ui32MsgID,srCANMessage.ui32MsgLen);

        	// ��������е���������
        	for(uIdx=0;uIdx<srCANMessage.ui32MsgLen;uIdx++)
        	{
        		UARTprintf("%02X",ucrMsgData[uIdx]);
        	}

        	// ����������յ��ı�����Ŀ
        	UARTprintf(" total count Received = %u\n", g_ulMsgCount_RX);
        	//�������գ��õ�PD6��Ϩ��LED_RX��ʾ���ڽ�������
        	GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_6,0x00);
        }
    }
    //return(0);
}

