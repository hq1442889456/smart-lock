#include "main.h"                  // Device header
/*
Function name:Usart6_Config
Description:����6 IO��ʼ��
param:brr ������  
retval:None
Remarks:
PC6--TX:�����������
PC7--RX:��������
*/
void Usart6_Config(u32 brr)
{
	//ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);//��PCʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//����ʱ��ʹ��
	//IO
	GPIO_InitTypeDef GPIO_InitStruct={0};//����IO�ڳ�ʼ���ṹ�����
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;//����ģʽ
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;//PC6��PC7����
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6);//���Ÿ���
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6);//���Ÿ���   Ҫ�ڳ�ʼ��GPIO�ṹ��֮ǰ���ã�������ڴ�ӡǰ��һ����
	GPIO_Init(GPIOC,&GPIO_InitStruct);//ʹ��IO�ڳ�ʼ���ṹ��
	USART_InitTypeDef USART_InitStruct={0};//���崮�ڳ�ʼ���ṹ�����
	USART_InitStruct.USART_BaudRate = brr;//������
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;//���ͺͽ�����ʹ��
	USART_InitStruct.USART_Parity = USART_Parity_No;//����żУ��
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//һλֹͣλ
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//8λ����λ
	USART_Init(USART6,&USART_InitStruct);//ʹ�ܴ��ڳ�ʼ���ṹ��
	USART_ITConfig(USART6,USART_IT_RXNE,ENABLE);//���ڽ����ж�ʹ��
	USART_ITConfig(USART6,USART_IT_IDLE,ENABLE);//���ڿ����ж�ʹ��
	//NVIC���ȼ�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef nvic_InitTypeDef={0};
	nvic_InitTypeDef.NVIC_IRQChannel = USART6_IRQn;
	nvic_InitTypeDef.NVIC_IRQChannelCmd = ENABLE;
	nvic_InitTypeDef.NVIC_IRQChannelPreemptionPriority = 1;
	nvic_InitTypeDef.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&nvic_InitTypeDef);
	USART_Cmd(USART6,ENABLE);//����ʹ��
}

/*
Function name:Usart6_Send_Byte
Description:����6�����ֽں���
param:data  
retval:None
Remarks:
*/
void Usart6_Send_Byte(u8 data)
{
	while(USART_GetFlagStatus(USART6,USART_FLAG_TXE)==RESET);
	USART_SendData(USART6,data);//TXE flag is cleared only by a write to the USART_DR register (USART_SendData()).
}

///*
//Function name:Usart6_Rev_Byte
//Description:����6�����ֽں���
//param:None
//retval:u8
//Remarks:
//*/
//u8 Usart6_Rev_Byte(void)
//{
//	u8 data;
//	while(USART_GetFlagStatus(USART6,USART_FLAG_RXNE)==RESET);
//	data=USART_ReceiveData(USART6);//RXNE flag can be also cleared by a read to the USART_DR register (USART_ReceiveData()).
//	return data;
//}



//����6�жϷ�����
u8 mg_rec_Buf[10];//�������ݰ�
u8 mg_flag;//����״̬��־
void USART6_IRQHandler(void)
{
	static u8 i = 0;
	if(USART_GetITStatus(USART6,USART_IT_RXNE)==SET)
	{
		mg_rec_Buf[i++] = USART_ReceiveData(USART6);//RXNE pending bit can be also cleared by a read to the USART_DR register (USART_ReceiveData()).
	}
	if(USART_GetITStatus(USART6,USART_IT_IDLE)==SET)//�������
	{
		/*IDLE (Idle line detected) pending bits are cleared by 
  *software sequence: a read operation to USART_SR register 
  *(USART_GetITStatus()) followed by a read operation to USART_DR register 
  *(USART_ReceiveData()).*/
		USART_GetITStatus(USART6,USART_IT_IDLE);
		USART_ReceiveData(USART6);
//		USART6->SR;
//		USART6->DR;//������
		i = 0;//��̬�������ֶ����㣬Ϊ��һ�ν�����׼��
		mg_flag = 1;//������ɱ�־
	}
}

