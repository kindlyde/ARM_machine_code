OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS {
	. = 0x50008000;
	
	. = ALIGN(4);     //����� 4 �ֽڶ���     2014/9/1����ע��
	.text :            //�����
	{
	start.o (.text)
	*(.text)
	}

	. = ALIGN(4);
	.data : 
	{
	*(.data)
	}
	
	. = ALIGN(4);
	bss_start = .;
	.bss : 
	{
	*(.bss) 
	}
	bss_end = .;
}
