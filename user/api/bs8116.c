#include "main.h"                  // Device header


/*
Function name:BS8116_IO_Config
Description:���ݰ���BS8116 IO��ʼ��
param:None
retval:None
Remarks:
PB6--BS8116--SCLK -- ͨ���������  ��ֻ��MCU�������ʱ�ӣ���ֻ��MCU������ʱ���ߣ�
PB7--BS8116--SDA -- ͨ�ÿ�©���  �������л�Ϊ���룩
PA1--BS8116--IRQ--  ����         (�͵�ƽ��ʾ�������£������ֲ�IRQ������һ����˵)
*/
void BS8116_Config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);//PBʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//PAʱ��ʹ��
	//PB6
	GPIO_InitTypeDef gpio_InitTypeDef = {0};//���岢����  ��ΪIO�ṹ�������֮ǰ�����ù�
	gpio_InitTypeDef.GPIO_Mode = GPIO_Mode_OUT;
	gpio_InitTypeDef.GPIO_OType = GPIO_OType_PP;//ͨ���������
	gpio_InitTypeDef.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOB,&gpio_InitTypeDef);
	//PB7
	gpio_InitTypeDef.GPIO_OType = GPIO_OType_OD;//ͨ�ÿ�©���
	gpio_InitTypeDef.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOB,&gpio_InitTypeDef);
	//PA1
	gpio_InitTypeDef.GPIO_Mode = GPIO_Mode_IN;//ͨ������
	gpio_InitTypeDef.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOA,&gpio_InitTypeDef);
}



/*
Function name:Get_BsKey_Value
Description:BS8116��ȡ��ֵ
param:None
retval:��������ֵ
Remarks:�����ֲ�IIC�ӿ��½���ʱ��ͼ������ʱ��ͼ��д

*/
u8 Get_BsKey_Value(void)
{
	u16 key = 0;
	BSIIC_Start();
	if(BSIIC_Send_Data(BS_ADDR_W))
	{
		printf("Ӧ��1����\r\n");
		return 1;
	}
	if(BSIIC_Send_Data(0x08))
	{
		printf("Ӧ��2����\r\n");
		return 2;
	}
	BSIIC_Start();
	if(BSIIC_Send_Data(BS_ADDR_R))
	{
		printf("Ӧ��3����\r\n");
		return 3;
	}
	key = BSIIC_Rev_Data(ACK);//���յ�λ��ֵ�Ĵ�������
	key = BSIIC_Rev_Data(NACK)<<8 | key;//���ո�λ��ֵ�Ĵ������ݲ��͵�λ��ֵ�������
	BSIIC_Stop();
	switch(key)//BS8116�ֲ�û��һ����Ҫ����Ϣ���������İ����ߵ�λ�Ĵ�������λ�ĺ͵�λ�ļĴ��������λ����1������λֵ��0X8080
	{
		case 0x8081:return '1';//TK1  ����Ӳ���ж�
		case 0x8480:return '2';//TK11
		case 0x8080:return '3';//TK8
		case 0x8082:return '4';//TK2
		case 0x8880:return '5';//TK12
		case 0x80c0:return '6';//TK7
		case 0x8088:return '7';//TK4
		case 0x8180:return '8';//TK9
		case 0x80a0:return '9';//TK6
		case 0x8280:return '0';//TK10
		case 0x8084:return '*';//TK3
		case 0x8090:return '#';//TK5
	}
	return 0;
}


u8 page_flag;