/*
Function name:MG200_Config
Description:MG200 IO��ʼ��
param:brr ������  
retval:None
Remarks:
MG200_DETECT--PC0--����
MG200_PWR   --PC1--ͨ���������
*/
void MG200_Config(void)
{
	//ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);//��PCʱ��
	//IO
	GPIO_InitTypeDef GPIO_InitStruct={0};//����IO�ڳ�ʼ���ṹ�����
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;//����ģʽ
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;//PC0����
	GPIO_Init(GPIOC,&GPIO_InitStruct);//ʹ��IO�ڳ�ʼ���ṹ��
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//���ģʽ
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;//PC1����
	GPIO_Init(GPIOC,&GPIO_InitStruct);//ʹ��IO�ڳ�ʼ���ṹ��
}

/*
Function name:MG200_Init
Description:MG200��ʼ��
param:brr ������  
retval:None
Remarks:
PC6--TX:�����������
PC7--RX:��������
MG200_DETECT--PC0--����
MG200_PWR   --PC1--ͨ���������
*/
void MG200_Init(void)
{
	Usart6_Config(115200);//�ֲ�Ĭ��ֵ
	MG200_Config();
//	MG200_PWR_ON;//��ʼ��
}

/*
Function name:MG_Send_Pack
Description:MCU�������ݰ�����
param:cmd-->ָ����  
			param-->����
retval:None
Remarks:ÿ�η������ݰ��� 7 �ֽ�
*/
void MG_Send_Pack(u8 cmd,u8 param)
{
	u8 check_sum=0;
	Usart6_Send_Byte(0x6c);//��ʼ��
	Usart6_Send_Byte(0x63);//���͵�ַ
	Usart6_Send_Byte(0x62);//���յ�ַ
	Usart6_Send_Byte(cmd);//ָ����
	Usart6_Send_Byte(param);//����
	Usart6_Send_Byte(0x00);//Ԥ��
	check_sum = (0x63+0x62+cmd+param)&0xff;//У���  &0xff��Ϊ�˵õ���λ����
	Usart6_Send_Byte(check_sum);//Ч����
}
/*
Function name:MG_Rev_Pack
Description:MG200�������հ�����
param:cmd-->Ӧ����
			res-->����ֵ�ĵ�ַ
			param-->�����ĵ�ַ
retval:״̬��(0:��ȷ ����:����)
Remarks:ÿ�ν������ݰ��� 8 �ֽ�
*/
u8 MG_Rev_Pack(u8 cmd,u8 *res,u8 *param)
{
	u8 check_sum= 0; 
	
	while(!mg_flag);//�ȴ����ձ�־λ��1
	for(u8 i = 0;i<8;i++)
	{
		printf("%#x ",mg_rec_Buf[i]);
	}
	mg_flag = 0;//���㣬��ֹ�´ν���ֱ�ӽ���
	if(mg_rec_Buf[0] != 0x6c)
	return 1;//������
	if(mg_rec_Buf[1] != 0x62)
	return 2;//������
	if(mg_rec_Buf[2] != 0x63)
	return 3;//������
	if(mg_rec_Buf[3] != cmd)
	return 4;//������
	*res = mg_rec_Buf[4];//��ȡ����ֵ   ��Ϊֻ�ܷ���һ�����ݣ�����ͨ�βεķ�ʽ���س�ȥ�����ݣ���Ϊÿ��ָ��ķ���ֵ�Ͳ�����һ��������ֱ���ں������жϣ�Ҫ��ֵ���س�ȥ�ڹ��ܺ����ڲ������ж�
	*param = mg_rec_Buf[5];//��ȡ����
	if(mg_rec_Buf[6] != 0x00)
	return 5;//������
	check_sum = (0x62+0x63+cmd+*res+*param)&0xff;
	if(mg_rec_Buf[7]!=check_sum)
	return 6;//������
	return 0;//��ȷ��
}

