#include "DM9000.h"
#include "arp.h"

#define SROM_BW (*((volatile unsigned long *)0x70000000)) //SROM位宽\等待控制寄存器
#define SROM_BC1 (*((volatile unsigned long *)0x70000008)) //SROM Bank1 control register:对应DM9000的接口
#define DM_ADD (*((volatile unsigned short *)0x18000000)) //命令控制接口 (16位数据格式)
#define DM_DAT (*((volatile unsigned short *)0x18000004))	//数据控制接口

#define GPNCON (*((volatile unsigned long *)0x7F008830)) //外部中断7
#define EINT0CON0 (*((volatile unsigned long *)0x7F008900)) //外部中断0控制寄存器
#define EINT0MASK (*((volatile unsigned long *)0x7F008920)) //外部中断0屏蔽寄存器
#define EINT7_ADDR (*((volatile unsigned long*)0x71200104))   //向量地址寄存器 VECTOR ADDRESS REGISGERS,不同中断源对应不同地址的中断处理程序
#define VIC0INTENABLE (*((volatile unsigned long *)0x71200010)) //使能中断
//清除工作
#define EINT0PEND (*(volatile unsigned long*)0x7F008924) //外部中断未决寄存器，对应位被置位时说明有外部中断已经被处理；中断执行处理程序之后需将对应位清零
#define VIC0ADDRESS (*(volatile unsigned long*)0x71200F00) //当执行对应中断时，会将对应的处理程序地址放入该寄存器，执行完毕应清除
#define VIC1ADDRESS (*(volatile unsigned long*)0x71300F00) //中断向量控制器VIC1对应的寄存器

u8 host_mac_addr[6] = {0xff,0xff,0xff,0xff,0xff,0xff}; //因为不知道目的地址的MAC，所以先将其填充为0
u8 ipaddr[4] = {192,168,1,2};
u8 host_ip_addr[4] = {192,168,1,110};//实际主机IP
u8 mac_addr[6] = {9,8,7,6,5,4};  //此处任意设置MAC的地址
u8 *buffer = &arp_buf;
u16 packet_len;

void dm_isr(void);

//对DM9000寄存器写
void dm9000_reg_write(u16 reg,u16 data)
{
    DM_ADD = reg;        
    DM_DAT = data;      
}
//对DM9000寄存器读
u8 dm9000_reg_read(u16 reg)
{
    DM_ADD = reg;
    return DM_DAT;       
}

void cs_init()
{ 
   SROM_BW &= (~(0xf<<4));
   SROM_BW |=  (0x1<<4);
   SROM_BC1 =(0<<0)|(0x2<<4)|(0x2<<8)|(0x2<<12)|(0x2<<16)|(0x2<<24)|(0x2<<28);
}

static void dm9000_reset(void)
{
	dm9000_reg_write(DM9000_GPCR, GPCR_GPIO0_OUT); //设置为I/O为输出
	dm9000_reg_write(DM9000_GPR, 0);//开启内部PHY的电源
	dm9000_reg_write(DM9000_NCR, (NCR_LBK_INT_MAC | NCR_RST));//复位
	dm9000_reg_write(DM9000_NCR, 0);
	dm9000_reg_write(DM9000_NCR, (NCR_LBK_INT_MAC | NCR_RST));//第二轮的复位
	dm9000_reg_write(DM9000_NCR, 0);
}
/* 寻找DM9000芯片（移植dm9000_probe（void））;对比厂商ID、产品ID */
u8 dm9000_probe(void)
{
	u32 id_val;
	id_val = dm9000_reg_read(DM9000_VIDL);
	id_val |= dm9000_reg_read(DM9000_VIDH) << 8;
	id_val |= dm9000_reg_read(DM9000_PIDL) << 16;
	id_val |= dm9000_reg_read(DM9000_PIDH) << 24;
	if (id_val == DM9000_ID) {
		printf("DM9000 is found\n");
		return 0;
	} else {
		printf("dm9000 is not found \n");
		return -1;
	}

}

