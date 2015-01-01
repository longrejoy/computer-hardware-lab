#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "utils/uartstdio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/uart.h"

// UART1�����ü���ʼ�����֡�
// UART1ģ��������TC35Iģ�鷢��ATָ���Ҫ����InitConsole
//������һЩUARTprintf��䡣

void InitConsole(void)
{
    // ����UART1ʹ��PC4,PC5�������ţ������Ҫʹ��GPIOCģ��
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

	// ��PC4��PC5�������ŵĹ���ѡ��Ϊִ��UART1ģ��Ĺ���
    GPIOPinConfigure(GPIO_PC4_U1RX);
    GPIOPinConfigure(GPIO_PC5_U1TX);

    // ��PC4��PC5������������ΪUART����
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    // UART�ı�׼��ʼ��
    UARTStdioConfig(1,115200,40000000);
}

int main(void)
{
	//����ʱ����ƵΪ40MHz
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	//��ʼ��UART1
	InitConsole();
	//��PE2(IGBT)���ź�������TC35Iģ��
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE,GPIO_PIN_2);
	GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_2,0xFF);
	SysCtlDelay(SysCtlClockGet()/30);//��ʱ100ms
	GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_2,0x00);
	SysCtlDelay(1600000);//��ʱ120ms

	//��ʱ6s�Եȴ�SIM����¼
	SysCtlDelay(SysCtlClockGet());
	SysCtlDelay(SysCtlClockGet());
	SysCtlDelay(SysCtlClockGet());
	SysCtlDelay(SysCtlClockGet());
	SysCtlDelay(SysCtlClockGet());
	SysCtlDelay(SysCtlClockGet());

	//�������ӳɹ�
	UARTprintf("AT\r");
	SysCtlDelay(SysCtlClockGet());
	//����ΪTextģʽ
	UARTprintf("AT+CMGF=1\r");
	SysCtlDelay(SysCtlClockGet());
	//���ö���Ϣ���ĺ��룬��������һ��
	UARTprintf("AT+CSCA=\"+8613800451500\"\r");
	SysCtlDelay(SysCtlClockGet());
	//����Textģʽ���������һ�����������ݱ�������
	//0��ʾĬ���ַ���(GSM)��4��ʾ8λ����
	UARTprintf("AT+CSMP=17,167,0,0\r");
	SysCtlDelay(SysCtlClockGet());
	//����ΪGSM�ַ���
	UARTprintf("AT+CSCS=GSM\r");
	SysCtlDelay(SysCtlClockGet());
	//����Է��绰����
	UARTprintf("AT+CMGS=180xxxxxxxx\r");
	SysCtlDelay(SysCtlClockGet());
	//д�뷢�����ݣ���ΪӢ�ķ��ź�����
	UARTprintf("12345\r");
	//����(���ͷ�����ASCIIֵ��26����16���Ʒ���ʱ��1A)
	MAP_UARTCharPut(UART1_BASE,26);
	while(1){;}
	//return 0;
}
