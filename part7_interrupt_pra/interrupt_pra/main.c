void mmu_init(); //������������������make��ͨ��

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
