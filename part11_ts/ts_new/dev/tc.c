#define ADCCON (*(volatile unsigned long *)0x7E00B000)
#define ADCTSC (*(volatile unsigned long *)0x7E00B004)//轮询按下或弹起
#define ADCDLY (*(volatile unsigned long *)0x7E00B008)
#define ADCDAT0 (*(volatile unsigned short *)0x7E00B00C)//读取X轴坐标
#define ADCDAT1 (*(volatile unsigned short *)0x7E00B010)//读取Y轴坐标

#define ADCUPDN (*(volatile unsigned long *)0x7E00B014)

#define ADCCLRINTPNDNUP (*(volatile unsigned char *)0x7E00B020)//清除按下/抬起中断寄存器
#define ADCCLRINT (*(volatile unsigned char *)0x7E00B018)//清除ADC中断寄存器

#define VIC1INTENABLE (*(volatile unsigned long *) 0x71300010)
#define VIC1VECTADDR30 (*(volatile unsigned long *) 0x71300178)
#define VIC1VECTADDR31 (*(volatile unsigned long *) 0x7130017c)
#define VIC1ADDRESS (*((volatile unsigned long *)0x71300f00))
#define VIC0ADDRESS (*((volatile unsigned long *)0x71200f00))

static int x, y;
static char has_get_xy;

void wait_for_pen_down(void)
{
	has_get_xy = 0;
	ADCTSC = 0xd3;
}

void wait_for_pen_up(void)
{
	ADCTSC = 0x1d3;
}

void start_adc(void)
{	
	ADCCON |= (1<<0);
}

void auto_measure_xy_mode(void)
{	
	ADCTSC |= (1<<2)|(1<<3);
}



void irq_pen_down_up(void)
{
	//1、保存环境
	__asm__(
	"sub lr, lr, #4\n"
	"stmfd sp!, {r0-r12,lr}\n"

	);
	//2、处理部分
	if (ADCDAT0 & (1<<15))//弹起
	{
		printf("pen up\n\r");
		wait_for_pen_down();
	}
	else
	{
		printf("pen down\n\r");
		auto_measure_xy_mode();
		start_adc();
		while(ADCCON & 0x01);//设置AD自动转换，启动转换后此位自动清零；貌似ADCCON[0]位未自动清零
		x = (ADCDAT0&0X3FF);
		y = (ADCDAT1&0X3FF);
		wait_for_pen_up();
		printf("[x,y] = [%d, %d]\n\r", x, y);
		
	}
	//3、清除中断
	ADCUPDN = 0;
	ADCCLRINTPNDNUP = 0;
	VIC1ADDRESS = 0;
	VIC0ADDRESS = 0;
	
	//4、恢复环境
	__asm__(
	"ldmfd sp!, {r0-r12,pc}^\n"
	:
	:
	);
}

/* void irq_adc(void)
{	
	printf("[x,y] = [%d, %d]\n\r", x, y);
	ADCCLRINT = 1;//清除AD中断
	if ((ADCDAT0 & (1<<15)) == 0) /* 触摸笔仍然处于按下状态 */
/* 	{
		x = (ADCDAT0&0X3FF);
		y = (ADCDAT1&0X3FF);
		has_get_xy = 1;
		wait_for_pen_up();
	}
	else
	{
/* 		/* 在ADC期间, 触摸笔已经松开了 */
/* 		wait_for_pen_down();
	}
	
	VIC1ADDRESS = 0;
	VIC0ADDRESS = 0;

} */  


void ts_init(void)
{
	/* 一般的设置:比如时钟 */
	ADCCON &= ~((0xff << 6) | (1<<2));
	
	ADCCON |= (1<<14) | (65<<6);  
	
	//ADCCLRINT = 1;//清除AD中断
	ADCCLRINTPNDNUP = 1;
	ADCDLY = 0xffff;
		
	/* 中断设置 */
	VIC1VECTADDR30 = irq_pen_down_up;
	//VIC1VECTADDR31 = irq_adc;
	VIC1INTENABLE |= (1<<30); /* 使能INT_PENDNUP, INT_ADC */ 

	wait_for_pen_down();
	
}