void dm_init(void)
{
	u8 i;
	/* 1、由于DM9000是一个单独的芯片，所以要设置其片选
	   1.1设置数据位宽
	   1.2设置其工作时序（参考SROM读写时序图） */
	 cs_init();
	
	/* 2、中断初始化（采用中断的方式使用DM9000）*/
	GPNCON &= ~(0b11<<14);
	GPNCON |= (0b10<<14);
	EINT0CON0 &= ~(0b111<<12);
	EINT0CON0 |= (0b010<<12);//下降沿触发中断
	EINT0MASK &= ~(1<<7);//取消屏蔽
	
	EINT7_ADDR = (int)dm_isr;
	VIC0INTENABLE = (1<<1);
	 
	 
	/* 3、复位设备（参考uboot得出）（注意第一步设置的位宽为16bit） */
	dm9000_reset();
	
	/* 4、寻找DM9000芯片（移植dm9000_probe（void）） */
	dm9000_probe();
	
	/* 5、MAC初始化（移植）,还需了解MAC详细初始化过程 */
	/* Program operating register, only internal phy supported */
	dm9000_reg_write(DM9000_NCR, 0x0);
	/* TX Polling clear */
	dm9000_reg_write(DM9000_TCR, 0);
	/* Less 3Kb, 200us */
	dm9000_reg_write(DM9000_BPTR, BPTR_BPHW(3) | BPTR_JPT_600US);
	/* Flow Control : High/Low Water */
	dm9000_reg_write(DM9000_FCTR, FCTR_HWOT(3) | FCTR_LWOT(8));
	/* SH FIXME: This looks strange! Flow Control */
	dm9000_reg_write(DM9000_FCR, 0x0);
	/* Special Mode */
	dm9000_reg_write(DM9000_SMCR, 0);
	/* clear TX status */
	dm9000_reg_write(DM9000_NSR, NSR_WAKEST | NSR_TX2END | NSR_TX1END);
	/* Clear interrupt status */
	dm9000_reg_write(DM9000_ISR, ISR_ROOS | ISR_ROS | ISR_PTS | ISR_PRS);
	
	
	/* 6、填充MAC地址 */
	/* fill device MAC address registers 10H-15H */
	for (i = 0; i < 6; i++)
		dm9000_reg_write(DM9000_PAR + i, mac_addr[i]);
		
	/* 7、激活DM9000	 */
	/* Activate DM9000 */
	/* RX enable */
	dm9000_reg_write(DM9000_RCR, RCR_DIS_LONG | RCR_DIS_CRC | RCR_RXEN);
	/* Enable TX/RX interrupt mask */
	dm9000_reg_write(DM9000_IMR, IMR_PAR);
}

/*  DM9000发送函数 */
void dm9000_tx(u8 *data,u32 length)
{
    u32 i;
    //禁止中断
    dm9000_reg_write(DM9000_IMR,0x80);
    //写入发送数据的长度
    dm9000_reg_write(DM9000_TXPLL, length & 0xff);
    dm9000_reg_write(DM9000_TXPLH, (length >> 8) & 0xff);
    //写入待发送的数据
    DM_ADD = DM9000_MWCMD;
	
    for(i=0; i<length; i+=2) 
    {
		DM_DAT = (data[i]) | (data[i+1]<<8);
	}
    //启动发送
    dm9000_reg_write(DM9000_TCR, TCR_TXREQ);
    //等待发送结束
	
    while(1)
    {
       u8 status;
       status = dm9000_reg_read(DM9000_TCR);
       if((status&0x01)==0x00)  //TCR寄存器的第0位发送完毕后自动清零
           break;
    }
    //清除发送状态
    dm9000_reg_write(DM9000_NSR,0x2c);
    //恢复中断使能
    dm9000_reg_write(DM9000_IMR,0x81);
	 
}

#define PKT_MAX_LEN 1522

u32 dm9000_rx(u8 *data)
{
	u8 status;
	u16 i,tmp;
	u32 len;
	
	/* 1、检测接收状态，判断产生接收中断,产生则代表接收到数据，未产生中断则返回---ISR（中断状态寄存器）的接收位第0位PR */
	/* 2、并且清除接收位---向ISR第0位写1 */
	if (!(dm9000_reg_read(DM9000_ISR) & 0x01)) /* Rx-ISR bit must be set. */
	{
		dm9000_reg_write(DM9000_ISR,0x01);		
	}
	else
	{
		return 0;
	}
	
	/* 3、空读（Dummy read）---MRCMDX */
	dm9000_reg_read(DM9000_MRCMDX);	/* Dummy read */
	
	//先隔过对数据的检查，进行以下操作
	
	/* 4、读取数据包状态 ----MRCMD *//* A packet ready now & Get status/length */ 
	status = dm9000_reg_read(DM9000_MRCMD);	
	
	/* 4.1读取数据包的长度----LEN= DM_DAT */
	len = DM_DAT;
	
	/* 5、读取包的数据--与以太网的数据的最大长度（ PKT_MAX	 1536）作比较，不可超过，注意接收的数据宽度为16位 */
	if(len < PKT_MAX_LEN)
	{
		for(i=0; i < len; i++)
		{
			tmp = DM_DAT;
			data[i] = tmp & 0xff;
			data[i+1] = (tmp >> 8) & 0xff;
		}
	}
	return len;
}

void dm_isr(void)
{
	//1、保存环境
	__asm__(
	"sub lr, lr, #4\n"
	"stmfd sp!, {r0-r12,lr}\n"

	);
	//2、处理部分
	packet_len = dm9000_rx(buffer);
	printf("\n\r************************************\n\r"); 
	arp_process();
	
	//3、清除中断
	EINT0PEND = ~0x0;
	VIC0ADDRESS = 0;
	
	//4、恢复环境
	__asm__(
	"ldmfd sp!, {r0-r12,pc}^\n"
	:
	:
	);


}

void dm9000_arp()
{
	while(1)
	{
		
		arp_request();
	}
}