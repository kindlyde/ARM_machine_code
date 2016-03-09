#define GPICON (*(volatile unsigned long*)0x7F008100)
#define GPJCON (*(volatile unsigned long*)0x7F008120)

#define GPFCON (*(volatile unsigned long*)0x7F0080A0)
#define GPFDAT  (*((volatile unsigned long *)0x7F0080A4))

//����˿ڼĴ���
#define SPCON (*(volatile unsigned long*)0x7F0081A0)

//���õ��ƽ�����ƼĴ���
#define MIFPCON (*(volatile unsigned long*)0x7410800C)

//ͼ��ʱ����ƼĴ���
#define VIDTCON0 (*(volatile unsigned long*)0x77100010)
#define VIDTCON1 (*(volatile unsigned long*)0x77100014)
#define VIDTCON2 (*(volatile unsigned long*)0x77100018)

//ͼ�������ƼĴ���
#define VIDCON0 (*(volatile unsigned long*)0x77100000)
#define VIDCON1 (*(volatile unsigned long*)0x77100004)

//���ڵĸ�ʽ
#define WINCON0 (*(volatile unsigned long*)0x77100020)

//����0��С
#define VIDOSD0A (*(volatile unsigned long*)0x77100040)
#define VIDOSD0B (*(volatile unsigned long*)0x77100044)
#define VIDOSD0C (*(volatile unsigned long*)0x77100048)

//֡�����С
#define VIDW00ADD0B0 (*(volatile unsigned long*)0x771000A0)
#define VIDW00ADD1B0 (*(volatile unsigned long*)0x771000D0)
#define VIDW00ADD2 (*(volatile unsigned long*)0x77100100)

//����ģʽ�Ĵ���
#define DITHMODE (*(volatile unsigned long*)0x77100170)

#define VSPW 9 //vertical sync pulse width ����ֵΪ10������ʱ��ͼvspw+1=10
#define VBPD 1 //vbpd + 1 = 2 
#define VFPD 1 //vfpd + 1 = 2 
#define HSPW 40  //hspw + 1 = 41 
#define HBPD 1 //hbpd + 1 = 2
#define HFPD 1 //hfpd + 1 = 2
#define LINEVAL 271 //����
#define HOZVAL 479 //����
#define VCLK 1

unsigned int LCDBUFFER[544][960];

void lcd_init()
{
	/* 1.���ų�ʼ�� */
	GPICON |= 0xaaaaaaaa;
	GPJCON |= 0xaaaaaa;
	
	GPFCON &= ~(0x3<<28);
	GPFCON |= (0<<28);//LCD����ʹ����رա�����
	
	//��lcd����Ϊ��ͨģʽ
	MIFPCON &= ~(1<<3);
	
	//��LCD�˿���������ΪRGB I/F ģʽ
	SPCON    &= ~(0x3);
	SPCON |= (1<<0);
	

	VIDCON0 &= ~((3<<26) | (3<<17) | (0xff<<6)  | (3<<2));     /* RGB I/F, RGB Parallel format,  */
	VIDCON0 |= ((14<<6) | (1<<4) );      /* vclk== HCLK / (CLKVAL+1) = 133/15 = 9MHz */

	VIDCON1 &= ~(1<<7);   /* ��vclk���½��ػ�ȡ���� */
	VIDCON1 |= ((1<<6) | (1<<5));  /* HSYNC�ߵ�ƽ��Ч, VSYNC�ߵ�ƽ��Ч, */

	//VIDEO MAIN CONTROL 0��1 REGISTER  P494
	//VIDCON0 |= ((9<<6)|(0<<5)|(1<<4)|(0<<3)|(0<<2));  
	//VIDCON1 |= (VCLK << 7);

	/* 2.ʱ���ʼ��;    ʱ��ͬ��ʾ��ͼ��ͬ */
 	VIDTCON0 |= ((VSPW) | (VFPD<<8) | (VBPD<<16));
	VIDTCON1 |= ((HSPW) | (HFPD<<8) | (HBPD<<16));
	VIDTCON2 |= ((LINEVAL<<11) | (HOZVAL));
 
	//���ڵĸ�ʽ
	WINCON0 &= ~(0xf << 2);
	WINCON0 |= (0xb<<2); //unpacked 24 BPP (non-palletized R:8-G:8-B:8 )
	 
	//���ô���0�Ĵ�С
	VIDOSD0A = ((0<<11) | (0<<0)); //���Ͻ�����
	VIDOSD0B = ((479<<11) | (271<<0)); //���½�����
	VIDOSD0C = (480 * 272); //Eq. Height * Width (Number of Word)��ע
	
	/* 3.֡�����С */
	VIDW00ADD0B0 = ((((unsigned long)LCDBUFFER)>>24)<<24) | (((unsigned long)LCDBUFFER)&0xffffff);// �˼Ĵ�����[31:24]:ͼ�񻺳����ڴ��еĵ�ַ��A[31:24]
	VIDW00ADD1B0 = (((unsigned long)LCDBUFFER)&0xffffff + 960*544);  //�˼Ĵ�����[23:0]��ͼ��֡����Ľ�����ַ
	VIDW00ADD2 = (0<<13) | ((480*4)<<0);  //�ɲ��á���
	
	//����red dither bit ;green dither bit; blue dither bit ����λ
	//DITHMODE |= ((0<<5) | (0<<3) | (0<<1) | (1<<0));//�Ƿ����ô˼Ĵ�������ûɶӰ��

	
	//����ʾ
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
    	    //c = (unsigned int)(bmp[p] | (bmp[p+1]<<8) | (bmp[p+2]<<16)) ;	//����˾��������ʾ�κ���ɫ����������������11/22
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
	unsigned int *addr = (unsigned int *)fb_base_addr;//����������
 	for(x=0; x < xsize; x++)
     {   
		*(addr + x)= 0xFFFF00;
	 } 
} */
