#include <stdbool.h>
#include <stdint.h>
#include "DS18B20.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"

//复位DS18B20
void DS18B20_Reset(void)
{
	Output_DQ;
	Reset_DQ;
	SysCtlDelay(10000);	//延时750us
	Set_DQ;
	SysCtlDelay(200);	//延时15us
}

//等待DS18B20的回应
//返回1：未检测到DS18B20
//返回0：存在
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
		SysCtlDelay(40/3);//延时1us
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
		SysCtlDelay(40/3);//延时1us
	}
	if (retry>=240)
		return 1;
	return 0;
}

//从DS18B20读取一个位
//返回值：0/1
uint8_t DS18B20_Read_Bit(void)
{
	uint8_t data=0;
	uint32_t temp=0;
	Output_DQ;
	Reset_DQ;
	SysCtlDelay(80/3);//延时2us
	Set_DQ;
	Input_DQ;
	SysCtlDelay(160);//延时12us
	temp=Status_DQ;
	if (temp) data=1;
	else data=0;
	SysCtlDelay(2000/3);//延时50us
	return data;
}

//从DS18B20读取一个字节
//返回值：读到的数据
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

//写一个字节到DS18B20
//data：要写入的字节
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
			SysCtlDelay(80/3);//延时2us
			Set_DQ;
			SysCtlDelay(800);//延时60us
		}
		else
		{
			Reset_DQ;
			SysCtlDelay(800);//延时60us
			Set_DQ;
			SysCtlDelay(80/3);//延时2us
		}
	}
}

//开始温度转换
void DS18B20_Start_Convert(void)
{
	DS18B20_Reset();
	DS18B20_Check();
	DS18B20_Write_Byte(0xcc);
	DS18B20_Write_Byte(0x44);
}

//初始化DS18B20的IO口DQ同时检测DS18B20的存在
//DQ为PA6
//返回1：未检测到DS18B20
//返回0：存在
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

//从DS18B20得到温度值
//精度：0.1℃
//返回值：实际温度值*10（-550-1250）
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
