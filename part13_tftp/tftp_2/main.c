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
	led_off();
	dm_init();
	//dm9000_arp();
	while(1);
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
			tftp_send_request("a");//����������������������
			break;
		
			case 2:
			arp_request();//�õ�Mac��ַ
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