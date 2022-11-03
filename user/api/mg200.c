#include "main.h"                  // Device header
/*
Function name:Usart6_Config
Description:串口6 IO初始化
param:brr 波特率  
retval:None
Remarks:
PC6--TX:复用推挽输出
PC7--RX:复用输入
*/
void Usart6_Config(u32 brr)
{
	//时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);//打开PC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//串口时钟使能
	//IO
	GPIO_InitTypeDef GPIO_InitStruct={0};//定义IO口初始化结构体变量
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;//复用模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;//PC6和PC7引脚
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6);//引脚复用
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6);//引脚复用   要在初始化GPIO结构体之前配置，否则会在打印前多一个？
	GPIO_Init(GPIOC,&GPIO_InitStruct);//使能IO口初始化结构体
	USART_InitTypeDef USART_InitStruct={0};//定义串口初始化结构体变量
	USART_InitStruct.USART_BaudRate = brr;//波特率
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;//发送和接收器使能
	USART_InitStruct.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//一位停止位
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//8位数据位
	USART_Init(USART6,&USART_InitStruct);//使能串口初始化结构体
	USART_ITConfig(USART6,USART_IT_RXNE,ENABLE);//串口接收中断使能
	USART_ITConfig(USART6,USART_IT_IDLE,ENABLE);//串口空闲中断使能
	//NVIC优先级分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef nvic_InitTypeDef={0};
	nvic_InitTypeDef.NVIC_IRQChannel = USART6_IRQn;
	nvic_InitTypeDef.NVIC_IRQChannelCmd = ENABLE;
	nvic_InitTypeDef.NVIC_IRQChannelPreemptionPriority = 1;
	nvic_InitTypeDef.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&nvic_InitTypeDef);
	USART_Cmd(USART6,ENABLE);//串口使能
}

/*
Function name:Usart6_Send_Byte
Description:串口6发送字节函数
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
//Description:串口6接收字节函数
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



//串口6中断服务函数
u8 mg_rec_Buf[10];//接收数据包
u8 mg_flag;//接收状态标志
void USART6_IRQHandler(void)
{
	static u8 i = 0;
	if(USART_GetITStatus(USART6,USART_IT_RXNE)==SET)
	{
		mg_rec_Buf[i++] = USART_ReceiveData(USART6);//RXNE pending bit can be also cleared by a read to the USART_DR register (USART_ReceiveData()).
	}
	if(USART_GetITStatus(USART6,USART_IT_IDLE)==SET)//接收完成
	{
		/*IDLE (Idle line detected) pending bits are cleared by 
  *software sequence: a read operation to USART_SR register 
  *(USART_GetITStatus()) followed by a read operation to USART_DR register 
  *(USART_ReceiveData()).*/
		USART_GetITStatus(USART6,USART_IT_IDLE);
		USART_ReceiveData(USART6);
//		USART6->SR;
//		USART6->DR;//读清零
		i = 0;//静态变量、手动清零，为下一次接收做准备
		mg_flag = 1;//接收完成标志
	}
}

/*
Function name:MG200_Config
Description:MG200 IO初始化
param:brr 波特率  
retval:None
Remarks:
MG200_DETECT--PC0--输入
MG200_PWR   --PC1--通用推挽输出
*/
void MG200_Config(void)
{
	//时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);//打开PC时钟
	//IO
	GPIO_InitTypeDef GPIO_InitStruct={0};//定义IO口初始化结构体变量
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;//输入模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;//PC0引脚
	GPIO_Init(GPIOC,&GPIO_InitStruct);//使能IO口初始化结构体
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//输出模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;//PC1引脚
	GPIO_Init(GPIOC,&GPIO_InitStruct);//使能IO口初始化结构体
}

/*
Function name:MG200_Init
Description:MG200初始化
param:brr 波特率  
retval:None
Remarks:
PC6--TX:复用推挽输出
PC7--RX:复用输入
MG200_DETECT--PC0--输入
MG200_PWR   --PC1--通用推挽输出
*/
void MG200_Init(void)
{
	Usart6_Config(115200);//手册默认值
	MG200_Config();
//	MG200_PWR_ON;//起始开
}

