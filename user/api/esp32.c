#include "esp32.h"

/*
Function name:usart2_config
Description:���ڳ�ʼ��
param:brr ������
retval:None
Remarks:PA2 PA3
*/
void usart2_config(void)
{
	NVIC_InitTypeDef nvic_InitTypeDef;
	//1. �������ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	//2. ����PA2��PA3
	GPIO_InitTypeDef  GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;//����Ϊ����
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//����
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;//
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;//����
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//3.��PA2��PA3���õ�USART2��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
	
	//4. ����USART2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	USART_InitTypeDef  USART_InitStruct = {0};
	USART_InitStruct.USART_BaudRate = 115200;//������
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ��������
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//ʹ�ܷ��ͽ���
	USART_InitStruct.USART_Parity = USART_Parity_No;//����żУ��
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b; 
	USART_Init(USART2,&USART_InitStruct);
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//ʹ�ܽ����ж�
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);//ʹ�ܿ����ж�
	
	//���ȼ�
	nvic_InitTypeDef.NVIC_IRQChannel=USART2_IRQn;//�жϺ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//���� ������
	nvic_InitTypeDef.NVIC_IRQChannelPreemptionPriority=1;//ռ��
	nvic_InitTypeDef.NVIC_IRQChannelSubPriority=1;//�μ�
	nvic_InitTypeDef.NVIC_IRQChannelCmd=ENABLE;//ʹ��
	NVIC_Init(&nvic_InitTypeDef);
	
	//5.ʹ��USART2
	USART_Cmd(USART2,ENABLE);
	
}

/*
Function name:usart2_send_str
Description:����2�����ַ���
param:data �ַ����׵�ַ
retval:None
Remarks:
*/
void usart2_send_str(u8 *data)
{
	while(*data!='\0')
	{
		while(!(USART2->SR & (1<<6)));
		USART2->DR=*data++;
	}
}


USA2_REC esp32rec={0};
//���ڽ��պ���
void USART2_IRQHandler(void)
{
	u8 temp;
	if(USART_GetITStatus(USART2,USART_IT_RXNE))//���������ж�
	{
		temp=USART_ReceiveData(USART2);//��������
		if(temp==0)//�ո���ղ��� 
		{
			esp32rec.buff[esp32rec.len++]=' ';
		}
		else
		{
			esp32rec.buff[esp32rec.len++]=temp;
		}
	}
	if(USART_GetITStatus(USART2,USART_IT_IDLE))//���������жϡ��������
	{
		USART2->SR;//������
		USART2->DR;//������
		esp32rec.buff[esp32rec.len]='\0';//������
		esp32rec.flag=1;//������ɱ�־
		esp32rec.len=0;//�����´ν���
	}
}


/*
Function name:Esp_Send_And_Ack
Description:����ָ��ж���Ӧ�Ƿ�����
						��ʱ�ж�
param:cmd-->ָ��  eack-->��Ӧ  timeout-->��ʱ�ȴ�ʱ��
retval:0-->������Ӧ 1-->��������Ӧ
Remarks:ָ�� ��Ӧ�����ַ���
*/
u8 Esp_Send_And_Ack(u8 *cmd,u8 *eack,u32 timeout)
{
	u32 cnt=0;
	usart2_send_str(cmd);
	Delay_Ms(100);
	while(1)
	{
		while(!esp32rec.flag)//�ȴ��������
		{
			cnt++;
			Delay_Ms(1);
			if(cnt>timeout)//��ʱ
			{
				return 1;//���ش�����
			}
		}
		esp32rec.flag=0;
		if(strstr((char *)esp32rec.buff,(char *)eack)!=NULL)//���ذ�����OK
		{
			return 0;//������ȷ��
		}
	}
}

/*
Function name:Esp_Exit
Description:�˳�͸��
param:None
retval:None
Remarks:����+++ ����Ҫ\r\n
*/
void Esp_Exit(void)
{
	usart2_send_str((u8 *)"+++");
	Delay_Ms(2000);
	printf("�˳�͸���ɹ�\r\n");
}


/*
Function name:Esp_RESTORE
Description:�ָ���������
param:None
retval:0-->������Ӧ ����-->��������Ӧ
Remarks:����AT+RESTORE\r\n
*/
u8 Esp_RESTORE(void)
{
	if(Esp_Send_And_Ack((u8 *)"AT+RESTORE\r\n",(u8 *)"ready",3000)==0)
	{
		printf("�ָ��������óɹ�\r\n");
		return 0;
	}
	else
	{
		printf("�ָ���������ʧ��\r\n");
		return 1;
	}
}


/*
Function name:Esp_Init
Description:��ʼ��
param:None
retval:None
Remarks:
*/
void Esp_Init(void)
{
	usart2_config();
	Esp_Send_And_Ack((u8 *)"AT\r\n",(u8 *)"OK",10);//���������ʼָ���һ������ָ��
	if(Esp_Send_And_Ack((u8 *)"AT\r\n",(u8 *)"OK",10)!=0)
	{
		Esp_Exit();//�˳�͸��ģʽ
	}
	Esp_RESTORE();//�ָ���������
	//����ģʽ
	if(Esp_Send_And_Ack((u8 *)"AT+CWMODE=1\r\n",(u8 *)"OK",10)==0)
	{
		printf("ģʽ���óɹ�\r\n");
	}
	else
	{
		printf("ģʽ����ʧ��\r\n");
	}
}


