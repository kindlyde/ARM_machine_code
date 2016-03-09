#define ADCCON (*(volatile unsigned long *)0x7E00B000)
#define ADCTSC (*(volatile unsigned long *)0x7E00B004)//��ѯ���»���
#define ADCDLY (*(volatile unsigned long *)0x7E00B008)
#define ADCDAT0 (*(volatile unsigned long *)0x7E00B00C)//��ȡX������
#define ADCDAT1 (*(volatile unsigned long *)0x7E00B010)//��ȡY������

#define ADCCLRINTPNDNUP (*(volatile unsigned char *)0x7E00B020)//�������/̧���жϼĴ���
#define ADCCLRINT (*(volatile unsigned char *)0x7E00B018)//���ADC�жϼĴ���

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
	ADCTSC = 0xd3;//���������ƼĴ������ȴ�������
}

void wait_for_up(void)
{
	ADCTSC = 0x1d3;//�ȴ�����
}

/* TC�жϴ��� */
void tc_handler()
{
	
	
	if(!(ADCDAT0 & (1<<15))) //������
	{
	/* 1������XY�����Զ�ת�� */
	ADCTSC &= ~(0b11<<0);
	ADCTSC |= (1<<2)|(1<<3);
	ADCCON |= (1<<0);
	
	ADCDLY = 0xffff;
	printf("X is %d,Y is %d",x,y);	
	while(ADCCON & 0x01);//����AD�Զ�ת��������ת�����λ�Զ����㣻ò��ADCCON[0]λδ�Զ�����
	printf("X is %d,Y is %d",x,y);
	/* 2���ȴ�ת����� */
	while(!(ADCCON & 0x8000));
	
	/* 3����ȡ���� */
	x = (ADCDAT0&0X3FF);
	y = (ADCDAT1&0X3FF);
	
	has_get_xy = 1;
	
	ADCCLRINT = 0;//��ADת���ж�
	
	wait_for_up();//����Ϊ�ȴ�����
	
	}
	else
	{
		wait_for_down();
		
	}	
	//���ж�
	
	ADCCLRINTPNDNUP = 0;
	VIC0ADDRESS = 0;
	VIC1ADDRESS = 0;

}

/* ��������ʼ�� */
void tc_init()
{
	/* 1�����������ADC�Ĵ����ʹ����������жϼĴ��� */
	ADCCLRINTPNDNUP = 0;//������ADC�Ĵ���
	ADCCLRINT = 0;//�����������жϼĴ���
	
	/* 2������ADת��ʱ��,ע���[2]λ������ */
	ADCCON |= ((1<<14) | (65<<6)|(0<<2)|(0<<1));
	
	/* 2.1���յ��жϺ���ʱ��ʱ��ξͻ����ADת���� */
	ADCDLY = 0xffff;
	
	/* 3�������ж�����λ��TC�жϻ���ADC�жϣ���ʹ��TC�жϣ�ʹ��ADC�ж�*/
	
	VIC1VECTADDR30 = tc_handler;//�����жϵ�ַ�з�����Ӧ�жϴ������
	VIC1INTENABLE |= (1<<30);//ʹ�ܰ��µ���TC���ж� ADC Pen down/up interrupt

	/* 4������ȴ��ж�ģʽ */
	ADCTSC = 0xd3;//�ȴ�������
	
}