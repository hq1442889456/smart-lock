#include "main.h"                  // Device header

/*
Function name:Voice_Config
Description:����ģ���ʼ��
param:
retval:None
Remarks:
PC4--VOICE_BUSY -- ��������
PC5--VOICE_DATA -- ͨ�����
*/
void Voice_Config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);//PCʱ��ʹ��
	GPIO_InitTypeDef gpio_InitTypeDef;
	gpio_InitTypeDef.GPIO_Mode = GPIO_Mode_IN;
	gpio_InitTypeDef.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOC,&gpio_InitTypeDef);//PC4 ����
	gpio_InitTypeDef.GPIO_Mode = GPIO_Mode_OUT;
	gpio_InitTypeDef.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOC,&gpio_InitTypeDef);//PC5 ���
	Send_L;//���е�
}

/*
Function name:Voice_Work
Description:����ģ�鿪ʼ����
param:�ֲ��еİ�λЭ����
retval:None
Remarks:
PC4--VOICE_BUSY -- ��������
PC5--VOICE_DATA -- ͨ�����
*/
void Voice_Work(u8 data)
{
	u8 i;
	
	Voice_Start();//ͬ��ͷ
	for(i=0;i<8;i++)
	{
		if(data&(0x80>>i))
		{
			Voice_Data(1);
		}
		else
		{
			Voice_Data(0);
		}
	}
	while(Recv_Busy);//�ȴ���������
	TIM5_Delay_Ms(500);//�������������ʱ
	
}
/*
Function name:Voice_Start
Description:MCU�������ݵ�ͬ��ͷ
param:None
retval:None
Remarks:
ͬ��ͷΪ 8MS �ߺ� 1MS �����
*/
void Voice_Start(void)
{
	Send_H;
	TIM5_Delay_Ms(8);//8ms�ߵ�ƽ
	Send_L;
	TIM5_Delay_Ms(1);//1ms�͵�ƽ
}

/*
Function name:Voice_Data
Description:MCU��������
param:u8
retval:None
Remarks:
���� ��0�� �� 0.5MS �ߵ�ƽ�� 1.5MS �͵�ƽ��ɡ�
���� ��1�� �� 1.5MS �ߵ�ƽ�� 0.5MS �͵�ƽ��ɡ�

*/
void Voice_Data(u8 data)
{
	if(data)//��������1
	{
		Send_H;
		TIM5_Delay_Us(1500);
		Send_L;
		TIM5_Delay_Us(500);
	}
	else if(data==0)//��������0
	{
		Send_H;
		TIM5_Delay_Us(500);
		Send_L;
		TIM5_Delay_Us(1500);
	}
}


void VOICE_Set_Volume(void)
{
	Voice_Work(0X09);//��������
	printf("��������");
	LCD_Clear(0,239,0,239,WHITE);//����
	LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"��*���Ӵ���������#�ż�С����");
	LCDFONT_Display_Str(70,0,BLUE,WHITE,16,(u8 *)"��0���˳�");
	while(1)
	{
		while(BS_IRQ);//�ȴ���������
		
		if(!BS_IRQ)
		{	
			u8 key=Get_BsKey_Value();
			if(key=='*')
			{
				Voice_Work(volume--);
				if(volume<=0x20)
				{
					volume=0x20;
				}
			}
			else if(key=='#')
			{
				Voice_Work(volume++);
				if(volume>=0x27)
				{
					volume=0x27;
				}
			}
			else if(key=='0')
			{
				break;
			}
			Voice_Work(0X1d);//�   Ҫ���ڻ�ȡ��ֵ֮���������IICʱ��
			printf("%#x ",volume);
			while(!BS_IRQ);//̧����ܳ��ȴ�
		}			
	}
	Voice_Work(0X11);//�����ɹ�
	LCD_Clear(0,239,0,239,WHITE);//����
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"�����ɹ�");
	Delay_Ms(500);
	lcd_flag=1;
}

