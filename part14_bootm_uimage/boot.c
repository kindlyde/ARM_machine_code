#include "tag.h"
#include <string.h>

#define SDRAM_KERNEL_START  0x51000000 //下载程序到此处即可 ；为内核的启动地址处
#define SDRAM_TAGS_START  0x50000100 // 启动参数的保存地址
#define SDRAM_MEM_START  0x50000000 // 内存地址
#define SDRAM_TOTAL_SIZE  0x10000000 // 内存大小256M


void (*thekernel)(int , int , unsigned int); //声明的函数指针

struct tag *pcurtag;

void steup_core_tag()
{
	pcurtag = (struct tag *)SDRAM_TAGS_START;
	
	pcurtag->hdr.tag = ATAG_CORE;
	pcurtag->hdr.size = tag_size(tag_core);
	
	pcurtag->u.core.flags = 0;
	pcurtag->u.core.pagesize = 4096;
	pcurtag->u.core.rootdev = 0;

	pcurtag = tag_next(pcurtag);
	
}

void steup_mem_tag()
{
	pcurtag->hdr.tag = ATAG_MEM;
	pcurtag->hdr.size = tag_size (tag_mem32);
	
	pcurtag->u.mem.start = SDRAM_MEM_START;
	pcurtag->u.mem.size = SDRAM_TOTAL_SIZE;

	pcurtag = tag_next (pcurtag);

}

void steup_cline_tag()
{
	int linelen = strlen(cmdline);
	
	pcurtag->hdr.tag = ATAG_CMDLINE;
	pcurtag->hdr.size =(sizeof (struct tag_header) + linelen  + 1 + 4) >> 2;

	strcpy (pcurtag->u.cmdline.cmdline, cmdline);

	pcurtag = tag_next (pcurtag);

	
}

void steup_end_tag()
{
	pcurtag->hdr.tag = ATAG_NONE;
	pcurtag->hdr.size = 0;
}

void boot_linux(int , int , unsigned int)
{
	/* 1、获取linux启动地址 */
	thekernel = (void (*)(int , int , unsigned int))SDRAM_KERNEL_START; //将内核的启动地址声明为函数指针
	
	/* 2、设置启动参数 主要的启动参数*/
	/* 核心启动参数、 */ 
	steup_core_tag();
	
	/* 内存参数、 */
	steup_mem_tag();
	
	/* 命令行参数、 */
	steup_cline_tag();
	
	/* 参数结束） */
	steup_end_tag();
	
    /* 3、启动linux系统 */
	thekernel(0,1999,SDRAM_TAGS_START);

}