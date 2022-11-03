#include "rfid.h"

u8 CardId[4];  				//卡号
u8 CardType[2];//卡类型
u8 writeDataBuf[16]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0,1,2,3,4,0xff,0xff,0xff}; //写入的数据
u8 RFKEY[6]={0xff,0xff,0xff,0xff,0xff,0xff}; //卡密码-初始密码--白卡的出厂密码--
/********************
PA15--CS--推挽输出
PC10--SCL--推挽输出
PC11--MISO--浮空输入
PC12--MOSI--推挽输出
PD2--RST--推挽输出
********************/
void Spi3_Init(void)
{
	//GPIO初始化
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC |RCC_AHB1Periph_GPIOD, ENABLE);//1.开时钟
	
	//PC10--SCL--推挽输出,PC12--MOSI--推挽输出
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	//PA15--CS--推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//PD2--RST--推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	//PC11--MISO--浮空输入
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_SetBits(GPIOD, GPIO_Pin_2);
	GPIO_SetBits(GPIOA,GPIO_Pin_15);

}

/*
**********************************************************************
函数功能：spi数据交换的函数(主机发送一个字节数据给从机，并且读取从机发送过来的一个字节数据)
函数形参：主机要发送给从机的数据
函数返回值：主机读到从机数据
备注：数据位8bit  先高后低的顺序
**********************************************************************
*/
u8 SPI3TransferByte(u8 data)
{
	u8 i,buf = 0;
	SPI3_SCL_H;
	for(i = 0; i < 8; i++)
	{
		SPI3_SCL_L;
		if(data & (0x80 >> i))
		{
			RIFD_MOSI_H;
		}
		else
		{
			RIFD_MOSI_L;
		}
		SPI3_SCL_H;
		buf <<= 1;
		if(RIFD_MISO)
			buf |= 1;
	}
	return buf;
}

//读RC632寄存器
//Address:寄存器地址
u8 ReadRawRC(u8 Address)
{
	u8 ucAddr;
	u8 ucResult=0;
	RIFD_CS_L;
	ucAddr = ((Address<<1)&0x7E)|0x80;

	SPI3TransferByte(ucAddr);
	ucResult=SPI3TransferByte(0);
	RIFD_CS_H;
	return ucResult;
}


//写RC632寄存器
//Address[IN]:寄存器地址
//value[IN]:写入的值
void WriteRawRC(uint8_t Address, uint8_t value)
{  
    uint8_t ucAddr;

	RIFD_CS_L;
    ucAddr = ((Address<<1)&0x7E);//0111 1110

	SPI3TransferByte(ucAddr);
	SPI3TransferByte(value);
	RIFD_CS_H;
}

//清寄存器位
void ClearBitMask(u8 reg,u8 mask)// 0000 1000
{
    char   tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
} 

//置RC522寄存器位
//reg[IN]:寄存器地址
//mask[IN]:置位值
void SetBitMask(u8 reg,u8 mask)  
{
    char   tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}


//复位RC522
char PcdReset(void)
{
	RIFD_RES_H;
	Delay_Us(2);
	
	RIFD_RES_L;
	Delay_Us(2);
	
	RIFD_RES_H;
	Delay_Us(2);
	WriteRawRC(CommandReg,PCD_RESETPHASE);
	WriteRawRC(CommandReg,PCD_RESETPHASE);
	Delay_Us(2);
    
	WriteRawRC(ModeReg,0x3D);            //和Mifare卡通讯，CRC初始值0x6363
	WriteRawRC(TReloadRegL,30);           
	WriteRawRC(TReloadRegH,0);
	WriteRawRC(TModeReg,0x8D);
	WriteRawRC(TPrescalerReg,0x3E);
	
	WriteRawRC(TxAutoReg,0x40);//必须要
   
   return MI_OK;
	
}

//关闭天线
void PcdAntennaOff(void)
{
	ClearBitMask(TxControlReg, 0x03);
}

