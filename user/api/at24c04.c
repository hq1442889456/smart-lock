#include "main.h"                  // Device header

/*
Function name:AT24C04_Config
Description:EEPROM AT24C04 IO��ʼ��
param:None
retval:None
Remarks:
PA8--AT24C04--SCLK -- ͨ���������  ��ֻ��MCU�������ʱ�ӣ���ֻ��MCU������ʱ���ߣ�
PC9--AT24C04--SDA -- ͨ�ÿ�©���  �������л�Ϊ���룩
*/
void AT24C04_Config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);//PCʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//PAʱ��ʹ��
	
	//PA8
	GPIO_InitTypeDef gpio_InitTypeDef;
	gpio_InitTypeDef.GPIO_Mode = GPIO_Mode_OUT;
	gpio_InitTypeDef.GPIO_OType = GPIO_OType_PP;//ͨ���������
	gpio_InitTypeDef.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOA,&gpio_InitTypeDef);
	
	//PC9
	gpio_InitTypeDef.GPIO_OType = GPIO_OType_OD;//ͨ�ÿ�©���
	gpio_InitTypeDef.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOC,&gpio_InitTypeDef);
}

/*
Function name:AT_Page_Write
Description:AT24C04 ҳд
param:None
retval:������
Remarks:addr-->д���ַ:0~15--��һҳ   16~31--�ڶ�ҳ ����
				buf-->��д�����ݵ��׵�ַ
				len-->���ݳ���
*/
u8 AT_Page_Write(u8 addr,u8 *buf,u8 len)
{
	u8 i;
	ATIIC_Start();
	if(ATIIC_Send_Data(AT_ADDR_W))
	{
		printf("Ӧ��1����\r\n");
		return 1;
	}
	if(ATIIC_Send_Data(addr))
	{
		printf("Ӧ��2����\r\n");
		return 2;
	}
	
	//��ʦ�ģ�Ч�ʣ�
//	while(len--)
//	{
//		if(ATIIC_Send_Data(*buf++))
//		{
//			printf("Ӧ��3����\r\n");
//			return 3;
//		}
//	}
	
	//�Լ��ģ��׶���
	for(i=0;i<len;i++)//д��len������
	{
		if(ATIIC_Send_Data(*buf))
		{
			printf("Ӧ��3����\r\n");
			return 3;
		}
		buf++;
	}
	ATIIC_Stop();
	TIM5_Delay_Ms(5);
	return 0;
}

/*
Function name:AT_Random_Read
Description:�����
param:addr-->�������ݵ��׵�ַ ��0~15--��һҳ   16~31--�ڶ�ҳ ����
			buf-->�������ݴ洢���׵�ַ
			len-->�������ݳ���
retval:������
Remarks:
*/
u8 AT_Random_Read(u8 addr,u8 *buf,u8 len)
{
	u8 i;
	ATIIC_Start();
	if(ATIIC_Send_Data(AT_ADDR_W))
	{
		printf("Ӧ��4����\r\n");
		return 1;
	}
	if(ATIIC_Send_Data(addr))
	{
		printf("Ӧ��5����\r\n");
		return 2;
	}
	ATIIC_Start();
	if(ATIIC_Send_Data(AT_ADDR_R))
	{
		printf("Ӧ��6����\r\n");
		return 3;
	}
	//��ʦ��
//	len--;
//	while(len--)//��ǰlen-1�����ݽ��պ���Ӧ��
//	{
//		*buf++ = ATIIC_Rev_Data(ACK);
//	}
//	*buf = ATIIC_Rev_Data(NACK);//�ý������һ�����ݺ���Ӧ���
	
	//�Լ���
	for(i=0;i<len-1;i++)//��ǰlen-1�����ݽ��պ���Ӧ��
	{
		*buf = ATIIC_Rev_Data(ACK);
		buf++;
	}
	*buf = ATIIC_Rev_Data(NACK);//�ý������һ�����ݺ���Ӧ���
	
	ATIIC_Stop();
	return 0;
}

/*
Function name:AT_Cross_Write
Description:��ҳд
param:addr-->д���ַ ��0~15--��һҳ   16~31--�ڶ�ҳ ����
			buf-->��д�����ݵ��׵�ַ
			len-->���ݳ���
retval:������
Remarks:
*/
u8 AT_Cross_Write(u8 addr,u8 *buf,u8 len)
{
	u8 len_rest;
	len_rest=16-addr%16;//��ҳʣ����
	
	//��ʦ��
//	if(len_rest>=len)//����Ҫ��ҳ
//	{
//		len_rest=len;
//	}
//	while(1)
//	{
//		AT_Page_Write(addr,buf,len_rest);
//		if(len==len_rest)
//		{
//			break;
//		}
//		len-=len_rest;
//		addr+=len_rest;
//		buf+=len_rest;
//		if(len>16)
//		{
//			len_rest=16;
//		}
//		else
//		{
//			len_rest=len;
//		}
//	}

	//�Լ�д
	if(len_rest>=len)//����Ҫ��ҳ
	{
		AT_Page_Write(addr,buf,len);//ʣ�����㹻д���ݸ�����ֱ��д��ʣ��ռ伴��
	}
	else
	{
		while(1)
		{
			AT_Page_Write(addr,buf,len_rest);//��һ�ν�����ʣ��Ŀռ�����
			if(len==len_rest)//��������
			{
				break;
			}
			len-=len_rest;//ҳд��������ָ��ƫ�����ᱻ���� ������Ҫƫ��
			addr+=len_rest;
			buf+=len_rest;
			if(len>16)
			{
				len_rest = 16;
			}
			else
			{
				len_rest = len;
			}
		}
	}
	
	
	return 0;
}