/*
Function name:CaptureAndExtract
Description:MG_200ץȡָ��ͼ�����ȡ������
param:parameter-->����
			3~5����ȡ(0x00 0x01 0x02 0x03 0x04)
retval:״̬��(0��ȷ �������)
Remarks:ָ����--->0x51
*/
u8 CaptureAndExtract(u8 parameter)
{
	u8 res,param=0;
	MG_Send_Pack(0x51,parameter);//����ץȡָ��ָ��
	if(MG_Rev_Pack(0x51,&res,&param))
	{
		printf("�������ݳ���\r\n");
		return 1;
	}
	switch(res)
	{
		case 0xB1:printf("ָ�ƹ�С(small finger)\r\n");break;
		case 0xB2:printf("��ָ�� (no finger)\r\n");break;
		case 0xB3:printf("ָ��λ�ù��ڿ�����Ҫ����ָ���ұ�������left finger��\r\n");break;
		case 0xB4:printf("ָ��λ�ù��ڿ��ң���Ҫ����ָ�����������right finger��\r\n");break;
		case 0xB5:printf("ָ��λ�ù��ڿ��ϣ���Ҫ����ָ���±�������up finger��\r\n");break;
		case 0xB6:printf("ָ��λ�ù��ڿ��£���Ҫ����ָ���ϱ����� (down finger��\r\n");break;
		case 0xB7:printf("ʪ��ָ ��ָ�Ʊ����������ˮ�չ��ࣩ (wet finger)\r\n");break;
		case 0xB8:printf(" ������ָ(dry finger)\r\n");break;
		case 0xC0:printf(" ������ȡ����\r\n");break;
		case 0xC2:printf(" ������ȡ���� (ͼ��������)\r\n");break;
		default:printf("ץȡָ��ͼ����������ȡ�ɹ�\r\n");
	}
	return 0;
}
/*
Function name:Enroll
Description:MG_200ע���û�
param:parameter-->����
			ע��� ID �ţ�ID ��Χ 1~100������û���Ϊ 100�� ���ò���Ϊ 00h ʱ��ģ��
			ע��ɹ��� ID ����ģ���ڲ��Զ�����ģ�
retval:״̬��(0��ȷ �������)
Remarks:ָ����--->0x7f
*/
u8 Enroll(u8 parameter)
{
	u8 result,res,param=0;
	
	MG200_PWR_ON;
	Delay_Ms(30);//�ȴ��ϵ�
	printf("��һ�η�����ָ\r\n");
	Voice_Work(0X16);//�����ָ
	LCD_Clear(0,239,0,239,WHITE);//����
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"�����ָ");
	Delay_Ms(500);
	while(!MG200_DETECT);//�ȴ���ָ����
	do{
		result = CaptureAndExtract(0x00);//��һ����ȡ
	}while(result);
	printf("���ɿ���ָ\r\n");
	LCD_Clear(0,239,0,239,WHITE);//����
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"���ɿ���ָ");
	Delay_Ms(500);
	while(MG200_DETECT);//�ȴ���ָ�ɿ�
	printf("�ڶ��η�����ָ\r\n");
	Voice_Work(0X16);//�����ָ
	LCD_Clear(0,239,0,239,WHITE);//����
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"�ڶ��η�����ָ");
	Delay_Ms(500);
	while(!MG200_DETECT);//�ȴ���ָ����
	do{
		result = CaptureAndExtract(0x01);//�ڶ�����ȡ
	}while(result);
	printf("���ɿ���ָ\r\n");
	LCD_Clear(0,239,0,239,WHITE);//����
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"���ɿ���ָ");
	
	while(MG200_DETECT);//�ȴ���ָ�ɿ�
	printf("�����η�����ָ\r\n");
	Voice_Work(0X16);//�����ָ
	LCD_Clear(0,239,0,239,WHITE);//����
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"�����η�����ָ");
	
	while(!MG200_DETECT);//�ȴ���ָ����
	do{
		result = CaptureAndExtract(0x02);//��������ȡ
	}while(result);
	printf("���ɿ���ָ\r\n");
	LCD_Clear(0,239,0,239,WHITE);//����
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"���ɿ���ָ");
	
	while(MG200_DETECT);//�ȴ���ָ�ɿ�
	do{
		MG_Send_Pack(0x7F,parameter);//����ע��ָ��
		result = (MG_Rev_Pack(0x7F,&res,&param));
	}while(result);//ֱ�������ɹ��ų�ѭ��
	
