//D2	LED1	PD6
//D3	LED3	PB3
/*
 * �°汾led�ܽŶ���Ϊ
 * LED0 PB2
 * LED1 PE0
 */
#ifndef __LED_H
#define __LED_H

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"

void LED_Init(void);//��ʼ��



#define	D0ON	GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_3,0x08)//D0	LED0	PB3	��1��
#define	D1ON	GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_6,0x40)//D1	LED1	PD6	��1��
#define	D0OFF	GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_3,0x00)//D0	LED0	PB3	��0��
#define	D1OFF	GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_6,0x00)//D1	LED1	PD6	��0��

/*
 * �°汾led�ܽŶ���Ϊ
#define	D0ON	GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2,0x04)//D0	LED0	PB2	��1��
#define	D1ON	GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0,0x01)//D1	LED1	PE0	��1��
#define	D0OFF	GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2,0x00)//D0	LED0	PB2	��0��
#define	D1OFF	GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0,0x00)//D1	LED1	PE0	��0��
*/

#endif
