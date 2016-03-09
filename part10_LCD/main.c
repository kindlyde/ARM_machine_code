extern const unsigned char bmp[193388];
extern void lcd_test();
 
int gboot_main()
{
#ifdef MMU_ON
    mmu_init();
#endif
	
	led_init();
	key_init();
	int_irq();
	uart_init();
	lcd_init();
	lcd_test();
	led_on();
	while(1);
    return 0;    
}