//	if(MG_Rev_Pack(0x7F,&res,&param))
//	{
//		printf("����ʧ��!\r\n");
//		MG200_PWR_OFF;//�ر�ʡ��,����͹���
//		return 1;
//	}
	switch(res)
	{
		case 0x83:printf("ID ����(ID < 0 ���� ID > ����û���)����ͨ�Ŵ���!\r\n");
		LCD_Clear(0,239,0,239,WHITE);//����
		LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"ID ����(ID < 0 ���� ID > ����û���)����ͨ�Ŵ���!");
		
		break;
		case 0x91:printf("ע��ʧ��(�û���������)\r\n");
		LCD_Clear(0,239,0,239,WHITE);//����
		LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"ע��ʧ��(�û���������)");
		
		break;
		case 0x93:printf("�Ѿ�ע��� ID\r\n");
		LCD_Clear(0,239,0,239,WHITE);//����
		LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"�Ѿ�ע��� ID");
		
		break;
		case 0x94:printf("ָ����ȡ���� < 3\r\n");
		LCD_Clear(0,239,0,239,WHITE);//����
		LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"ָ����ȡ���� < 3");
		
		break;
		default:printf("ע��ɹ�\r\n");Voice_Work(0X11);//�����ɹ�
		LCD_Clear(0,239,0,239,WHITE);//����
		LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"�����ɹ�");
		
	}
	Delay_Ms(500);
	MG200_PWR_OFF;//�ر�ʡ��
	return 0;
}

/*
Function name:EraseAll
Description:MG_200ɾ�������û�
param:None
retval:״̬��(0��ȷ �������)
Remarks:ָ����--->0x54
*/
u8 EraseAll(void)
{
	u8 res,param,result = 0;
	MG200_PWR_ON;
	Delay_Ms(30);//�ȴ��ϵ�
	do{
		MG_Send_Pack(0x54,0x00);//����ɾ�������û�ָ��
		result = MG_Rev_Pack(0x54,&res,&param);
	}while(result);
//	MG_Send_Pack(0x54,0x00);
//	if(MG_Rev_Pack(0x54,&res,&param))
//	{
//		printf("��������!\r\n");
//		MG200_PWR_OFF;//�ر�ʡ��
//		return 1;
//	}
	switch(res)
	{
		case 0x00:	printf("ɾ��ȫ���û���Ϣ�ɹ�\r\n");
					Voice_Work(0X11);//ɾ��ȫ���û���Ϣ�ɹ�
					printf("ɾ��ȫ���û���Ϣ�ɹ�");
					LCD_Clear(0,239,0,239,WHITE);//����
					LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"ɾ��ȫ���û���Ϣ�ɹ�");
					Delay_Ms(500);
					break;
		case 0x90:	printf("ɾ��ʧ�� (ע����û���Ϊ 0 ��ʱ��)\r\n");
					LCD_Clear(0,239,0,239,WHITE);//����
					LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"ɾ��ʧ�� (ע����û���Ϊ 0 ��ʱ��)");
					Delay_Ms(500);
					break;
		default:	printf("ɾ��ʧ��\r\n");
					LCD_Clear(0,239,0,239,WHITE);//����
					LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"ɾ��ʧ��");
					Delay_Ms(500);
				
	}
	MG200_PWR_OFF;//�ر�ʡ��
	return 0;
	
}

