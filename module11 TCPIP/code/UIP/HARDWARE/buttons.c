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


//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//LEFT_BUTTON，SWITCH1按下
//RIGHT_BUTTON,SWITCH2按下

//注意此函数有响应优先级,SWITCH1>SWITCH2
uint32_t ButtonsState(uint8_t mode)
{
	static uint8_t key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按
	if(key_up&&(SWITCH1==0||SWITCH2==0))
	{
		SysCtlDelay(133333);//延时10ms去抖动
		key_up=0;
		if(SWITCH1==0)return LEFT_BUTTON;
		else if(SWITCH2==0)return RIGHT_BUTTON;
	}else if(SWITCH1==0x10&&SWITCH2==0x01)key_up=1;
 	return 0;// 无按键按下
}
