#ifndef __BITBAND_H_
#define __BITBAND_H_

//����������ĵ�ַ��
//addr -- �洢���ĵ�ַ    bit --Ҫ������λ
#define BITBAND(addr,bit) (((addr&0xF0000000)+0x2000000)+ ((addr&0xFFFFF)*8+bit)*4)
                                   //����������ַ         ��������ƫ����

#define MEMADDR(addr,bit)  *(unsigned int *)BITBAND(addr,bit)


//IDR
#define PAin(bit)  MEMADDR((unsigned int)&GPIOA->IDR,bit)
#define PBin(bit)  MEMADDR((unsigned int)&GPIOB->IDR,bit)
#define PCin(bit)  MEMADDR((unsigned int)&GPIOC->IDR,bit)
#define PDin(bit)  MEMADDR((unsigned int)&GPIOD->IDR,bit)
#define PEin(bit)  MEMADDR((unsigned int)&GPIOE->IDR,bit)
#define PFin(bit)  MEMADDR((unsigned int)&GPIOF->IDR,bit)
#define PGin(bit)  MEMADDR((unsigned int)&GPIOG->IDR,bit)
#define PHin(bit)  MEMADDR((unsigned int)&GPIOH->IDR,bit)

//ODR
#define PAout(bit)  MEMADDR((unsigned int)&GPIOA->ODR,bit)
#define PBout(bit)  MEMADDR((unsigned int)&GPIOB->ODR,bit)
#define PCout(bit)  MEMADDR((unsigned int)&GPIOC->ODR,bit)
#define PDout(bit)  MEMADDR((unsigned int)&GPIOD->ODR,bit)
#define PEout(bit)  MEMADDR((unsigned int)&GPIOE->ODR,bit)
#define PFout(bit)  MEMADDR((unsigned int)&GPIOF->ODR,bit)
#define PGout(bit)  MEMADDR((unsigned int)&GPIOG->ODR,bit)
#define PHout(bit)  MEMADDR((unsigned int)&GPIOH->ODR,bit)




#endif

