void mmu_init(); //需声明？？不声明，make不通过

int gboot_main()
{
#ifdef MMU_ON
    mmu_init();
#endif
	
	led_init();
	key_init();
	int_irq();
	led_on();
	while(1);
    return 0;    
}
