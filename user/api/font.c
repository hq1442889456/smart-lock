#include "main.h"

/*
Function name:Font_Dma_Config
Description:DMA���˴���RX����
param:None
retval:None
Remarks:
USART1->DR(RX) ----> DMA2������5ͨ��4   �������֮��Ȼ�������λ����������ļ���RX�ſ�ʼ�����ݽ��գ�DMA�ſ�ʼ���˹��������Բ��õ����ڲ��������л��DMA�����ж�
*/
REV rev={0};
void Font_Dma_Config(void)
{
	//�������
	DMA_InitTypeDef dma_InitTypeDef={0};
	NVIC_InitTypeDef nvic_InitTypeDef={0};
	//ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//��DMA2ʱ��
	
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);//USART TXʹ��
	DMA_Cmd(DMA2_Stream5,DISABLE);//��ֹʹ��DMA
	
	//�ṹ���ʼ������
	dma_InitTypeDef.DMA_BufferSize=DMABUFFSIZE;//��������Ŀ 4096���ֽ�
	dma_InitTypeDef.DMA_Channel=DMA_Channel_4;//ͨ��
	dma_InitTypeDef.DMA_DIR=DMA_DIR_PeripheralToMemory;//���ݷ���
	dma_InitTypeDef.DMA_FIFOMode=DMA_FIFOMode_Disable;//��ֹFIFO ֱ��ģʽ
	dma_InitTypeDef.DMA_Memory0BaseAddr=(u32)rev.dma_buff[0];//�洢����ַ,��һ�������Ƚ���
//	dma_InitTypeDef.DMA_MemoryBurst=DMA_MemoryBurst_Single;//���δ���
	dma_InitTypeDef.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;//�洢�����ݿ��
	dma_InitTypeDef.DMA_MemoryInc=DMA_MemoryInc_Enable;//�洢����ַ����
	dma_InitTypeDef.DMA_Mode=DMA_Mode_Circular;//ʹ��ѭ�� ѭ��ģʽ�����ڴ���ѭ�������������������������� ADC ɨ��ģʽ��
	dma_InitTypeDef.DMA_PeripheralBaseAddr=(u32)&USART1->DR;//�����ַ
//	dma_InitTypeDef.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;//���δ���
	dma_InitTypeDef.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;//�������ݿ��
	dma_InitTypeDef.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//�����ַ������
//	dma_InitTypeDef.DMA_Priority=DMA_Priority_Medium;//���ȼ�  ֻ��һ�������������Բ���
	DMA_Init(DMA2_Stream5,&dma_InitTypeDef);
	
	//˫������
	DMA_DoubleBufferModeConfig(DMA2_Stream5,(u32)rev.dma_buff[1],DMA_Memory_0);//���õ�һ�����˽���������dma_buff[0]���ڶ������˽���������dma_buff[1]
	DMA_DoubleBufferModeCmd(DMA2_Stream5,ENABLE);
	
	//ʹ�ܿ����ж� 
	DMA_ITConfig(DMA2_Stream5,DMA_IT_TC,ENABLE);
	
	//���ȼ�
	nvic_InitTypeDef.NVIC_IRQChannel=DMA2_Stream5_IRQn;//�жϺ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//���� ������
	nvic_InitTypeDef.NVIC_IRQChannelPreemptionPriority=0;//ռ��
	nvic_InitTypeDef.NVIC_IRQChannelSubPriority=1;//�μ�
	nvic_InitTypeDef.NVIC_IRQChannelCmd=ENABLE;//ʹ��
	NVIC_Init(&nvic_InitTypeDef);
	
	DMA_Cmd(DMA2_Stream5,ENABLE);//ʹ��DMA
}

//DMA�����ж�   �����4096�ֽڵİ��˺�Ż������жϣ�Ȼ�����һ�ΰ��˵��ֽ���С��4096����Ϊ1.82MB/4096B==1.82*1024*1024B/4096B=465.92,���Ի���0.92*4096������û�������Դ������˸��жϣ���Ҫȥ���ڿ����ж�ȥ����ʣ�������
void DMA2_Stream5_IRQHandler(void)
{
	DMA_ClearITPendingBit(DMA2_Stream5,DMA_IT_TCIF5);//����жϱ�־λ
	rev.len=DMABUFFSIZE;
	rev.flag=DMAIT;
}


/*
Function name:Font_Load
Description:�ֿ����ص�W25Qxx��
param:None
retval:None
Remarks:1.82M/65536=29.12 ��Ҫ30���С�ռ�д��
*/
void Font_Load(void)
{
	u8 i=0;
	u16 cnt=0;
	printf("���ڲ���...\r\n");
	for(i=0;i<30;i++)//��д��֮ǰ������в���
	{
		W25Q_Block_Erase(i*65536);
	}
	printf("�������\r\n");
	while(1)
	{
		switch(rev.flag)
		{
			case FREE:break;
			case DMAIT:
				rev.flag=FREE;
				W25Q_Cross_Write(rev.addr,(u8 *)rev.dma_buff[rev.buff_flag],rev.len);//��ҳд һҳ��16�ֽڣ�ÿ��д4096�ֽڣ�����Ҫ��ҳд
			  rev.addr+=rev.len;//д���ַ��ƫ��
			  rev.buff_flag=!rev.buff_flag;//�����±��־λ���л�
			  printf("cnt=%d\r\n",cnt++);//����cnt=4-=65/466
				break;
			case USARTIT:
				W25Q_Cross_Write(rev.addr,(u8 *)rev.dma_buff[rev.buff_flag],rev.len);//д�����һ֡
				printf("�������\r\n");
				return;
		}
	}
}