/*
Function name:MG_Send_Pack
Description:MCU发送数据包函数
param:cmd-->指令码  
			param-->参数
retval:None
Remarks:每次发送数据包共 7 字节
*/
void MG_Send_Pack(u8 cmd,u8 param)
{
	u8 check_sum=0;
	Usart6_Send_Byte(0x6c);//起始码
	Usart6_Send_Byte(0x63);//发送地址
	Usart6_Send_Byte(0x62);//接收地址
	Usart6_Send_Byte(cmd);//指令码
	Usart6_Send_Byte(param);//参数
	Usart6_Send_Byte(0x00);//预留
	check_sum = (0x63+0x62+cmd+param)&0xff;//校验和  &0xff是为了得到八位数据
	Usart6_Send_Byte(check_sum);//效验码
}
/*
Function name:MG_Rev_Pack
Description:MG200解析接收包函数
param:cmd-->应答码
			res-->返回值的地址
			param-->参数的地址
retval:状态码(0:正确 非零:错误)
Remarks:每次接受数据包共 8 字节
*/
u8 MG_Rev_Pack(u8 cmd,u8 *res,u8 *param)
{
	u8 check_sum= 0; 
	
	while(!mg_flag);//等待接收标志位置1
	for(u8 i = 0;i<8;i++)
	{
		printf("%#x ",mg_rec_Buf[i]);
	}
	mg_flag = 0;//清零，防止下次解析直接进入
	if(mg_rec_Buf[0] != 0x6c)
	return 1;//错误码
	if(mg_rec_Buf[1] != 0x62)
	return 2;//错误码
	if(mg_rec_Buf[2] != 0x63)
	return 3;//错误码
	if(mg_rec_Buf[3] != cmd)
	return 4;//错误码
	*res = mg_rec_Buf[4];//获取返回值   因为只能返回一个数据，所以通形参的方式返回出去的数据，因为每个指令的返回值和参数不一样，不可直接在函数内判断，要把值返回出去在功能函数内部进行判断
	*param = mg_rec_Buf[5];//获取参数
	if(mg_rec_Buf[6] != 0x00)
	return 5;//错误码
	check_sum = (0x62+0x63+cmd+*res+*param)&0xff;
	if(mg_rec_Buf[7]!=check_sum)
	return 6;//错误码
	return 0;//正确码
}

/*
Function name:CaptureAndExtract
Description:MG_200抓取指纹图像和提取特征点
param:parameter-->参数
			3~5次提取(0x00 0x01 0x02 0x03 0x04)
retval:状态码(0正确 非零错误)
Remarks:指令码--->0x51
*/
u8 CaptureAndExtract(u8 parameter)
{
	u8 res,param=0;
	MG_Send_Pack(0x51,parameter);//发送抓取指纹指令
	if(MG_Rev_Pack(0x51,&res,&param))
	{
		printf("解析数据出错！\r\n");
		return 1;
	}
	switch(res)
	{
		case 0xB1:printf("指纹过小(small finger)\r\n");break;
		case 0xB2:printf("无指纹 (no finger)\r\n");break;
		case 0xB3:printf("指纹位置过于靠左，需要将手指靠右边摁按（left finger）\r\n");break;
		case 0xB4:printf("指纹位置过于靠右，需要将手指靠左边摁按（right finger）\r\n");break;
		case 0xB5:printf("指纹位置过于靠上，需要将手指靠下边摁按（up finger）\r\n");break;
		case 0xB6:printf("指纹位置过于靠下，需要将手指靠上边摁按 (down finger）\r\n");break;
		case 0xB7:printf("湿手指 （指纹表面出汗或者水渍过多） (wet finger)\r\n");break;
		case 0xB8:printf(" 干燥手指(dry finger)\r\n");break;
		case 0xC0:printf(" 特征提取错误\r\n");break;
		case 0xC2:printf(" 特征提取错误 (图像质量差)\r\n");break;
		default:printf("抓取指纹图像及特征量提取成功\r\n");
	}
	return 0;
}
/*
Function name:Enroll
Description:MG_200注册用户
param:parameter-->参数
			注册的 ID 号（ID 范围 1~100，最大用户数为 100； 当该参数为 00h 时，模块
			注册成功的 ID 号是模块内部自动分配的）
retval:状态码(0正确 非零错误)
Remarks:指令码--->0x7f
*/
u8 Enroll(u8 parameter)
{
	u8 result,res,param=0;
	
	MG200_PWR_ON;
	Delay_Ms(30);//等待上电
	printf("第一次放下手指\r\n");
	Voice_Work(0X16);//请放手指
	LCD_Clear(0,239,0,239,WHITE);//清屏
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"请放手指");
	Delay_Ms(500);
	while(!MG200_DETECT);//等待手指按下
	do{
		result = CaptureAndExtract(0x00);//第一次提取
	}while(result);
	printf("请松开手指\r\n");
	LCD_Clear(0,239,0,239,WHITE);//清屏
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"请松开手指");
	Delay_Ms(500);
	while(MG200_DETECT);//等待手指松开
	printf("第二次放下手指\r\n");
	Voice_Work(0X16);//请放手指
	LCD_Clear(0,239,0,239,WHITE);//清屏
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"第二次放下手指");
	Delay_Ms(500);
	while(!MG200_DETECT);//等待手指按下
	do{
		result = CaptureAndExtract(0x01);//第二次提取
	}while(result);
	printf("请松开手指\r\n");
	LCD_Clear(0,239,0,239,WHITE);//清屏
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"请松开手指");
	
	while(MG200_DETECT);//等待手指松开
	printf("第三次放下手指\r\n");
	Voice_Work(0X16);//请放手指
	LCD_Clear(0,239,0,239,WHITE);//清屏
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"第三次放下手指");
	
	while(!MG200_DETECT);//等待手指按下
	do{
		result = CaptureAndExtract(0x02);//第三次提取
	}while(result);
	printf("请松开手指\r\n");
	LCD_Clear(0,239,0,239,WHITE);//清屏
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"请松开手指");
	
	while(MG200_DETECT);//等待手指松开
	do{
		MG_Send_Pack(0x7F,parameter);//发送注册指令
		result = (MG_Rev_Pack(0x7F,&res,&param));
	}while(result);//直到解析成功才出循环
	
