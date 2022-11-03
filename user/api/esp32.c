#include "esp32.h"

/*
Function name:usart2_config
Description:串口初始化
param:brr 波特率
retval:None
Remarks:PA2 PA3
*/
void usart2_config(void)
{
	NVIC_InitTypeDef nvic_InitTypeDef;
	//1. 打开外设的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	//2. 配置PA2和PA3
	GPIO_InitTypeDef  GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;//配置为复用
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//推挽
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;//
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;//低速
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//3.把PA2和PA3复用到USART2中
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
	
	//4. 配置USART2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	USART_InitTypeDef  USART_InitStruct = {0};
	USART_InitStruct.USART_BaudRate = 115200;//波特率
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控制
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//使能发送接收
	USART_InitStruct.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b; 
	USART_Init(USART2,&USART_InitStruct);
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//使能接收中断
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);//使能空闲中断
	
	//优先级
	nvic_InitTypeDef.NVIC_IRQChannel=USART2_IRQn;//中断号
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//分组 第五组
	nvic_InitTypeDef.NVIC_IRQChannelPreemptionPriority=1;//占先
	nvic_InitTypeDef.NVIC_IRQChannelSubPriority=1;//次级
	nvic_InitTypeDef.NVIC_IRQChannelCmd=ENABLE;//使能
	NVIC_Init(&nvic_InitTypeDef);
	
	//5.使能USART2
	USART_Cmd(USART2,ENABLE);
	
}

/*
Function name:usart2_send_str
Description:串口2发送字符串
param:data 字符串首地址
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
//串口接收函数
void USART2_IRQHandler(void)
{
	u8 temp;
	if(USART_GetITStatus(USART2,USART_IT_RXNE))//触发接收中断
	{
		temp=USART_ReceiveData(USART2);//接收数据
		if(temp==0)//空格接收不到 
		{
			esp32rec.buff[esp32rec.len++]=' ';
		}
		else
		{
			esp32rec.buff[esp32rec.len++]=temp;
		}
	}
	if(USART_GetITStatus(USART2,USART_IT_IDLE))//触发空闲中断、接收完成
	{
		USART2->SR;//读清零
		USART2->DR;//读清零
		esp32rec.buff[esp32rec.len]='\0';//结束符
		esp32rec.flag=1;//接收完成标志
		esp32rec.len=0;//方便下次接收
	}
}


/*
Function name:Esp_Send_And_Ack
Description:发送指令并判断响应是否正常
						超时判断
param:cmd-->指令  eack-->响应  timeout-->超时等待时间
retval:0-->正常响应 1-->非正常响应
Remarks:指令 响应都是字符串
*/
u8 Esp_Send_And_Ack(u8 *cmd,u8 *eack,u32 timeout)
{
	u32 cnt=0;
	usart2_send_str(cmd);
	Delay_Ms(100);
	while(1)
	{
		while(!esp32rec.flag)//等待接收完成
		{
			cnt++;
			Delay_Ms(1);
			if(cnt>timeout)//超时
			{
				return 1;//返回错误码
			}
		}
		esp32rec.flag=0;
		if(strstr((char *)esp32rec.buff,(char *)eack)!=NULL)//返回包内有OK
		{
			return 0;//返回正确码
		}
	}
}

/*
Function name:Esp_Exit
Description:退出透传
param:None
retval:None
Remarks:发送+++ 不需要\r\n
*/
void Esp_Exit(void)
{
	usart2_send_str((u8 *)"+++");
	Delay_Ms(2000);
	printf("退出透传成功\r\n");
}


/*
Function name:Esp_RESTORE
Description:恢复出厂设置
param:None
retval:0-->正常响应 非零-->非正常响应
Remarks:发送AT+RESTORE\r\n
*/
u8 Esp_RESTORE(void)
{
	if(Esp_Send_And_Ack((u8 *)"AT+RESTORE\r\n",(u8 *)"ready",3000)==0)
	{
		printf("恢复出厂设置成功\r\n");
		return 0;
	}
	else
	{
		printf("恢复出厂设置失败\r\n");
		return 1;
	}
}


/*
Function name:Esp_Init
Description:初始化
param:None
retval:None
Remarks:
*/
void Esp_Init(void)
{
	usart2_config();
	Esp_Send_And_Ack((u8 *)"AT\r\n",(u8 *)"OK",10);//无意义的起始指令、第一条测试指令
	if(Esp_Send_And_Ack((u8 *)"AT\r\n",(u8 *)"OK",10)!=0)
	{
		Esp_Exit();//退出透传模式
	}
	Esp_RESTORE();//恢复出厂设置
	//设置模式
	if(Esp_Send_And_Ack((u8 *)"AT+CWMODE=1\r\n",(u8 *)"OK",10)==0)
	{
		printf("模式设置成功\r\n");
	}
	else
	{
		printf("模式设置失败\r\n");
	}
}


