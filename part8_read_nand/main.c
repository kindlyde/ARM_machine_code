void mmu_init(); //������������������make��ͨ��

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
