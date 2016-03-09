#define GPMCON (volatile unsigned long *)(0x7F008820) 
#define GPMDAT (volatile unsigned long *)(0x7F008824)

int uboot_main()
{
	(*GPMCON) = 0x1001;
	(*GPMDAT) = 0x36;
	return 0;
}