/*
Function name:Match1N
Description:MG_200�Ա������û�
param:None
retval:״̬��(0��ȷ �������)
Remarks:ָ����--->0x71
*/
u8 Match1N(void)
{
	u8 res,param,result = 0;
	u8 rev_buf[50]={0};
	MG200_PWR_ON;
	Delay_Ms(30);//�ȴ��ϵ�
	printf("��һ�η�����ָ\r\n");
	TIM9->CNT =0;
	TIM_Cmd(TIM9,ENABLE);//�򿪼�����
	while(!MG200_DETECT)
	{
		wait_flag2=1;
		if(wait_flag1)
		{
			wait_flag1=0;
			wait_flag2=0;
			MG200_PWR_OFF;//�ر�ʡ��
			return 0;
		}
	}//�ȴ���ָ����
	
	do{
		result = CaptureAndExtract(0x00);//��һ����ȡ
	}while(result);//ֱ����ȡ�ɹ��ų�ѭ��
	
	LCD_Clear(0,239,0,239,WHITE);//����
	LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"���ɿ���ָ");
	printf("���ɿ���ָ\r\n");
	while(MG200_DETECT);//�ȴ���ָ�ɿ�
	
	do{
		MG_Send_Pack(0x71,0x00);//���ͱȶ�ָ��
		result = MG_Rev_Pack(0x71,&res,&param);//�����������ݰ����
	}while(result);//ֱ�������ɹ��ų�ѭ��
	
	switch(res)
	{
		case 0x00:	printf("�ȶԳɹ�\r\n");	
					Voice_Work(0X18);//��ӭ�ؼ�
					LCD_Clear(0,239,0,239,WHITE);//����
					sprintf((char *)rev_buf,"�û�IDΪ��%d",param);
					LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)rev_buf);
					LCDFONT_Display_Str(70,0,BLUE,WHITE,16,(u8 *)"�ȶԳɹ�");
					Delay_Ms(500);
					Open_Door();
					page_flag=1;
					break;
		default:	printf("�ȶ�ʧ��\r\n");			
					Voice_Work(0X19);//����ʧ��
					LCD_Clear(0,239,0,239,WHITE);//����
					LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"�Ա�ʧ�ܣ�");
					Delay_Ms(500);
	}
	if(param)
	{
		printf("�û�ID���ǣ�%d\r\n",param);
	}
	MG200_PWR_OFF;//�ر�ʡ��	
	
	return 0;
}


void MG_Open_lock(void)
{
	LCD_Clear(0,239,0,239,WHITE);//����
	LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"�������ָ��");
	printf("�������ָ��\r\n");
	Voice_Work(0X16);//�����ָ
	Match1N();//ָ�ƶԱȺ���
	lcd_flag = 1;	
}

void MG_Enroll(void)
{
	Voice_Work(0X03);//�Ǽ�ָ��
	LCD_Clear(0,239,0,239,WHITE);//����
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"�Ǽ�ָ��:");
	Delay_Ms(500);
	Enroll(0x00);
	lcd_flag=1;
	
	
}

void MG_Delect_All(void)
{
	Voice_Work(0X06);//ɾ������ָ��
	printf("ɾ������ָ��");
	LCD_Clear(0,239,0,239,WHITE);//����
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"ɾ������ָ��");
	Delay_Ms(500);
	EraseAll();
	
	lcd_flag=1;
}




u8 MG_Del_By_Id(void)
{
	u8 res,param,result = 0;
	LCD_Clear(0,239,0,239,WHITE);//����
	LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"������Ҫɾ����ID");
	while(1)
	{
		if(!BS_IRQ)
		{
			key_v=Get_BsKey_Value();
			while(!BS_IRQ);
			printf("key_v1=%c\r\n",key_v);//��ӡ��ֵ���ж�λ��
			if(key_v)
			{
				MG200_PWR_ON;
				Delay_Ms(30);//�ȴ��ϵ�
				do{
					MG_Send_Pack(0x73,key_v-48);//����ɾ��ָ���û�ָ��
					result = MG_Rev_Pack(0x73,&res,&param);
				}while(result);
				switch(res)
				{
					case 0x00:	printf("ɾ���ɹ�\r\n");
								LCD_Clear(0,239,0,239,WHITE);//����
								LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"ɾ���ɹ�");
								break;
					case 0x83:	printf("��������(ID��0 ���� ID > ����û���)\r\n");
								LCD_Clear(0,239,0,239,WHITE);//����
								LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"��������(ID��0 ���� ID > ����û���)");
								break;
					case 0x90:	printf("δע����û�\r\n");
								LCD_Clear(0,239,0,239,WHITE);//����
								LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"δע����û�");
								break;
					case 0xff:	printf("д�� ROM ����\r\n");
								LCD_Clear(0,239,0,239,WHITE);//����
								LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"д�� ROM ����");
								break;	
				}
				Delay_Ms(500);
				MG200_PWR_OFF;//�ر�ʡ��
				lcd_flag=1;
				return 0;
			}
		}
		
	}
	
}