/*
Function name:Esp_Connect_AP
Description:连接AP
param:ssid-->WiFi名  pwd-->WiFi密码
retval:0-->正常响应 非零-->非正常响应
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
		printf("wifi连接成功\r\n");
		return 0;
	}
	else
	{
		printf("wifi连接失败\r\n");
		return 1;
	}
	
}


/*
Function name:Esp_Connect_Tcp
Description:连接阿里云
param:
retval:0-->正常响应 非零-->非正常响应
Remarks:
AT+CIPSTART="TCP","a1xCs8ynKqM.iot-as-mqtt.cn-shanghai.aliyuncs.com",1883\r\n

*/
u8 Esp_Connect_Tcp(u8 *ip,u32 id)
{
	u8 tcpbuff[200]={0};
	sprintf((char *)tcpbuff,"AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",ip,id);
	if(Esp_Send_And_Ack((u8 *)tcpbuff,(u8 *)"OK",5000)==0)
	{
		printf("tcp连接成功\r\n");
	}
	else
	{
		printf("tcp连接失败\r\n");
		return 1;
	}
	
	
	if(Esp_Send_And_Ack((u8 *)"AT+CIPMODE=1\r\n",(u8 *)"OK",10)==0)
	{
		printf("开启透传成功\r\n");
	}
	else
	{
		printf("开启透传失败\r\n");
		return 2;
	}
	
	if(Esp_Send_And_Ack((u8 *)"AT+CIPSEND\r\n",(u8 *)"OK",10)==0)
	{
		printf("开启发送数据成功\r\n");
	}
	else
	{
		printf("开启发送数据失败\r\n");
		return 3;
	}
	
	return 0;
}



//ESP32订阅
void ESP32_Subscription(void)
{
	Esp_Connect_AP((u8 *)"der",(u8 *)"huangqilol");//热点名和密码
	errorCode = Esp_Connect_Tcp((u8 *)"h2z6ZiRMFQA.iot-as-mqtt.cn-shanghai.aliyuncs.com",1883);
	if(!errorCode)
	{
		_mqtt.Init(rxbuf,0,txbuf,0);
		//MQTT连接
		errorCode=_mqtt.Connect("ESP32|securemode=3,signmethod=hmacsha1|",
		"smart_lock_XYD&h2z6ZiRMFQA",
		"0df4dcfa8391ccaf515d77c58d57069b32c70dbf");		
		if(errorCode==1)
		{
			printf("MQTT接入成功\r\n");
			errorCode=_mqtt.SubscribeTopic("/sys/h2z6ZiRMFQA/smart_lock_XYD/thing/service/property/set",0,1);//订阅
			if(errorCode==1)
			{
				printf("MQTT订阅成功\r\n");
			}
		}
	}
}



//ESP32发布
void ESP32_Publish(u8 status)
{
	//发布  可以发送门锁信息到阿里云
	//开门关门状态 1 ：开门 0： 关门
	sprintf(string, "{\"method\":\"thing.service.property.set\",\"id\":\"89942833\",\"params\":{\"lock_status\":%d},\"version\":\"1.0.0\"}",status);
	_mqtt.PublishData("/sys/h2z6ZiRMFQA/smart_lock_XYD/thing/event/property/post",string,0);
}


//ESP32开锁
void ESP32_Open_Lock(void)
{
	esp32rec.flag = 0;
	printf("接收:%s\r\n",esp32rec.buff);
	u8 *p=(u8 *)strstr((char *)esp32rec.buff,"status");
	if(*(p+8)=='1')
	{
		LCD_Clear(0,239,0,239,WHITE);//清屏
		LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"欢迎回家！");
		Delay_Ms(500);
		Voice_Work(0X18);//欢迎回家
		Open_Door();
		sprintf(string, "{\"method\":\"thing.service.property.set\",\"id\":\"1821971172\",\"params\":{\"lock_status\":%d},\"version\":\"1.0.0\"}",1);//上传设备信息，开门状态
		_mqtt.PublishData("/sys/h2z6ZiRMFQA/smart_lock_XYD/thing/event/property/post",string,0);
		ESP32_Publish(1);//esp32上报阿里云状态：开门
		lcd_flag=1;
		page_flag=1;
		LED3_ON;
	}
	
}

void ESP32_Close_Lock(void)
{
	esp32rec.flag = 0;
	printf("接收:%s\r\n",esp32rec.buff);
	u8 *p=(u8 *)strstr((char *)esp32rec.buff,"status");
	if(*(p+8)=='0')
	{
		LED3_OFF;
		Close_Door();
		page_flag=0;
		ESP32_Publish(0);//esp32上报阿里云状态：关门
	}
}
