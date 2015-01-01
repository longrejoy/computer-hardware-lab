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
// һ�������������ڼ�¼�����͵����ݰ���
volatile uint32_t g_ulMsgCount_TX = 0;
// ����ָʾ�ڴ���������Ƿ���ִ����Լ���������
volatile uint32_t g_bErrFlag = 0;


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
    // ����Ƿ�����message object 1����ķ����ж�
    else if(ulStatus == 1)
    {
        // ���ݰ������Ѿ���ɣ�����ж�
        CANIntClear(CAN0_BASE, 1);

		 //������һ�����ݰ���������g_ulMsgCount_TX����
        g_ulMsgCount_TX++;
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

    // ����CAN�ķ��Ͷ���
    tCANMsgObject sCANMessage;

    // ���巢�����ݴ洢��
    uint8_t ucMsgData[4];

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

    //���ÿ�������CAN�жϵ��ж�Դ
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);

    // ʹ��CAN0ģ����ж�
    IntEnable(INT_CAN0);



    //��ʼ������message object���������͵ı��Ķ���
    //���úñ��Ķ�������CANMessageSet�����������ã�
    //�������ľͿ����Զ������ͳ�ȥ

    // �����͵�����Ϊ0
    *(uint32_t *)ucMsgData = 0;
    // CAN message ID:ʹ��1��Ϊ���ĵ�ID
    sCANMessage.ui32MsgID = 1;
    // û��MASK����
    sCANMessage.ui32MsgIDMask = 0;
    //ʹ�ܷ����ж�
    sCANMessage.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
    //�������ݰ���СΪ4���ֽ�
    sCANMessage.ui32MsgLen = sizeof(ucMsgData);
    // ָ�������ݵ�ָ��
    sCANMessage.pui8MsgData = ucMsgData;

    // ʹ��UARTģ��
    InitConsole();
    //��ʼ��LED
    InitLED();
    //ʹ���ж�
    IntMasterEnable();

    // ��ʼ���뷢�����ݰ���ѭ���� ÿ�뽫����һ�����ݰ���
    while(1)
    {
        // �������͵����ݰ�����ͨ��UART���䲢��ʾ����
        UARTprintf("Sending msg: 0x%02X %02X %02X %02X",
             ucMsgData[0], ucMsgData[1], ucMsgData[2], ucMsgData[3]);

       // �������͵ı������õ�message object 1�С�
        CANMessageSet(CAN0_BASE, 1, &sCANMessage, MSG_OBJ_TYPE_TX);

        // ��ʼ���ͣ�PB3�ø��Ե���LED_TX��ʾ���ڷ���
        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_3,0x08);
        // �ȴ�1s
        SimpleDelay();
        // ͨ��g_bErrFlag�����鿴�Ƿ����
        if(g_bErrFlag)
        {
            UARTprintf(" error - cable connected?\n");
        }
        else
        {
            // ���û�г�������ʾ�Ѿ����͵ı�����Ŀ
            UARTprintf(" total count Transmit = %u\n", g_ulMsgCount_TX);
        }
        // ÿ�η�����ϣ����������е���������+1
        (*(uint32_t *)ucMsgData)++;
        //���ͽ�����PB3�õ��Թر�LED_TX��ʾ���ڷ���
        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_3,0x00);
    }
    //return(0);
}