//开启天线
void PcdAntennaOn(void)
{
    u8   i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}


void Reset_RC522(void)
{
	PcdReset();       //功    能：复位RC522
	PcdAntennaOff();  //关闭天线
	PcdAntennaOn();   //开启天线
}    

//设置RC522工作模式
char M500PcdConfigISOType(u8 type)
{
   if (type == 'A')                     //ISO14443_A
   { 
			ClearBitMask(Status2Reg,0x08);
			WriteRawRC(ModeReg,0x3D);//3F
			WriteRawRC(RxSelReg,0x86);//84
			WriteRawRC(RFCfgReg,0x7F);   //4F
			WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
			WriteRawRC(TReloadRegH,0);
			WriteRawRC(TModeReg,0x8D);
			WriteRawRC(TPrescalerReg,0x3E);
			Delay_Us(2);
			PcdAntennaOn();
   }
   else{ return 1; }
   
   return MI_OK;
}

void RFID_Init(void)
{
	Spi3_Init();
	Reset_RC522();
	M500PcdConfigISOType('A');
}

/*******************************************************************
* 函数名：     PcdComMF522()
* 功能描述 ：  通过RC522和ISO14443卡通讯
* 作者：         谢国卿 
* 参数说明:   Command[IN]:RC522命令字
							pIn [IN]:通过RC522发送到卡片的数据
							InLenByte[IN]:发送数据的字节长度
							pOut [OUT]:接收到的卡片返回数据
							*pOutLenBit[OUT]:返回数据的位长度
* 返回值说明：
* 修改记录：
* 其他：
*******************************************************************/
char PcdComMF522(u8   Command, 
                 u8 *pIn , 
                 u8   InLenByte,
                 u8 *pOut , 
                 u8 *pOutLenBit)
{
    char   status = MI_ERR;
    u8   irqEn   = 0x00;
    u8   waitFor = 0x00;
    u8   lastBits;
    u8   n;
    u16   i;
    switch (Command)
    {
        case PCD_AUTHENT:
													irqEn   = 0x12;
													waitFor = 0x10;
													break;
				case PCD_TRANSCEIVE:
													irqEn   = 0x77;
													waitFor = 0x30;
													break;
				default:	break;
    }
   
    WriteRawRC(ComIEnReg,irqEn|0x80);
    ClearBitMask(ComIrqReg,0x80);	//清所有中断位
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);	 	//清FIFO缓存
    
    for (i=0; i<InLenByte; i++)
    {   WriteRawRC(FIFODataReg, pIn [i]);    }
    WriteRawRC(CommandReg, Command);	  
//   	 n = ReadRawRC(CommandReg);
    
    if (Command == PCD_TRANSCEIVE)
    {    SetBitMask(BitFramingReg,0x80);  }	 //开始传送
    										 
      i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
// 	  i = 100000;
    do 
    {
        n = ReadRawRC(ComIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));
    ClearBitMask(BitFramingReg,0x80);

    if (i!=0)
    {    
        if(!(ReadRawRC(ErrorReg)&0x1B))
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {   status = MI_NOTAGERR;   }
            if (Command == PCD_TRANSCEIVE)
            {
               	n = ReadRawRC(FIFOLevelReg);
              	lastBits = ReadRawRC(ControlReg) & 0x07;
                if (lastBits)
                {   *pOutLenBit = (n-1)*8 + lastBits;   }
                else
                {   *pOutLenBit = n*8;   }
                if (n == 0)
                {   n = 1;    }
                if (n > MAXRLEN)
                {   n = MAXRLEN;   }
                for (i=0; i<n; i++)
                {   pOut [i] = ReadRawRC(FIFODataReg);    }
            }
        }
        else
        {   status = MI_ERR;   }
        
    }
   

    SetBitMask(ControlReg,0x80);           // stop timer now
    WriteRawRC(CommandReg,PCD_IDLE); 
    return status;
}

