#define GPNCON (volatile unsigned long*)0x7F008830

void key_init(void)
{

	*(GPNCON) = 0b10 ;
	//*(GPNCON) = 0b10 | (0b10 << 10);//设置引脚功能为外部中断触发按键s2、s7
}