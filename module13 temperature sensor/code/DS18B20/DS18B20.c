#include <stdbool.h>
#include <stdint.h>
#include "DS18B20.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"

//��λDS18B20
void DS18B20_Reset(void)
{
	Output_DQ;
	Reset_DQ;
	SysCtlDelay(10000);	//��ʱ750us
	Set_DQ;
	SysCtlDelay(200);	//��ʱ15us
}

//�ȴ�DS18B20�Ļ�Ӧ
//����1��δ��⵽DS18B20
//����0������
uint8_t DS18B20_Check(void)
{
	uint8_t retry=0;
	uint32_t temp=0;
	Input_DQ;
	temp=Status_DQ;
	while (temp && retry<200)
	{
		temp=Status_DQ;
		retry++;
		SysCtlDelay(40/3);//��ʱ1us
	}
	if (retry>=200)
		return 1;
	else
		retry=0;
	temp=Status_DQ;
	while (!temp && retry<240)
	{
		temp=Status_DQ;
		retry++;
		SysCtlDelay(40/3);//��ʱ1us
	}
	if (retry>=240)
		return 1;
	return 0;
}

//��DS18B20��ȡһ��λ
//����ֵ��0/1
uint8_t DS18B20_Read_Bit(void)
{
	uint8_t data=0;
	uint32_t temp=0;
	Output_DQ;
	Reset_DQ;
	SysCtlDelay(80/3);//��ʱ2us
	Set_DQ;
	Input_DQ;
	SysCtlDelay(160);//��ʱ12us
	temp=Status_DQ;
	if (temp) data=1;
	else data=0;
	SysCtlDelay(2000/3);//��ʱ50us
	return data;
}

//��DS18B20��ȡһ���ֽ�
//����ֵ������������
uint8_t DS18B20_Read_Byte(void)
{
	uint8_t i,j,data=0;
	for (i=1;i<=8;i++)
	{
		j=DS18B20_Read_Bit();
		data=(j<<7) | (data>>1);
	}
	return data;
}

//дһ���ֽڵ�DS18B20
//data��Ҫд����ֽ�
void DS18B20_Write_Byte(uint8_t data)
{
	uint8_t j,temp=0;
	Output_DQ;
	for (j=1;j<=8;j++)
	{
		temp=data&0x01;
		data=data>>1;
		if (temp)
		{
			Reset_DQ;
			SysCtlDelay(80/3);//��ʱ2us
			Set_DQ;
			SysCtlDelay(800);//��ʱ60us
		}
		else
		{
			Reset_DQ;
			SysCtlDelay(800);//��ʱ60us
			Set_DQ;
			SysCtlDelay(80/3);//��ʱ2us
		}
	}
}

//��ʼ�¶�ת��
void DS18B20_Start_Convert(void)
{
	DS18B20_Reset();
	DS18B20_Check();
	DS18B20_Write_Byte(0xcc);
	DS18B20_Write_Byte(0x44);
}

//��ʼ��DS18B20��IO��DQͬʱ���DS18B20�Ĵ���
//DQΪPA6
//����1��δ��⵽DS18B20
//����0������
uint8_t DS18B20_Init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	Output_DQ;
	//GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE,GPIO_PIN_6);
	Set_DQ;
	//GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_6,0x40);
	DS18B20_Reset();
	return DS18B20_Check();

}

//��DS18B20�õ��¶�ֵ
//���ȣ�0.1��
//����ֵ��ʵ���¶�ֵ*10��-550-1250��
uint32_t DS18B20_Get_Temp(void)
{
	uint8_t temp,TL,TH;
	uint32_t t;
	DS18B20_Start_Convert();
	DS18B20_Reset();
	DS18B20_Check();
	DS18B20_Write_Byte(0xcc);
	DS18B20_Write_Byte(0xbe);
	TL=DS18B20_Read_Byte();
	TH=DS18B20_Read_Byte();
	if (TH>7)
	{
		TL=TL-1;
		TH=~TH;
		TL=~TL;
		temp=0;
	}
	else temp=1;
	t=TH;
	t<<=8;
	t+=TL;
	t=(float)t*0.625;
	if (temp) return t;
	else return -t;

}