/*******************************************************************
* 函数名：     PcdRequest()
* 功能描述 ：   	寻卡
* 作者：         谢国卿 
*参数说明: req_code[IN]:寻卡方式
                0x52 = 寻感应区内所有符合14443A标准的卡
                0x26 = 寻未进入休眠状态的卡
           pTagType[OUT]：卡片类型代码
                0x4400 = Mifare_UltraLight
                0x0400 = Mifare_One(S50)
                0x0200 = Mifare_One(S70)
                0x0800 = Mifare_Pro(X)
                0x4403 = Mifare_DESFire
* 返回值说明：成功返回MI_OK
* 修改记录：
* 其他：
*******************************************************************/               
char PcdRequest(u8 req_code,u8 *pTagType)
{
	char   status;  
	u8   unLen;
	u8   ucComMF522Buf[MAXRLEN]; 

	ClearBitMask(Status2Reg,0x08);
	WriteRawRC(BitFramingReg,0x07);
	SetBitMask(TxControlReg,0x03);
 
	ucComMF522Buf[0] = req_code;

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);

	if ((status == MI_OK) && (unLen == 0x10))
	{    
		*pTagType     = ucComMF522Buf[0];
		*(pTagType+1) = ucComMF522Buf[1];
	}
	else
	{   status = MI_ERR;   }
   
	return status;
}


/*******************************************************************
* 函数名：     PcdAnticoll()
* 功能描述 ：   防冲撞
* 作者：         谢国卿 
* 参数说明：     pSnr[OUT]:卡片序列号，4字节
* 返回值说明：   成功返回MI_OK
* 修改记录：
* 其他：
*******************************************************************/
char PcdAnticoll(u8 *pSnr)
{
    char   status;
    u8   i,snr_check=0;
    u8   unLen;
    u8   ucComMF522Buf[MAXRLEN]; 
    

    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
    }
    
    SetBitMask(CollReg,0x80);
    return status;
}

/*******************************************************************
* 函数名：     CalulateCRC()
* 功能描述 ：  用MF522计算CRC16函数
* 作者：         谢国卿 
*参数说明: 
* 返回值说明： 
* 修改记录：
* 其他：
*******************************************************************/
void CalulateCRC(u8 *pIn ,u8   len,u8 *pOut )
{
    u8   i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {   WriteRawRC(FIFODataReg, *(pIn +i));   }
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOut [0] = ReadRawRC(CRCResultRegL);
    pOut [1] = ReadRawRC(CRCResultRegM);
}

/*******************************************************************
* 函数名：     PcdSelect()
* 功能描述 ：   选定卡片
* 作者：         谢国卿 
* 参数说明：     pSnr[IN]:卡片序列号，4字节
* 返回值说明：   成功返回MI_OK
* 修改记录：
* 其他：
*******************************************************************/
char PcdSelect(u8 *pSnr)
{
    char   status;
    u8   i;
    u8   unLen;
    u8   ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
    ClearBitMask(Status2Reg,0x08);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
    
    if ((status == MI_OK) && (unLen == 0x18))
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}


void print_info(unsigned char *p,int cnt)
{
  int i;
  for(i=0;i<cnt;i++)
  {
		if(!i)
			printf("0x%02X ",p[i]);
		else
			printf("%02X ",p[i]);
  }
  printf("\r\n");
}

/**********************************************
**函数功能：寻卡--防冲撞--选卡

**参数说明: 
pTagType[OUT]：卡片类型代码(做输出参数)
**                0x4400 = Mifare_UltraLight
**                0x0400 = Mifare_One(S50)
**                0x0200 = Mifare_One(S70)
**                0x0800 = Mifare_Pro(X)
**                0x4403 = Mifare_DESFire
pSnr[OUT]:卡片序列号，4字节(做输出参数)

**返回值：成功返回MI_OK
**作者：weihao Mo
**********************************************/
u8 Identificate_Card(u8 *pTagType,u8 *pSnr)
{
  u8 status=MI_ERR;
	u8 STA=0;

   if(PcdRequest(PICC_REQALL,pTagType)==MI_OK)//寻卡成功
   {
      STA |=0x01;
      if(PcdAnticoll(pSnr)==MI_OK)/*防冲撞成功*/
      {
				STA |=0x02;
		
        if(PcdSelect(pSnr)==MI_OK)//选卡成功
        {
					STA |=0x04;
        }
      }
   }
	 
	 
	 if(STA==0x07)
		 status=MI_OK;
	 
  return status;
}