/*
Function name:Esp_Connect_AP
Description:����AP
param:ssid-->WiFi��  pwd-->WiFi����
retval:0-->������Ӧ ����-->��������Ӧ
Remarks:AT+CWJAP="fwh","fwh244466666"\r\n
*/
u8 Esp_Connect_AP(u8 *ssid,u8 *pwd)
{
	u8 wifibuff[100]="AT+CWJAP=\"";
	strcat((char *)wifibuff,(char *)ssid);
	strcat((char *)wifibuff,(char *)"\",\"");
	strcat((char *)wifibuff,(char *)pwd);
  strcat((char *)wifibuff,(char *)"\"\r\n");
	if(Esp_Send_And_Ack((u8 *)wifibuff,(u8 *)"OK",5000)==0)
	{
		printf("wifi���ӳɹ�\r\n");
		return 0;
	}
	else
	{
		printf("wifi����ʧ��\r\n");
		return 1;
	}
	
}


/*
Function name:Esp_Connect_Tcp
Description:���Ӱ�����
param:
retval:0-->������Ӧ ����-->��������Ӧ
Remarks:
AT+CIPSTART="TCP","a1xCs8ynKqM.iot-as-mqtt.cn-shanghai.aliyuncs.com",1883\r\n

*/
u8 Esp_Connect_Tcp(u8 *ip,u32 id)
{
	u8 tcpbuff[200]={0};
	sprintf((char *)tcpbuff,"AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",ip,id);
	if(Esp_Send_And_Ack((u8 *)tcpbuff,(u8 *)"OK",5000)==0)
	{
		printf("tcp���ӳɹ�\r\n");
	}
	else
	{
		printf("tcp����ʧ��\r\n");
		return 1;
	}
	
	
	if(Esp_Send_And_Ack((u8 *)"AT+CIPMODE=1\r\n",(u8 *)"OK",10)==0)
	{
		printf("����͸���ɹ�\r\n");
	}
	else
	{
		printf("����͸��ʧ��\r\n");
		return 2;
	}
	
	if(Esp_Send_And_Ack((u8 *)"AT+CIPSEND\r\n",(u8 *)"OK",10)==0)
	{
		printf("�����������ݳɹ�\r\n");
	}
	else
	{
		printf("������������ʧ��\r\n");
		return 3;
	}
	
	return 0;
}



//ESP32����
void ESP32_Subscription(void)
{
	Esp_Connect_AP((u8 *)"der",(u8 *)"huangqilol");//�ȵ���������
	errorCode = Esp_Connect_Tcp((u8 *)"h2z6ZiRMFQA.iot-as-mqtt.cn-shanghai.aliyuncs.com",1883);
	if(!errorCode)
	{
		_mqtt.Init(rxbuf,0,txbuf,0);
		//MQTT����
		errorCode=_mqtt.Connect("ESP32|securemode=3,signmethod=hmacsha1|",
		"smart_lock_XYD&h2z6ZiRMFQA",
		"0df4dcfa8391ccaf515d77c58d57069b32c70dbf");		
		if(errorCode==1)
		{
			printf("MQTT����ɹ�\r\n");
			errorCode=_mqtt.SubscribeTopic("/sys/h2z6ZiRMFQA/smart_lock_XYD/thing/service/property/set",0,1);//����
			if(errorCode==1)
			{
				printf("MQTT���ĳɹ�\r\n");
			}
		}
	}
}



//ESP32����
void ESP32_Publish(u8 status)
{
	//����  ���Է���������Ϣ��������
	//���Ź���״̬ 1 ������ 0�� ����
	sprintf(string, "{\"method\":\"thing.service.property.set\",\"id\":\"89942833\",\"params\":{\"lock_status\":%d},\"version\":\"1.0.0\"}",status);
	_mqtt.PublishData("/sys/h2z6ZiRMFQA/smart_lock_XYD/thing/event/property/post",string,0);
}


//ESP32����
void ESP32_Open_Lock(void)
{
	esp32rec.flag = 0;
	printf("����:%s\r\n",esp32rec.buff);
	u8 *p=(u8 *)strstr((char *)esp32rec.buff,"status");
	if(*(p+8)=='1')
	{
		LCD_Clear(0,239,0,239,WHITE);//����
		LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"��ӭ�ؼң�");
		Delay_Ms(500);
		Voice_Work(0X18);//��ӭ�ؼ�
		Open_Door();
		sprintf(string, "{\"method\":\"thing.service.property.set\",\"id\":\"1821971172\",\"params\":{\"lock_status\":%d},\"version\":\"1.0.0\"}",1);//�ϴ��豸��Ϣ������״̬
		_mqtt.PublishData("/sys/h2z6ZiRMFQA/smart_lock_XYD/thing/event/property/post",string,0);
		ESP32_Publish(1);//esp32�ϱ�������״̬������
		lcd_flag=1;
		page_flag=1;
		LED3_ON;
	}
	
}

void ESP32_Close_Lock(void)
{
	esp32rec.flag = 0;
	printf("����:%s\r\n",esp32rec.buff);
	u8 *p=(u8 *)strstr((char *)esp32rec.buff,"status");
	if(*(p+8)=='0')
	{
		LED3_OFF;
		Close_Door();
		page_flag=0;
		ESP32_Publish(0);//esp32�ϱ�������״̬������
	}
}
