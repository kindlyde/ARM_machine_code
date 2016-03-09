.text                              @9��21 
.global mem_init
mem_init:

	@makes DRAM Controller enter ��Config�� state. 
	ldr r0, =0x7E001004
	mov r1, #0x4
	str r1, [r0]
	
	@����ʱ�������ˢ�£���RAMоƬK4X51163PC
	@����оƬ�ֲ�����ˢ��ʱ��
	@�����ڴ�ˢ��ʱ����������������ڴ�оƬ��K4X51163PCˢ��ʱ��Ϊ7.8us,�����ݵ������ʱ�䣬
	@����Ӧ���ñȴ�����С�����ڽ���ˢ�¡�����������Ϊ1037�������õ�ˢ�µ�����ӦΪ<=1037,
	@�����ý϶̵�ˢ�����ڻ�ռ�ݶ�̬�ڴ�������������Դ����������ˢ������Ϊ1037����0x40d.
	@����г���ʽ������
	
	ldr r0, =0x7e001010
	ldr r1, =( 7800 / (1000000000 / 133000000) + 1)
	str r0, [r1]
	
	@CAS latency register Ǳ���ڼĴ�����RAS�͵�ƽ��CAS�͵�ƽ��ʱ�Ӹ�����
	ldr r0, =0x7e001014
	ldr r1, =(3 << 1)
	str r1, [r0]
	
	@DQSS register  RAMоƬ�ֲ� 0.75 - 1.25
	ldr r0, =0x7E001018 
	ldr r1, =0x1
	str r1, [r0]
	
	@MRD register (Mode register set cycle time) min 2
	ldr r0, =0x7E00101C
	ldr r1, = 0x2
	str r1, [r0]
	
	@RAS register (Row active time) min 45 max 70000
	ldr r0, =0x7E001020
	ldr r1, =( 45 / (1000000000 / 133000000) + 1)
	str r1, [r0]

    ldr r0, =0x7e001024   @t_RC�Ĵ���
    ldr r1, =( ( 68 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]

    ldr r0, =0x7e001028   @t_RCD�Ĵ���
    ldr r1, =( ( 23 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]

    ldr r0, =0x7e00102c   @t_RFC�Ĵ���
    ldr r1, =( ( 80 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]

    ldr r0, =0x7e001030   @t_RP�Ĵ���
    ldr r1, =( ( 23 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]

    ldr r0, =0x7e001034   @t_rrd�Ĵ���
    ldr r1, =( ( 15 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]

    ldr r0, =0x7e001038   @t_wr�Ĵ���
    ldr r1, =( ( 15 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]

    ldr r0, =0x7e00103c   @t_wtr�Ĵ���
    mov r1, #0x07
    str r1, [r0]

    ldr r0, =0x7e001040   @t_xp�Ĵ���
    mov r1, #0x02
    str r1, [r0]

    ldr r0, =0x7e001044   @t_xsr�Ĵ���
    ldr r1, =( ( 120 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]

    ldr r0, =0x7e001048   @t_esr�Ĵ���
    ldr r1, =( ( 120 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]
	
	ldr r0, =0x7e00100c   @�ڴ�������üĴ���
    ldr r1, =0x00010012   @���ÿ�����
    str r1, [r0]
	
    ldr r0, =0x7e00104c   @32λDRAM���ÿ��ƼĴ���:����ʱ���ڴ����͡��ڴ���ȡ�һϵ��ʱ��
    ldr r1, =0x0b45
    str r1, [r0]

	ldr r0, =0x7e001200   @Ƭѡ�Ĵ���  P1_chip_0_cfg  AXI address��
    ldr r1, =0x150f8
    str r1, [r0]

	ldr r0, =0x7e001304   @�û����üĴ���   USER_CONFIG REGISTER  DQS��
    mov r1, #0x0
    str r1, [r0]


	
	@DIRECT COMMAND register [22]\[19:18]
	
	ldr r0, =0x7E001008
	ldr r1, =0x000c0000
    str r1, [r0]
	
	ldr r1, =0x00000000
    str r1, [r0]

    ldr r1, =0x00040000
    str r1, [r0]

    ldr r1, =0x000a0000
    str r1, [r0]

    ldr r1, =0x00080032
    str r1, [r0]

    ldr r0, =0x7e001004
    mov r1, #0x0
    str r1, [r0]
	
check_dmc1_ready:
    
    ldr r0, =0x7e001000 
    ldr r1, [r0]
    mov r2, #0x3
    and r1, r1, r2
    cmp r1, #0x1
    bne check_dmc1_ready
    nop
    mov pc, lr
	
		
		
		
		
		