/*******************************************************************
* 函数名：     PcdAuthState()
* 功能描述 ：   验证卡片密码
* 作者：         
*参数说明: auth_mode[IN]: 密码验证模式
                 0x60 = 验证A密钥
                 0x61 = 验证B密钥 
								addr[IN]：块地址
								pKey[IN]：密码
								pSnr[IN]：卡片序列号，4字节
* 返回值说明：   成功返回MI_OK
* 修改记录：
* 其他：
*******************************************************************/             
char PcdAuthState(u8  auth_mode,u8 addr,u8 *pKey,u8 *pSnr)
{
    char   status;
    u8   unLen;
		u8	 ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    memcpy(&ucComMF522Buf[2], pKey, 6); 
    memcpy(&ucComMF522Buf[8], pSnr, 4); 
    
    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {   status = MI_ERR;   }
    
    return status;
}

/*******************************************************************
* 函数名：     PcdWrite()
* 功能描述 ：   写数据到M1卡一块
* 作者：          
*参数说明: addr[IN]：块地址
           p [OUT]：读出的数据，16字节
* 返回值说明：   成功返回MI_OK
* 修改记录：
* 其他：
*******************************************************************/                  
char PcdWrite(u8   addr,u8 *p )
{
    char   status;
    u8   unLen;
    u8   i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, p , 16);
        for (i=0; i<16; i++)
        {    
        	ucComMF522Buf[i] = *(p +i);   
        }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    
    return status;
}


/*
功能：写数据到指定块
参数：
u8   auth_mode ：验证密码的类型。（分为PICC_AUTHENT1A和PICC_AUTHENT1B）
u8   addr      ：数据存放的地址。每个扇区的0、1、2块是存放数据。3是存放密码。
一般填：0、1、2 、4、5、6、8、9、10----
u8 *Src_Key    ：原密码 
u8 *data       ：新写入的数据
u8 *pSnr       ：卡号

数据一般格式：u8 writeDataBuf[16]={255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255}; //写入的金额；

*/
u8 WriteCardData(u8 auth_mode,u8 addr,u8 *Src_Key,u8 *data,u8 *pSnr)
{
  int status=MI_ERR;

  /*1. 寻卡--防冲撞--选卡*/
  status=Identificate_Card(CardType,pSnr); 
  /*2. 验证卡密码*/
  if(status==MI_OK)
  {
    status =PcdAuthState(0x60,addr/4*4+3,Src_Key,pSnr);   //验证卡片密码 形参参数：验证方式，块地址，密码，卡许列号
  }
  if(status==MI_OK)
  {
    printf("密码验证成功!\r\n");
  }
  /*3. 写数据到卡*/
  if(status==MI_OK)
  {
    status=PcdWrite(addr,data); //写数据到第addr块，data入的数据值。
  }
  if(status==MI_OK)
  {
    printf("数据写入OK!\r\n");
  }
  return status;
}

/*******************************************************************
* 函数名：     PcdRead()
* 功能描述 ：   读取M1卡一块数据
* 作者：         谢国卿 
*参数说明: addr[IN]：块地址
           p [OUT]：读出的数据，16字节
* 返回值说明：   成功返回MI_OK
* 修改记录：
* 其他：
*******************************************************************/ 
char PcdRead(u8   addr,u8 *p )
{
    char   status;
    u8   unLen;
    u8   i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
   
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
 //   {   memcpy(p , ucComMF522Buf, 16);   }
    {
        for (i=0; i<16; i++)
        {    *(p +i) = ucComMF522Buf[i];   }
    }
    else
    {   status = MI_ERR;   }
    
    return status;
}

