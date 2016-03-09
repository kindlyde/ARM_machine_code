void mmu_init(); //������������������make��ͨ��

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
	ts_init();
	
	while(1);
    return 0;    
}