//	if(MG_Rev_Pack(0x7F,&res,&param))
//	{
//		printf("解析失败!\r\n");
//		MG200_PWR_OFF;//关闭省电,进入低功耗
//		return 1;
//	}
	switch(res)
	{
		case 0x83:printf("ID 错误(ID < 0 或者 ID > 最大用户数)或者通信错误!\r\n");
		LCD_Clear(0,239,0,239,WHITE);//清屏
		LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"ID 错误(ID < 0 或者 ID > 最大用户数)或者通信错误!");
		
		break;
		case 0x91:printf("注册失败(用户区域已满)\r\n");
		LCD_Clear(0,239,0,239,WHITE);//清屏
		LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"注册失败(用户区域已满)");
		
		break;
		case 0x93:printf("已经注册的 ID\r\n");
		LCD_Clear(0,239,0,239,WHITE);//清屏
		LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"已经注册的 ID");
		
		break;
		case 0x94:printf("指纹提取次数 < 3\r\n");
		LCD_Clear(0,239,0,239,WHITE);//清屏
		LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"指纹提取次数 < 3");
		
		break;
		default:printf("注册成功\r\n");Voice_Work(0X11);//操作成功
		LCD_Clear(0,239,0,239,WHITE);//清屏
		LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"操作成功");
		
	}
	Delay_Ms(500);
	MG200_PWR_OFF;//关闭省电
	return 0;
}

/*
Function name:EraseAll
Description:MG_200删除所有用户
param:None
retval:状态码(0正确 非零错误)
Remarks:指令码--->0x54
*/
u8 EraseAll(void)
{
	u8 res,param,result = 0;
	MG200_PWR_ON;
	Delay_Ms(30);//等待上电
	do{
		MG_Send_Pack(0x54,0x00);//发送删除所有用户指令
		result = MG_Rev_Pack(0x54,&res,&param);
	}while(result);
//	MG_Send_Pack(0x54,0x00);
//	if(MG_Rev_Pack(0x54,&res,&param))
//	{
//		printf("解析出错!\r\n");
//		MG200_PWR_OFF;//关闭省电
//		return 1;
//	}
	switch(res)
	{
		case 0x00:	printf("删除全部用户信息成功\r\n");
					Voice_Work(0X11);//删除全部用户信息成功
					printf("删除全部用户信息成功");
					LCD_Clear(0,239,0,239,WHITE);//清屏
					LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"删除全部用户信息成功");
					Delay_Ms(500);
					break;
		case 0x90:	printf("删除失败 (注册的用户数为 0 的时候)\r\n");
					LCD_Clear(0,239,0,239,WHITE);//清屏
					LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"删除失败 (注册的用户数为 0 的时候)");
					Delay_Ms(500);
					break;
		default:	printf("删除失败\r\n");
					LCD_Clear(0,239,0,239,WHITE);//清屏
					LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"删除失败");
					Delay_Ms(500);
				
	}
	MG200_PWR_OFF;//关闭省电
	return 0;
	
}

