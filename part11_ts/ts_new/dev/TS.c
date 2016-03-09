#define ADCCON (*(volatile unsigned long *)0x7E00B000)
#define ADCTSC (*(volatile unsigned long *)0x7E00B004)//轮询按下或弹起
#define ADCDLY (*(volatile unsigned long *)0x7E00B008)
#define ADCDAT0 (*(volatile unsigned long *)0x7E00B00C)//读取X轴坐标
#define ADCDAT1 (*(volatile unsigned long *)0x7E00B010)//读取Y轴坐标

#define ADCCLRINTPNDNUP (*(volatile unsigned char *)0x7E00B020)//清除按下/抬起中断寄存器
#define ADCCLRINT (*(volatile unsigned char *)0x7E00B018)//清除ADC中断寄存器

#define VIC1INTENABLE (*(volatile unsigned long *) 0x71300010)
#define VIC1VECTADDR30 (*(volatile unsigned long *) 0x71300178)
#define VIC1VECTADDR31 (*(volatile unsigned long *) 0x7130017c)
#define VIC1ADDRESS (*((volatile unsigned long *)0x71300f00))
#define VIC0ADDRESS (*((volatile unsigned long *)0x71200f00))
static int x, y;
static char has_get_xy;


void wait_for_down(void)
{
	has_get_xy = 0; 
	ADCTSC = 0xd3;//触摸屏控制寄存器，等待被按下
}

void wait_for_up(void)
{
	ADCTSC = 0x1d3;//等待弹起
}

/* TC中断处理 */
void tc_handler()
{
	
	
	if(!(ADCDAT0 & (1<<15))) //被按下
	{
	/* 1、启动XY坐标自动转换 */
	ADCTSC &= ~(0b11<<0);
	ADCTSC |= (1<<2)|(1<<3);
	ADCCON |= (1<<0);
	
	ADCDLY = 0xffff;
	printf("X is %d,Y is %d",x,y);	
	while(ADCCON & 0x01);//设置AD自动转换，启动转换后此位自动清零；貌似ADCCON[0]位未自动清零
	printf("X is %d,Y is %d",x,y);
	/* 2、等待转换完成 */
	while(!(ADCCON & 0x8000));
	
	/* 3、获取坐标 */
	x = (ADCDAT0&0X3FF);
	y = (ADCDAT1&0X3FF);
	
	has_get_xy = 1;
	
	ADCCLRINT = 0;//清AD转换中断
	
	wait_for_up();//设置为等待弹起
	
	}
	else
	{
		wait_for_down();
		
	}	
	//清中断
	
	ADCCLRINTPNDNUP = 0;
	VIC0ADDRESS = 0;
	VIC1ADDRESS = 0;

}

/* 触摸屏初始化 */
void tc_init()
{
	/* 1、清除触摸屏ADC寄存器和触摸屏唤醒中断寄存器 */
	ADCCLRINTPNDNUP = 0;//触摸屏ADC寄存器
	ADCCLRINT = 0;//触摸屏唤醒中断寄存器
	
	/* 2、设置AD转换时钟,注意第[2]位的清零 */
	ADCCON |= ((1<<14) | (65<<6)|(0<<2)|(0<<1));
	
	/* 2.1接收到中断后，延时此时间段就会进行AD转换， */
	ADCDLY = 0xffff;
	
	/* 3、设置中断屏蔽位（TC中断或者ADC中断）；使能TC中断，使能ADC中断*/
	
	VIC1VECTADDR30 = tc_handler;//向量中断地址中放入相应中断处理程序
	VIC1INTENABLE |= (1<<30);//使能按下弹起（TC）中断 ADC Pen down/up interrupt

	/* 4、进入等待中断模式 */
	ADCTSC = 0xd3;//等待被按下
	
}