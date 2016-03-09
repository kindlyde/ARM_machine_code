#define GPICON (*(volatile unsigned long*)0x7F008100)
#define GPJCON (*(volatile unsigned long*)0x7F008120)

#define GPFCON (*(volatile unsigned long*)0x7F0080A0)
#define GPFDAT  (*((volatile unsigned long *)0x7F0080A4))

//特殊端口寄存器
#define SPCON (*(volatile unsigned long*)0x7F0081A0)

//设置调制解调控制寄存器
#define MIFPCON (*(volatile unsigned long*)0x7410800C)

//图像时序控制寄存器
#define VIDTCON0 (*(volatile unsigned long*)0x77100010)
#define VIDTCON1 (*(volatile unsigned long*)0x77100014)
#define VIDTCON2 (*(volatile unsigned long*)0x77100018)

//图像主控制寄存器
#define VIDCON0 (*(volatile unsigned long*)0x77100000)
#define VIDCON1 (*(volatile unsigned long*)0x77100004)

//窗口的格式
#define WINCON0 (*(volatile unsigned long*)0x77100020)

//窗口0大小
#define VIDOSD0A (*(volatile unsigned long*)0x77100040)
#define VIDOSD0B (*(volatile unsigned long*)0x77100044)
#define VIDOSD0C (*(volatile unsigned long*)0x77100048)

//帧缓存大小
#define VIDW00ADD0B0 (*(volatile unsigned long*)0x771000A0)
#define VIDW00ADD1B0 (*(volatile unsigned long*)0x771000D0)
#define VIDW00ADD2 (*(volatile unsigned long*)0x77100100)

//抖动模式寄存器
#define DITHMODE (*(volatile unsigned long*)0x77100170)

#define VSPW 9 //vertical sync pulse width 典型值为10，根据时序图vspw+1=10
#define VBPD 1 //vbpd + 1 = 2 
#define VFPD 1 //vfpd + 1 = 2 
#define HSPW 40  //hspw + 1 = 41 
#define HBPD 1 //hbpd + 1 = 2
#define HFPD 1 //hfpd + 1 = 2
#define LINEVAL 271 //行数
#define HOZVAL 479 //列数
#define VCLK 1

unsigned int LCDBUFFER[544][960];

void lcd_init()
{
	/* 1.引脚初始化 */
	GPICON |= 0xaaaaaaaa;
	GPJCON |= 0xaaaaaa;
	
	GPFCON &= ~(0x3<<28);
	GPFCON |= (0<<28);//LCD背光使能需关闭。。？
	
	//将lcd设置为普通模式
	MIFPCON &= ~(1<<3);
	
	//将LCD端口引脚设置为RGB I/F 模式
	SPCON    &= ~(0x3);
	SPCON |= (1<<0);
	

	VIDCON0 &= ~((3<<26) | (3<<17) | (0xff<<6)  | (3<<2));     /* RGB I/F, RGB Parallel format,  */
	VIDCON0 |= ((14<<6) | (1<<4) );      /* vclk== HCLK / (CLKVAL+1) = 133/15 = 9MHz */

	VIDCON1 &= ~(1<<7);   /* 在vclk的下降沿获取数据 */
	VIDCON1 |= ((1<<6) | (1<<5));  /* HSYNC高电平有效, VSYNC高电平有效, */

	//VIDEO MAIN CONTROL 0、1 REGISTER  P494
	//VIDCON0 |= ((9<<6)|(0<<5)|(1<<4)|(0<<3)|(0<<2));  
	//VIDCON1 |= (VCLK << 7);

	/* 2.时序初始化;    时序不同显示的图像不同 */
 	VIDTCON0 |= ((VSPW) | (VFPD<<8) | (VBPD<<16));
	VIDTCON1 |= ((HSPW) | (HFPD<<8) | (HBPD<<16));
	VIDTCON2 |= ((LINEVAL<<11) | (HOZVAL));
 
	//窗口的格式
	WINCON0 &= ~(0xf << 2);
	WINCON0 |= (0xb<<2); //unpacked 24 BPP (non-palletized R:8-G:8-B:8 )
	 
	//设置窗口0的大小
	VIDOSD0A = ((0<<11) | (0<<0)); //左上角坐标
	VIDOSD0B = ((479<<11) | (271<<0)); //右下角坐标
	VIDOSD0C = (480 * 272); //Eq. Height * Width (Number of Word)；注
	
	/* 3.帧缓存大小 */
	VIDW00ADD0B0 = ((((unsigned long)LCDBUFFER)>>24)<<24) | (((unsigned long)LCDBUFFER)&0xffffff);// 此寄存器的[31:24]:图像缓冲在内存中的地址的A[31:24]
	VIDW00ADD1B0 = (((unsigned long)LCDBUFFER)&0xffffff + 960*544);  //此寄存器的[23:0]：图像帧缓存的结束地址
	VIDW00ADD2 = (0<<13) | ((480*4)<<0);  //可不用。。
	
	//设置red dither bit ;green dither bit; blue dither bit 控制位
	//DITHMODE |= ((0<<5) | (0<<3) | (0<<1) | (1<<0));//是否设置此寄存器好像没啥影响

	
	//开显示
	VIDCON0 |= (3<<0);  
	WINCON0 |= (1<<0);

}

void LCD_DrawPixel(unsigned int x, unsigned int y, unsigned int color)  
{  
	if((x<960) && (y<544))         
	LCDBUFFER[y][x] = color;  
}  

void lcd_test(void)
{
    int x,y;
	for (y = 0; y < 544; y++)
	{
	   for(x = 0; x < 960; x++)
		 {   
			LCD_DrawPixel(x,y,0xFF0000);
		 }
	}
 }		

/* extern const unsigned char bmp[193388];
 
void Paint_Bmp(const unsigned char *bmp)
{
    unsigned int x,y;
    unsigned int c;
    unsigned int p = 0;
    
    for( y = 0 ; y < 544; y++ )
    {
    	for( x = 0; x < 960; x++ )
    	{
    	    //c = (unsigned int)(bmp[p] | (bmp[p+1]<<8) | (bmp[p+2]<<16)) ;	//加入此句则程序不显示任何颜色。而且死机。。？11/22
			//c = 0xff0000;
	    
			LCD_DrawPixel(x,y,0xff0000);
			
			p = p + 3 ;	
    	}
    }
}
 */ 
 

/* void draw_line(void)
{
	unsigned int x;
	unsigned int *addr = (unsigned int *)fb_base_addr;//卡死。。。
 	for(x=0; x < xsize; x++)
     {   
		*(addr + x)= 0xFFFF00;
	 } 
} */