/*
功能：读数据到指定块
参数：
u8   auth_mode ：验证密码的类型。（分为PICC_AUTHENT1A和PICC_AUTHENT1B）
u8   addr      ：数据存放的地址。每个扇区的0、1、2块是存放数据。3是存放密码。
一般填：0、1、2 、4、5、6、8、9、10
u8 *Src_Key    ：原密码 
u8 *data       ：读出的数据
u8 *pSnr       ：卡号

数据一般格式：u8 writeDataBuf[16]={255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255}; //写入的金额；

*/
u8 ReadCardData(u8 auth_mode,u8 addr,u8 *Src_Key,u8 *data,u8 *pSnr)
{
  int status;
	
  /*1. 寻卡--防冲撞--选卡*/
  status=Identificate_Card(CardType,pSnr);  
  /*2. 验证卡密码*/
  if(status==MI_OK)
  {
		printf("卡类型:");
		print_info(CardType,2);//打印类型
		printf("卡号:");
		print_info(pSnr,4);//打印卡号		
		
    status=PcdAuthState(auth_mode,addr/4*4+3,Src_Key,pSnr);   //验证卡片密码 形参参数：验证方式，块地址，密码，卡许列号
    printf("验证卡密码成功\r\n");
  }
  /*3. 读出数据*/
  if(status==MI_OK)
  {
    status=PcdRead(addr,data); //从第addr块读出数据值。
  }
  if(status==MI_OK)
  {
    printf("数据读出成功!\r\n");
  }
  return status;
}



void RFID_Open_lock(void)
{
	LCD_Clear(0,239,0,239,WHITE);//清屏
	LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"请放下卡片：");
	Voice_Work(0X13);//请放置卡片
	printf("请放下卡片：\r\n");
	TIM9->CNT =0;
	TIM_Cmd(TIM9,ENABLE);//打开计数器
	while(1)
	{
		//卡识别
		if(PcdRequest(PICC_REQALL,CT)==MI_OK)//寻卡成功
		{
			if(PcdAnticoll(card_id)==MI_OK)/*防冲撞成功*/
			{
//							printf("卡类型:");
//									print_info(CT,2);//打印类型
//								
//									printf("卡号:");
//									print_info(card_id,4);//打印卡号	
				if(PcdSelect(card_id)==MI_OK)//选卡成功
				{
//								printf("卡片获取成功\r\n");
//								Delay_Ms(1000);
				}
//					AT_Page_Write(0,card_id,sizeof(card_id));//向AT24C02写入卡号
				AT_Random_Read(0,rev_buf,sizeof(rev_buf));//向AT24C02读取卡号
//					printf("%#x %#x %#x %#x",rev_buf[0],rev_buf[1],rev_buf[2],rev_buf[3]);
				if(rev_buf[0]==card_id[0]&&rev_buf[1]==card_id[1]&&rev_buf[2]==card_id[2]&&rev_buf[3]==card_id[3])
				{
					printf("开门成功！");
					Voice_Work(0X18);//欢迎回家
					LCD_Clear(0,239,0,239,WHITE);//清屏
					LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"欢迎回家！");
					Delay_Ms(500);
					Open_Door();
					page_flag=1;
					TIM9->CNT =0;
					TIM_Cmd(TIM9,DISABLE);//关闭计数器
					
					break;
				}
				else
				{
					printf("验证失败！");
					Voice_Work(0X0f);//验证失败！
					LCD_Clear(0,239,0,239,WHITE);//清屏
					LCDFONT_Display_Str(50,0,BLUE,WHITE,32,(u8 *)"验证失败！");
					Delay_Ms(500);
					break;
				}
			
			}
		}
		wait_flag2=1;
		if(wait_flag1)
		{
			wait_flag1=0;
			wait_flag2=0;
			lcd_flag = 1;
			return;
		}
	}
	lcd_flag = 1;
}