void BS_Open_Lock(void)
{
	u8 i;
	LCD_Clear(0,239,0,239,WHITE);//����
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"���������룺");
	printf("���������룺\r\n");
	Voice_Work(0X0c);//���������Ա����
	TIM_Cmd(TIM9,ENABLE);//�򿪼�����
	for(i=0;i<6;i++)
	{
		TIM9->CNT =0;
		count=0;
		while(BS_IRQ)
		{
			wait_flag2=1;
			if(wait_flag1)
			{
				wait_flag1=0;
				wait_flag2=0;
				lcd_flag=1;
				return;//10sδ���������������
			}
		}//�ȴ���������
		if(!BS_IRQ)
		{
			
			key_buf[i]=Get_BsKey_Value();
			printf("%c\r\n",key_buf[i]);
			Voice_Work(0X1d);//�   Ҫ���ڻ�ȡ��ֵ֮���������IICʱ��
			LCDFONT_Display_ASCII(100,16*i,BLACK,WHITE,32,'*');//��LCD������ʾ*
			while(!BS_IRQ);//̧����ܳ��ȴ�
		}
	}
	key_buf[i]='\0';//�ַ���������־
	if(!strcmp((char *)key_buf,(char *)rev_buf))//����ȶ�
	{
		Voice_Work(0X18);//��ӭ�ؼ�
		printf("��ӭ�ؼ�\r\n");
		LCD_Clear(0,239,0,239,WHITE);//����
		LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"��ӭ�ؼң�");
		Delay_Ms(500);
		lcd_flag = 1;//������־λ
		page_flag = 1;
		Open_Door();
		TIM9->CNT =0;
		TIM_Cmd(TIM9,DISABLE);//�򿪼�����
		
	}
	else
	{
		Voice_Work(0X10);//���벻һ��
		printf("���벻һ��\r\n");
		LCD_Clear(0,239,0,239,WHITE);//����
		LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"���벻һ��");
		Delay_Ms(500);
		lcd_flag=1;//����
		TIM9->CNT =0;
		TIM_Cmd(TIM9,DISABLE);//�򿪼�����
	}
	
}





void BS_Change_Password(void)
{
	u8 i;
	LCD_Clear(0,239,0,239,WHITE);//����
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"�����������룺");
	printf("�����������룺\r\n");
	Voice_Work(0X0d);//������������
	for(i=0;i<6;i++)
	{
		while(BS_IRQ);//�ȴ���������
		if(!BS_IRQ)
		{
//								LCD_Display_Char(100,16*i,BLACK,WHITE,32,passward_buf);
			LCDFONT_Display_ASCII(100,16*i,BLACK,WHITE,32,'*');
			passward[i]=Get_BsKey_Value();
			printf("%c\r\n",passward[i]);
			Voice_Work(0X1d);//�   Ҫ���ڻ�ȡ��ֵ֮���������IICʱ��
			while(!BS_IRQ);//̧����ܳ��ȴ�
		}
	}
	passward[i] = '\0';
	LCD_Clear(0,239,0,239,WHITE);//����
	LCDFONT_Display_Str(50,0,BLUE,WHITE,24,(u8 *)"���ٴ�����������");
	printf("���ٴ�����������:\r\n");
	Voice_Work(0X0e);//���ٴ�����������
	for(i=0;i<6;i++)
	{
		while(BS_IRQ);//�ȴ���������
		if(!BS_IRQ)
		{
//								LCD_Display_Char(100,16*i,BLACK,WHITE,32,passward_buf);
			LCDFONT_Display_ASCII(100,16*i,BLACK,WHITE,32,'*');
			passward1[i]=Get_BsKey_Value();
			printf("%c\r\n",passward1[i]);
			Voice_Work(0X1d);//�   Ҫ���ڻ�ȡ��ֵ֮���������IICʱ��
			while(!BS_IRQ);//̧����ܳ��ȴ�
		}
	}
	passward1[i] = '\0';
	if(strcmp((char *)passward1,(char *)passward))
	{
		printf("���벻һ��\r\n");
		LCD_Clear(0,239,0,239,WHITE);//����
		LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"���벻һ�£�");
		LCDFONT_Display_Str(80,0,BLUE,WHITE,32,(u8 *)"�޸�ʧ�ܣ�");
		Delay_Ms(500);
		Voice_Work(0X10);//���벻һ�£�
		lcd_flag=1;
		password_flag=1;
	}
	if(!password_flag)
	{
		AT_Cross_Write(62,passward,sizeof(passward));//��EEPROMд������
		LCD_Clear(0,239,0,239,WHITE);//����
		printf("�޸�����ɹ�\r\n");
		LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"�޸�����ɹ�");
		Delay_Ms(500);
		Voice_Work(0X11);//�����ɹ���
		lcd_flag=1;
	
	}	
}




