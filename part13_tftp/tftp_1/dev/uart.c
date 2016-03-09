#define GPA0CON (*(volatile unsigned short*)0x7F008000)

#define ULCON0 (*(volatile unsigned long*)0x7F005000)
#define UCON0 (*(volatile unsigned long*)0x7F005004)

#define UBRDIV0 (*(volatile unsigned short*)0x7F005028)
#define UDIVSLOT0 (*(volatile unsigned short*)0x7F00502C)

#define UTRSTAT0 (*(volatile unsigned long*)0x7F005010)

#define UTXH0 (*(volatile unsigned char*)0x7F005020)
#define URXH0 (*(volatile unsigned char*)0x7F005024)

#define PCLK 66000000 //66MHZ
#define bps 115200 

/* 串口的初始化 */
void uart_init()
{
	
	/*  1.配置引脚功能（GPA） */
	GPA0CON &= ~0xff;
	GPA0CON |= 0x22;
	
	/* 2.设置数据格式(ULCON) */
	ULCON0 = 0b11;
	
	/*3设置其工作模式为轮询方式*/	
	UCON0 = 0b0101; 
	
	/* 4.配置波特率 ；UBRDIV0(等式后取整的值) = (PCLK / (bps x 16) ) -1
		UDIVSLOT0(得出的小数部分乘以16再查表得) = (PCLK / (bps x 16) ) -1*/
		
	UBRDIV0 = (int)((PCLK / (bps * 16)) -1);
	UDIVSLOT0 = 0xdddd;
}

void putc(unsigned char ch)
{
	while(!(UTRSTAT0 & (1<<2)));
	UTXH0 = ch;
}

unsigned char getc(void)
{
	unsigned char ret;
	
	while(!(UTRSTAT0 & (1<<0)));
	
	ret = URXH0;
	
	if ( (ret == 0x0d) || (ret == 0x0a) )
	{
	    putc(0x0d);
	    putc(0x0a);	
	}      	
	else
	    putc(ret);
	    
        return ret;
}