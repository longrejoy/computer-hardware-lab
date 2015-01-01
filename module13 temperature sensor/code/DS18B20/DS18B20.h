#include <stdint.h>

#define		DQ			PA6

//����DQ
#define		Set_DQ		GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_6,0x40);
//����DQ
#define		Reset_DQ	GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_6,0x00);
//����DQΪ����
#define		Input_DQ	GPIOPinTypeGPIOInput(GPIO_PORTA_BASE,GPIO_PIN_6)
//����DQΪ���
#define		Output_DQ	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE,GPIO_PIN_6)
//��ȡDQ��ǰ״̬
#define		Status_DQ	GPIOPinRead(GPIO_PORTA_BASE,GPIO_PIN_6);

void DS18B20_Reset(void);
uint8_t DS18B20_Check(void);
uint8_t DS18B20_Read_Bit(void);
uint8_t DS18B20_Read_Byte(void);
void DS18B20_Write_Byte(uint8_t data);
void DS18B20_Start_Convert(void);
uint8_t DS18B20_Init(void);
uint32_t DS18B20_Get_Temp(void);

