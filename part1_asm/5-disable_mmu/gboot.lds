OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS {
	. = 0x50008000;
	
	. = ALIGN(4);     //代码段 4 字节对齐     2014/9/1添加注释
	.text :            //代码段
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
