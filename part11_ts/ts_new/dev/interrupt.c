#define EINT0CON0 (volatile unsigned long*)0x7F008900  //外部中断控制寄存器
#define EINT0MASK (volatile unsigned long*)0x7F008920	//外部中断0屏蔽寄存器
#define VIC0INTENABLE (volatile unsigned long*)0x71200010  //中断控制器0（VIC0）使能寄存器
#define EINT0_ADDR (volatile unsigned long*)0x71200100   //向量地址寄存器 VECTOR ADDRESS REGISGERS,不同中断源对应不同地址的中断处理程序
#define EINT5_ADDR (volatile unsigned long*)0x71200104    //中断处理向量必须放在向量表的连续地址？？不放在一起，灯不亮

//清除工作
#define EINT0PEND (volatile unsigned long*)0x7F008924 //外部中断未决寄存器，对应位被置位时说明有外部中断已经被处理；中断执行处理程序之后需将对应位清零
#define VIC0ADDRESS (volatile unsigned long*)0x71200F00 //当执行对应中断时，会将对应的处理程序地址放入该寄存器，执行完毕应清除
#define VIC1ADDRESS (volatile unsigned long*)0x71300F00 //中断向量控制器VIC1对应的寄存器

void key2_irq(void)
{
	//1、保存环境
	__asm__(
	"sub lr, lr, #4\n"
	"stmfd sp!, {r0-r12,lr}\n"

	);
	//2、处理部分
	led_on();
	
	//3、清除中断
	*(EINT0PEND) = ~0x0;
	*(VIC0ADDRESS) = 0;
	//*(VIC1ADDRESS) = 0;
	
	//4、恢复环境
	__asm__(
	"ldmfd sp!, {r0-r12,pc}^\n"
	:
	:
	);

}

void key7_irq(void)
{
	//1、保存环境
	__asm__(
	"sub lr, lr, #4\n"
	"stmfd sp!, {r0-r12,lr}\n"

	);
	//2、处理部分
	led_off();
	
	//3、清除中断
	*(EINT0PEND) = ~0x0;
	*(VIC0ADDRESS) = 0;
	//*(VIC1ADDRESS) = 0;
	
	//4、恢复环境
	__asm__(
	"ldmfd sp!, {r0-r12,pc}^\n"
	:
	:
	);
}



void int_irq(void)
{
	//1、设置外部中断为下降沿触发
	*(EINT0CON0) = 0b010 | (0b010<<8);
	
	//2、取消中断屏蔽位EINT0MASK[0]、[5]
	
	*(EINT0MASK) &= ~(1 | (1<<5)); 
	/* *(EINT0MASK) = 0; */
	
	//3、使能中断源；见	数据手册P410  需要注意其使能方式。。。。
	*(VIC0INTENABLE) |= (0b1 )| (0b10);
	
	//向中断向量地址寄存器写入不同中断源对应的中断处理程序
	*(EINT0_ADDR) = (int)key2_irq;
	*(EINT5_ADDR) = (int)key7_irq;
	
	__asm__ (
	
	"mrc p15,0,r0,c1,c0,0\n"
    "orr r0,r0,#(1<<24)\n"
    "mcr p15,0,r0,c1,c0,0\n"
	
	"mrs r0, cpsr\n"
	"bic r0, r0, #0x80\n"
	"msr cpsr, r0\n"
	:
	:
	);
	
}