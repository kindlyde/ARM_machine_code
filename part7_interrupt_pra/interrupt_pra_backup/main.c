void mmu_init(); //需声明？？不声明，make不通过

int gboot_main()
{
	/* mmu_init(); */
	
	led_init();
	led_on();

    return 0;    
}
