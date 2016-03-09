#define EINT0CON0 (volatile unsigned long*)0x7F008900  //�ⲿ�жϿ��ƼĴ���
#define EINT0MASK (volatile unsigned long*)0x7F008920	//�ⲿ�ж�0���μĴ���
#define VIC0INTENABLE (volatile unsigned long*)0x71200010  //�жϿ�����0��VIC0��ʹ�ܼĴ���
#define EINT0_ADDR (volatile unsigned long*)0x71200100   //������ַ�Ĵ��� VECTOR ADDRESS REGISGERS,��ͬ�ж�Դ��Ӧ��ͬ��ַ���жϴ������
#define EINT5_ADDR (volatile unsigned long*)0x71200104    //�жϴ���������������������������ַ����������һ�𣬵Ʋ���

//�������
#define EINT0PEND (volatile unsigned long*)0x7F008924 //�ⲿ�ж�δ���Ĵ�������Ӧλ����λʱ˵�����ⲿ�ж��Ѿ��������ж�ִ�д������֮���轫��Ӧλ����
#define VIC0ADDRESS (volatile unsigned long*)0x71200F00 //��ִ�ж�Ӧ�ж�ʱ���Ὣ��Ӧ�Ĵ�������ַ����üĴ�����ִ�����Ӧ���
#define VIC1ADDRESS (volatile unsigned long*)0x71300F00 //�ж�����������VIC1��Ӧ�ļĴ���

void key2_irq(void)
{
	//1�����滷��
	__asm__(
	"sub lr, lr, #4\n"
	"stmfd sp!, {r0-r12,lr}\n"

	);
	//2��������
	led_on();
	
	//3������ж�
	*(EINT0PEND) = ~0x0;
	*(VIC0ADDRESS) = 0;
	//*(VIC1ADDRESS) = 0;
	
	//4���ָ�����
	__asm__(
	"ldmfd sp!, {r0-r12,pc}^\n"
	:
	:
	);

}

void key7_irq(void)
{
	//1�����滷��
	__asm__(
	"sub lr, lr, #4\n"
	"stmfd sp!, {r0-r12,lr}\n"

	);
	//2��������
	led_off();
	
	//3������ж�
	*(EINT0PEND) = ~0x0;
	*(VIC0ADDRESS) = 0;
	//*(VIC1ADDRESS) = 0;
	
	//4���ָ�����
	__asm__(
	"ldmfd sp!, {r0-r12,pc}^\n"
	:
	:
	);
}



void int_irq(void)
{
	//1�������ⲿ�ж�Ϊ�½��ش���
	*(EINT0CON0) = 0b010 | (0b010<<8);
	
	//2��ȡ���ж�����λEINT0MASK[0]��[5]
	
	*(EINT0MASK) &= ~(1 | (1<<5)); 
	/* *(EINT0MASK) = 0; */
	
	//3��ʹ���ж�Դ����	�����ֲ�P410  ��Ҫע����ʹ�ܷ�ʽ��������
	*(VIC0INTENABLE) |= (0b1 )| (0b10);
	
	//���ж�������ַ�Ĵ���д�벻ͬ�ж�Դ��Ӧ���жϴ������
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