/*
Function name:Match1N
Description:MG_200对比所有用户
param:None
retval:状态码(0正确 非零错误)
Remarks:指令码--->0x71
*/
u8 Match1N(void)
{
	u8 res,param,result = 0;
	u8 rev_buf[50]={0};
	MG200_PWR_ON;
	Delay_Ms(30);//等待上电
	printf("第一次放下手指\r\n");
	TIM9->CNT =0;
	TIM_Cmd(TIM9,ENABLE);//打开计数器
	while(!MG200_DETECT)
	{
		wait_flag2=1;
		if(wait_flag1)
		{
			wait_flag1=0;
			wait_flag2=0;
			MG200_PWR_OFF;//关闭省电
			return 0;
		}
	}//等待手指按下
	
	do{
		result = CaptureAndExtract(0x00);//第一次提取
	}while(result);//直到提取成功才出循环
	
	LCD_Clear(0,239,0,239,WHITE);//清屏
	LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"请松开手指");
	printf("请松开手指\r\n");
	while(MG200_DETECT);//等待手指松开
	
	do{
		MG_Send_Pack(0x71,0x00);//发送比对指令
		result = MG_Rev_Pack(0x71,&res,&param);//解析接收数据包结果
	}while(result);//直到解析成功才出循环
	
	switch(res)
	{
		case 0x00:	printf("比对成功\r\n");	
					Voice_Work(0X18);//欢迎回家
					LCD_Clear(0,239,0,239,WHITE);//清屏
					sprintf((char *)rev_buf,"用户ID为：%d",param);
					LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)rev_buf);
					LCDFONT_Display_Str(70,0,BLUE,WHITE,16,(u8 *)"比对成功");
					Delay_Ms(500);
					Open_Door();
					page_flag=1;
					break;
		default:	printf("比对失败\r\n");			
					Voice_Work(0X19);//开门失败
					LCD_Clear(0,239,0,239,WHITE);//清屏
					LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"对比失败！");
					Delay_Ms(500);
	}
	if(param)
	{
		printf("用户ID号是：%d\r\n",param);
	}
	MG200_PWR_OFF;//关闭省电	
	
	return 0;
}


void MG_Open_lock(void)
{
	LCD_Clear(0,239,0,239,WHITE);//清屏
	LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"请放下手指：");
	printf("请放下手指：\r\n");
	Voice_Work(0X16);//请放手指
	Match1N();//指纹对比函数
	lcd_flag = 1;	
}

void MG_Enroll(void)
{
	Voice_Work(0X03);//登记指纹
	LCD_Clear(0,239,0,239,WHITE);//清屏
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"登记指纹:");
	Delay_Ms(500);
	Enroll(0x00);
	lcd_flag=1;
	
	
}

void MG_Delect_All(void)
{
	Voice_Work(0X06);//删除所有指纹
	printf("删除所有指纹");
	LCD_Clear(0,239,0,239,WHITE);//清屏
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"删除所有指纹");
	Delay_Ms(500);
	EraseAll();
	
	lcd_flag=1;
}




u8 MG_Del_By_Id(void)
{
	u8 res,param,result = 0;
	LCD_Clear(0,239,0,239,WHITE);//清屏
	LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"请输入要删除的ID");
	while(1)
	{
		if(!BS_IRQ)
		{
			key_v=Get_BsKey_Value();
			while(!BS_IRQ);
			printf("key_v1=%c\r\n",key_v);//打印键值并判断位置
			if(key_v)
			{
				MG200_PWR_ON;
				Delay_Ms(30);//等待上电
				do{
					MG_Send_Pack(0x73,key_v-48);//发送删除指定用户指令
					result = MG_Rev_Pack(0x73,&res,&param);
				}while(result);
				switch(res)
				{
					case 0x00:	printf("删除成功\r\n");
								LCD_Clear(0,239,0,239,WHITE);//清屏
								LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"删除成功");
								break;
					case 0x83:	printf("参数错误(ID≤0 或者 ID > 最大用户数)\r\n");
								LCD_Clear(0,239,0,239,WHITE);//清屏
								LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"参数错误(ID≤0 或者 ID > 最大用户数)");
								break;
					case 0x90:	printf("未注册的用户\r\n");
								LCD_Clear(0,239,0,239,WHITE);//清屏
								LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"未注册的用户");
								break;
					case 0xff:	printf("写入 ROM 错误\r\n");
								LCD_Clear(0,239,0,239,WHITE);//清屏
								LCDFONT_Display_Str(50,0,BLUE,WHITE,16,(u8 *)"写入 ROM 错误");
								break;	
				}
				Delay_Ms(500);
				MG200_PWR_OFF;//关闭省电
				lcd_flag=1;
				return 0;
			}
		}
		
	}
	
}
