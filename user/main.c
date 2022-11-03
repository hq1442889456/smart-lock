#include "main.h"
u8 send_buf[15]={'1','2','3','4','5','6','\0'};//����
u8 card_id[4];  //��ǰIC����  �ҵ�IC������:0xAC AD 84 21  ����EEPROM�У��������������
unsigned char CT[2];//������
u8 key_v;//���ݰ�����ֵ
u8 errorCode;//mqtt������
char string[520] = {0};//��Ҫ�����ϴ����ݵ�����
u8 rev_buf[15]={0};//�洢�����������RFID��IC����
u8 key_buf[15]={0};//�洢���������ַ���

RTC_TimeTypeDef RTC_Time;//ʱ��ṹ�����
RTC_DateTypeDef RTC_Date;//���ڽṹ�����
u8 display_buf[100]={0};//�洢��ʾ��LCD�����ַ���������
u8 passward[15]={0};//���ڴ���޸ĺ�ĵ�һ�����������
u8 passward1[15]={0};//���ڴ���޸ĺ�ĵڶ�����������
u8 password_flag;//�޸ĵ��������벻һ�»���һ
u8 volume = 0x27;//��ʼ������С

u8 lcd_flag;//������־λ
u8 gif_flag;

int main(void)
{
	Led_Config();//LED��
	Usart_Config(115200);//����
	BS8116_Config();//���ݰ���
	AT24C04_Config();//EEPROM�洢оƬ
	Voice_Config();//����ģ��
	Voice_Work(volume);//��������
	Motor_Config();//���ģ��
	MG200_Init();//ָ��
	RFID_Init();//RFID
	SPI1_Config();//SPI��������ʼ��  ���ں�W25Q64����ͨ��
	LCD_Init();//LCD��Ļ
	RTC_Clender_Config();//RTC����
//	TIM9_MsIT_Config();//1MS��ʱ�ж�
	Esp_Init();//esp32��ʼ��
	printf("��ʼ���ɹ�!\r\n");
	
	AT_Cross_Write(62,send_buf,sizeof(send_buf));//��EEPROMд���ʼ����
	AT_Random_Read(62,rev_buf,sizeof(rev_buf));//��EEPROM�����������rev_buf[]������
	printf("%s\n",rev_buf);

	ESP32_Subscription();//esp32����WIFI�����İ�����
	ESP32_Publish(0);//esp32�ϱ�����������״̬����
mm:	
	while(1)
	{
		if(lcd_flag)
		{
			LCD_Clear(0,239,0,239,WHITE);//����
			lcd_flag =0;
		}
		LCD_Dis_GUI();
		if(!BS_IRQ)
		{
			key_v=Get_BsKey_Value();
			while(!BS_IRQ);
			printf("key_v1=%c\r\n",key_v);//��ӡ��ֵ���ж�λ��
			Voice_Work(0X1d);//�   Ҫ���ڻ�ȡ��ֵ֮���������IICʱ��
			switch(key_v)//���ݼ�ֵѡ������ʽ
			{
				case '1':BS_Open_Lock();break;
				case '2':MG_Open_lock();break;
				case '3':RFID_Open_lock();break;
			}
		}
		if(esp32rec.flag == 1)//�ƿ���
		{
			ESP32_Open_Lock();
		}
		while(page_flag==1)
		{
			if(lcd_flag)
			{
				LCD_Clear(0,239,0,239,WHITE);//����
				lcd_flag =0;
			}
			//RTC������ѡ����Ļ��ʾ
			LCD_Dis_RTC();
			if(!BS_IRQ)
			{
				key_v=Get_BsKey_Value();
				printf("key_v2=%c\r\n",key_v);//��ӡ��ֵ���ж�λ��
				Voice_Work(0X1d);//�   Ҫ���ڻ�ȡ��ֵ֮���������IICʱ��
				switch(key_v)//���ݼ�ֵѡ������ʽ
				{
					case '1':BS_Change_Password();break;
					case '2':MG_Enroll();break;
					case '3':MG_Delect_All();break;
					case '4':VOICE_Set_Volume();break;
					case '5':MG_Del_By_Id();break;
					case '0':page_flag=0;lcd_flag=1;esp32rec.flag = 0;goto mm;
				}
			}
			if(esp32rec.flag == 1)//�ƹ���
			{
				ESP32_Close_Lock();
				lcd_flag=1;
			}
		}
	}
}



