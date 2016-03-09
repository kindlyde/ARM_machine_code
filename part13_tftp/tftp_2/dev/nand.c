#define NFCONT (*((volatile unsigned long *)0x70200004))
#define NFCMMD (*((volatile unsigned char *)0x70200008))//�˴��Ĵ����Ĳ���ӦΪ���ֽڲ�����Ӧ��ȷ�������ڴ��������ͣ�unsigned char *��
#define NFADDR (*((volatile unsigned char *)0x7020000c)) //ͬʱ�Ƚ����¹淶�����ڴ�ĺ궨��
#define NFDATA (*((volatile unsigned char *)0x70200010)) 

#define NFSTAT (*((volatile unsigned char *)0x70200028))  

#define NFCONF (*((volatile unsigned long *)0x70200000)) //NAND Flash Configuration register
 
/* #define NFCONF             (*((volatile unsigned long*)0x70200000))
#define NFCONT             (*((volatile unsigned long*)0x70200004))
#define NFCMMD             (*((volatile unsigned char*)0x70200008))
#define NFSTAT             (*((volatile unsigned char*)0x70200028))
#define NFADDR             (*((volatile unsigned char*)0x7020000c))
#define NFDATA             (*((volatile unsigned char*)0x70200010))
 */

void select_chip(void)
{
	NFCONT &= ~(1<<1);
}

void deselect_chip(void)
{
	NFCONT |= (1<<1);
}

void clear_RnB()
{
	NFSTAT |= (1<<4);
}

void wait_RnB()
{
	while(!(NFSTAT & 0x1));
}


void send_cmd(unsigned char cmd)
{
	NFCMMD = cmd;
}

void send_addr(unsigned char addr)
{
	NFADDR = addr;
}



void reset(void)
{
//ѡ��оƬ
	select_chip();
//���RnB
	clear_RnB();
//��������
	send_cmd(0xff);
//�ȴ�RnB
	wait_RnB();
//ȡ��ѡ��
	deselect_chip();
}

/*
HCLK��Ƶ��Ϊ100MHZ�����ھ�Ϊ10ns
TACLS > 0 ns
TWRPH0	> 15ns
TWRPH1 > 5ns
 
TACLS��ֵ = HCLK x TACLS > 0ns
TWRPH0��ֵ = HCLK x (TWRPH0 + 1) > 15ns
TWRPH1��ֵ = HCLK x (TWRPH1 +1) > 5ns 
*/
	
#define TACLS 1
#define TWRPH0 2
#define TWRPH1 1

void nand_init(void)
{

	/* 1����ʼ��NFCONF */
	NFCONF &= ~((7<<12)|(7<<8)|(7<<4)); //������
	NFCONF |= ((TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4)); 
	
	/* 2����ʼ��NFCONT */
	
	NFCONT = 1 | (1<<1);//ʹnandflash����������ѡ��оƬ
	
    /* 3����λ */
	reset();
}

//nandflash��ҳ������
//addr����ȡ��ҳ��ַ
//buf:Ҫ����ĵ�ַ
void Nand_Read(unsigned long addr, unsigned char* buf)
{		
		int i;


		/* 1����ʹ��ѡ��nandflashоƬ */
		select_chip();

		/* 2����ѯnandflash��״̬�Ĵ���NFSTAT��0λ����1 ���� ���״̬�Ĵ���NFSTAT�ĵ�4λRnB��ʹ��Ϊ�ߵ�ƽ������ǰ�������ִ�����*/
		clear_RnB();

		/* 3����������0x00 */
		send_cmd(0x00);

		/* 4���������ʱ�����ڵĵ�ַ�����η��������е�ַ�������е�ַ ���ȷ���λ���ٷ���λ��*/
		send_addr(0x00);//�е�ַA0-A7
		send_addr(0x00);//�е�ַA8-A12
		
		send_addr((addr)&0xff);//�е�ַA13-A20
		send_addr((addr>>8) & (0xff));//�е�ַA21-A28
		send_addr((addr>>16) & (0xff));//�е�ַA29-A31
		
		/* 5����������0x30 */
		send_cmd(0x30);

		/* 6���ȴ�R/B������/æ����ź��ߣ���ɸߵ�ƽ��*/
		wait_RnB();

		/* 7����ȡ���� ;��ȡ������ҳ��4K*/
		
		 for(i = 0; i<1024*4; i++)
	    {
	    	buf[i] = NFDATA;
	    }
      
		 /* 8��ʧ��δѡ��nandflashоƬ��*/
		deselect_chip();
}

/*�������ܣ� ��nandflash�еĴ�����Ƶ��ڴ���
  ������start_addr nandflash����ʼ��ַ
		sdram_addr ���Ƶ��ڴ��λ��
		size ���ƴ���Ĵ�С
 */

void nand_to_sdram(unsigned long start_addr, unsigned char* sdram_addr, int size)
{
	int i;
/* 	for(i = (start_addr >> 12); size>0; )
	{
		Nand_Read(i, sdram_addr);
		size -= 4096;
		start_addr += 4096;
	}
	 */
/*
   ok6410����ʱ������8K���벻�Ǵ洢��Nand flash�ĵ�һҳ�ϣ�
   ���Ǵ洢��Nand flash��ǰ4ҳ�ϣ�ÿҳ2K(����������)���ܹ�8K
  
*/
	for (i = 0; i < 4; i++, sdram_addr+=2048) //�˴����ڰ��Ƶ�ǰ4K��4KҲ����
	{
		Nand_Read(i,sdram_addr);
	}
        
    size -= 1024*8;
        
    for( i=4; size>0;)
	{
	    Nand_Read(i,sdram_addr);	
	    size -= 4096;
	    sdram_addr += 4096;
	    i++;
	}	   
		   
}

