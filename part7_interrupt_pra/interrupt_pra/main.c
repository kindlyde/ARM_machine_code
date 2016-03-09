void mmu_init(); //需声明？？不声明，make不通过

int gboot_main()
{
/* 	mmu_init(); */
	
	led_init();
	key_init();
	int_irq();
	led_off();
	while(1);
    return 0;    
}
