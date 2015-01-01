#include "buttons.h"

void Buttons_Init(void)
{

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    //
    // Unlock PF0 so we can change it to a GPIO input
    // Once we have enabled (unlocked) the commit register then re-lock it
    // to prevent further changes.  PF0 is muxed with NMI thus a special case.
    //
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;


    GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_0 , GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0 , GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4 , GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4 , GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}


//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//LEFT_BUTTON��SWITCH1����
//RIGHT_BUTTON,SWITCH2����

//ע��˺�������Ӧ���ȼ�,SWITCH1>SWITCH2
uint32_t ButtonsState(uint8_t mode)
{
	static uint8_t key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������
	if(key_up&&(SWITCH1==0||SWITCH2==0))
	{
		SysCtlDelay(133333);//��ʱ10msȥ����
		key_up=0;
		if(SWITCH1==0)return LEFT_BUTTON;
		else if(SWITCH2==0)return RIGHT_BUTTON;
	}else if(SWITCH1==0x10&&SWITCH2==0x01)key_up=1;
 	return 0;// �ް�������
}
