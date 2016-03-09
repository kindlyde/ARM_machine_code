#define NFCONT (*((volatile unsigned long *)0x70200004))
#define NFCMMD (*((volatile unsigned char *)0x70200008))//此处寄存器的操作应为单字节操作，应正确定义其内存数据类型（unsigned char *）
#define NFADDR (*((volatile unsigned char *)0x7020000c)) //同时比较以下规范定义内存的宏定义
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
//选中芯片
	select_chip();
//清除RnB
	clear_RnB();
//发送命令
	send_cmd(0xff);
//等待RnB
	wait_RnB();
//取消选中
	deselect_chip();
}

/*
HCLK的频率为100MHZ，周期就为10ns
TACLS > 0 ns
TWRPH0	> 15ns
TWRPH1 > 5ns
 
TACLS的值 = HCLK x TACLS > 0ns
TWRPH0的值 = HCLK x (TWRPH0 + 1) > 15ns
TWRPH1的值 = HCLK x (TWRPH1 +1) > 5ns 
*/
	
#define TACLS 1
#define TWRPH0 2
#define TWRPH1 1

void nand_init(void)
{

	/* 1、初始化NFCONF */
	NFCONF &= ~((7<<12)|(7<<8)|(7<<4)); //先清零
	NFCONF |= ((TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4)); 
	
	/* 2、初始化NFCONT */
	
	NFCONT = 1 | (1<<1);//使nandflash工作，但不选中芯片
	
    /* 3、复位 */
	reset();
}

//nandflash按页读函数
//addr；读取的页地址
//buf:要存入的地址
void Nand_Read(unsigned long addr, unsigned char* buf)
{		
		int i;


		/* 1、先使能选中nandflash芯片 */
		select_chip();

		/* 2、查询nandflash的状态寄存器NFSTAT的0位等于1 或者 清除状态寄存器NFSTAT的第4位RnB；使其为高电平，代表前面的命令执行完毕*/
		clear_RnB();

		/* 3、发送命令0x00 */
		send_cmd(0x00);

		/* 4、发送五个时序周期的地址：依次发送两个列地址、三个行地址 （先发低位、再发高位）*/
		send_addr(0x00);//列地址A0-A7
		send_addr(0x00);//列地址A8-A12
		
		send_addr((addr)&0xff);//行地址A13-A20
		send_addr((addr>>8) & (0xff));//行地址A21-A28
		send_addr((addr>>16) & (0xff));//行地址A29-A31
		
		/* 5、发送命令0x30 */
		send_cmd(0x30);

		/* 6、等待R/B（就绪/忙输出信号线）变成高电平。*/
		wait_RnB();

		/* 7、读取数据 ;读取的整个页：4K*/
		
		 for(i = 0; i<1024*4; i++)
	    {
	    	buf[i] = NFDATA;
	    }
      
		 /* 8、失能未选中nandflash芯片。*/
		deselect_chip();
}

/*函数功能： 将nandflash中的代码搬移到内存中
  参数：start_addr nandflash的起始地址
		sdram_addr 搬移到内存的位置
		size 复制代码的大小
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
   ok6410启动时拷贝的8K代码不是存储在Nand flash的第一页上，
   而是存储在Nand flash的前4页上，每页2K(兼容性问题)，总共8K
  
*/
	for (i = 0; i < 4; i++, sdram_addr+=2048) //此处现在搬移的前4K，4K也可以
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

