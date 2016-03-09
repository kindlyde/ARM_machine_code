#ifdef MMU_ON

#define GPMCON (volatile unsigned long*)0xA0008820
#define GPMDAT (volatile unsigned long*)0xA0008824

#else

#define GPMCON (volatile unsigned long*)0x7f008820
#define GPMDAT (volatile unsigned long*)0x7f008824

#endif

void led_init(void)
{
	*(GPMCON) = 0x1111;//��ʼ��Ϊ���
}

void led_on(void)
{
	*(GPMDAT) = 0x0;//�͵�ƽ��
}

void led_off(void)
{
	*(GPMDAT) = 0xf;//�ߵ�ƽ��
}