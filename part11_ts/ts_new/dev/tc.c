#define ADCCON (*(volatile unsigned long *)0x7E00B000)
#define ADCTSC (*(volatile unsigned long *)0x7E00B004)//��ѯ���»���
#define ADCDLY (*(volatile unsigned long *)0x7E00B008)
#define ADCDAT0 (*(volatile unsigned short *)0x7E00B00C)//��ȡX������
#define ADCDAT1 (*(volatile unsigned short *)0x7E00B010)//��ȡY������

#define ADCUPDN (*(volatile unsigned long *)0x7E00B014)

#define ADCCLRINTPNDNUP (*(volatile unsigned char *)0x7E00B020)//�������/̧���жϼĴ���
#define ADCCLRINT (*(volatile unsigned char *)0x7E00B018)//���ADC�жϼĴ���

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
	//1�����滷��
	__asm__(
	"sub lr, lr, #4\n"
	"stmfd sp!, {r0-r12,lr}\n"

	);
	//2��������
	if (ADCDAT0 & (1<<15))//����
	{
		printf("pen up\n\r");
		wait_for_pen_down();
	}
	else
	{
		printf("pen down\n\r");
		auto_measure_xy_mode();
		start_adc();
		while(ADCCON & 0x01);//����AD�Զ�ת��������ת�����λ�Զ����㣻ò��ADCCON[0]λδ�Զ�����
		x = (ADCDAT0&0X3FF);
		y = (ADCDAT1&0X3FF);
		wait_for_pen_up();
		printf("[x,y] = [%d, %d]\n\r", x, y);
		
	}
	//3������ж�
	ADCUPDN = 0;
	ADCCLRINTPNDNUP = 0;
	VIC1ADDRESS = 0;
	VIC0ADDRESS = 0;
	
	//4���ָ�����
	__asm__(
	"ldmfd sp!, {r0-r12,pc}^\n"
	:
	:
	);
}

/* void irq_adc(void)
{	
	printf("[x,y] = [%d, %d]\n\r", x, y);
	ADCCLRINT = 1;//���AD�ж�
	if ((ADCDAT0 & (1<<15)) == 0) /* ��������Ȼ���ڰ���״̬ */
/* 	{
		x = (ADCDAT0&0X3FF);
		y = (ADCDAT1&0X3FF);
		has_get_xy = 1;
		wait_for_pen_up();
	}
	else
	{
/* 		/* ��ADC�ڼ�, �������Ѿ��ɿ��� */
/* 		wait_for_pen_down();
	}
	
	VIC1ADDRESS = 0;
	VIC0ADDRESS = 0;

} */  


void ts_init(void)
{
	/* һ�������:����ʱ�� */
	ADCCON &= ~((0xff << 6) | (1<<2));
	
	ADCCON |= (1<<14) | (65<<6);  
	
	//ADCCLRINT = 1;//���AD�ж�
	ADCCLRINTPNDNUP = 1;
	ADCDLY = 0xffff;
		
	/* �ж����� */
	VIC1VECTADDR30 = irq_pen_down_up;
	//VIC1VECTADDR31 = irq_adc;
	VIC1INTENABLE |= (1<<30); /* ʹ��INT_PENDNUP, INT_ADC */ 

	wait_for_pen_down();
	
}
