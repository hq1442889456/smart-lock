#include "main.h"

/*
Function name:Font_Dma_Config
Description:DMA搬运串口RX数据
param:None
retval:None
Remarks:
USART1->DR(RX) ----> DMA2数据流5通道4   擦除完成之后，然后才在上位机点击发送文件，RX才开始有数据接收，DMA才开始搬运工作，所以不用担心在擦除过程中会进DMA空闲中断
*/
REV rev={0};
void Font_Dma_Config(void)
{
	//定义变量
	DMA_InitTypeDef dma_InitTypeDef={0};
	NVIC_InitTypeDef nvic_InitTypeDef={0};
	//时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//打开DMA2时钟
	
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);//USART TX使能
	DMA_Cmd(DMA2_Stream5,DISABLE);//禁止使能DMA
	
	//结构体初始化配置
	dma_InitTypeDef.DMA_BufferSize=DMABUFFSIZE;//数据项数目 4096个字节
	dma_InitTypeDef.DMA_Channel=DMA_Channel_4;//通道
	dma_InitTypeDef.DMA_DIR=DMA_DIR_PeripheralToMemory;//数据方向
	dma_InitTypeDef.DMA_FIFOMode=DMA_FIFOMode_Disable;//禁止FIFO 直接模式
	dma_InitTypeDef.DMA_Memory0BaseAddr=(u32)rev.dma_buff[0];//存储器地址,第一个数组先接收
//	dma_InitTypeDef.DMA_MemoryBurst=DMA_MemoryBurst_Single;//单次传输
	dma_InitTypeDef.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;//存储器数据宽度
	dma_InitTypeDef.DMA_MemoryInc=DMA_MemoryInc_Enable;//存储器地址递增
	dma_InitTypeDef.DMA_Mode=DMA_Mode_Circular;//使能循环 循环模式可用于处理循环缓冲区和连续数据流（例如 ADC 扫描模式）
	dma_InitTypeDef.DMA_PeripheralBaseAddr=(u32)&USART1->DR;//外设地址
//	dma_InitTypeDef.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;//单次传输
	dma_InitTypeDef.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;//外设数据宽度
	dma_InitTypeDef.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//外设地址不递增
//	dma_InitTypeDef.DMA_Priority=DMA_Priority_Medium;//优先级  只有一个数据流，可以不配
	DMA_Init(DMA2_Stream5,&dma_InitTypeDef);
	
	//双缓冲区
	DMA_DoubleBufferModeConfig(DMA2_Stream5,(u32)rev.dma_buff[1],DMA_Memory_0);//配置第一个搬运接收数组是dma_buff[0]，第二个搬运接收数组是dma_buff[1]
	DMA_DoubleBufferModeCmd(DMA2_Stream5,ENABLE);
	
	//使能空闲中断 
	DMA_ITConfig(DMA2_Stream5,DMA_IT_TC,ENABLE);
	
	//优先级
	nvic_InitTypeDef.NVIC_IRQChannel=DMA2_Stream5_IRQn;//中断号
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//分组 第五组
	nvic_InitTypeDef.NVIC_IRQChannelPreemptionPriority=0;//占先
	nvic_InitTypeDef.NVIC_IRQChannelSubPriority=1;//次级
	nvic_InitTypeDef.NVIC_IRQChannelCmd=ENABLE;//使能
	NVIC_Init(&nvic_InitTypeDef);
	
	DMA_Cmd(DMA2_Stream5,ENABLE);//使能DMA
}

//DMA空闲中断   当完成4096字节的搬运后才会进入该中断，然后最后一次搬运的字节数小于4096，因为1.82MB/4096B==1.82*1024*1024B/4096B=465.92,所以还有0.92*4096个数据没传，所以触发不了该中断，需要去串口空闲中断去传完剩余的数据
void DMA2_Stream5_IRQHandler(void)
{
	DMA_ClearITPendingBit(DMA2_Stream5,DMA_IT_TCIF5);//清除中断标志位
	rev.len=DMABUFFSIZE;
	rev.flag=DMAIT;
}


/*
Function name:Font_Load
Description:字库下载到W25Qxx中
param:None
retval:None
Remarks:1.82M/65536=29.12 需要30块大小空间写入
*/
void Font_Load(void)
{
	u8 i=0;
	u16 cnt=0;
	printf("正在擦除...\r\n");
	for(i=0;i<30;i++)//在写入之前必须进行擦除
	{
		W25Q_Block_Erase(i*65536);
	}
	printf("擦除完成\r\n");
	while(1)
	{
		switch(rev.flag)
		{
			case FREE:break;
			case DMAIT:
				rev.flag=FREE;
				W25Q_Cross_Write(rev.addr,(u8 *)rev.dma_buff[rev.buff_flag],rev.len);//跨页写 一页才16字节，每次写4096字节，故需要跨页写
			  rev.addr+=rev.len;//写入地址做偏移
			  rev.buff_flag=!rev.buff_flag;//数组下标标志位做切换
			  printf("cnt=%d\r\n",cnt++);//正常cnt=4-=65/466
				break;
			case USARTIT:
				W25Q_Cross_Write(rev.addr,(u8 *)rev.dma_buff[rev.buff_flag],rev.len);//写入最后一帧
				printf("传输完成\r\n");
				return;
		}
	}
}






