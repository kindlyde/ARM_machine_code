#define GPNCON (volatile unsigned long*)0x7F008830

void key_init(void)
{

	*(GPNCON) = 0b10 ;
	//*(GPNCON) = 0b10 | (0b10 << 10);//�������Ź���Ϊ�ⲿ�жϴ�������s2��s7
}