void mmu_init(); //需声明？？不声明，make不通过

int gboot_main()
{

	int num;
#ifdef MMU_ON
    mmu_init();
#endif
	
	led_init();
	key_init();
	int_irq();
	uart_init();
	led_on();
	while(1)
	{
		
		printf("\n\r************************************\n\r");
		printf("\n\r**********kindlyde******************\n\r");
		printf("\n\r1:Downloads Linux Kernel from TFTP Servers\n\r");
		printf("\n\r2:Boot Linux from Ram\n\r");
		printf("\n\r3:Boot Linux from Nand Flash\n\r");
		printf("\n\rplease Select\n\r");
		
		scanf("%d", &num);
		//putc('a');
		switch (num)
		{
			case 1:
			
			break;
			
			case 2:
			
			break;
			
			case 3:
			
			break;
			
			default: 
				printf("\n\rError: please select agian\n\r");
			break;
		}
	
	
	}	
    return 0;    
}
