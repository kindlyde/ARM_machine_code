/* 
 * ���ڶ���������һЩ�궨��
 */ 
#define MMU_FULL_ACCESS     (3 << 10)    /* ����Ȩ�� */ 
#define MMU_DOMAIN          (0 << 5)    /* �����ĸ��� */
#define MMU_SPECIAL         (1 << 4)    /* ������1 */
#define MMU_CACHEABLE       (1 << 3)    /* cacheable */
#define MMU_BUFFERABLE      (1 << 2)    /* bufferable */
#define MMU_SECTION         (2)         /* ��ʾ���Ƕ������� */
#define MMU_SECDESC         (MMU_FULL_ACCESS | MMU_DOMAIN | MMU_SPECIAL | MMU_SECTION)
#define MMU_SECDESC_WB      (MMU_FULL_ACCESS | MMU_DOMAIN | MMU_SPECIAL | MMU_CACHEABLE | MMU_BUFFERABLE | MMU_SECTION)


void create_page_table(void)
{
    unsigned long *ttb = (unsigned long *)0x50000000;/* ҳ�������ڴ����ʼ��ַ����ҳ�����ַ�ڸ�λ�� */
    unsigned long vaddr, paddr;

    vaddr = 0xA0000000;
    paddr = 0x7f000000;
    *(ttb + (vaddr >> 20)) = (paddr & 0xFFF00000) | MMU_SECDESC;/* ������led��ӦGPIO�ĵ�ַӳ�� */

    vaddr = 0x50000000;
    paddr = 0x50000000;
    while (vaddr < 0x54000000)/* �����������ַ��ʵ�ʵ������ַ֮���ӳ��һһ��Ӧ�����ڴ����ӳ�� */
    {
        *(ttb + (vaddr >> 20)) = (paddr & 0xFFF00000) | MMU_SECDESC_WB;
        vaddr += 0x100000;
        paddr += 0x100000;
    }
    
}

void mmu_enable()
{
	__asm__(
    
    /*����TTB*/
    "ldr    r0, =0x50000000\n"                  
    "mcr    p15, 0, r0, c2, c0, 0\n"    /* ��һ��ҳ��������ַд��TTB �Ĵ�����CP15��C2��;��MMUʹ��ʱ�Զ�ȡ���� */
    
    /*������Ȩ�޼��*/
    "mvn    r0, #0\n"                   
    "mcr    p15, 0, r0, c3, c0, 0\n"    /* c3 */
    
    
   /*ʹ��MMU*/
    "mrc    p15, 0, r0, c1, c0, 0\n"   /* c1 */
    "orr    r0, r0, #0x0001\n"          
    "mcr    p15, 0, r0, c1, c0, 0\n"    
    : 
    : 
  );
}

void mmu_init()
{
	create_page_table();
    mmu_enable();
}
