.text                              @9、21 
.global mem_init
mem_init:

	@makes DRAM Controller enter ‘Config’ state. 
	ldr r0, =0x7E001004
	mov r1, #0x4
	str r1, [r0]
	
	@设置时间参数（刷新），RAM芯片K4X51163PC
	@根据芯片手册设置刷新时间
	@设置内存刷新时间参数，根据所用内存芯片：K4X51163PC刷新时间为7.8us,即数据的最长保存时间，
	@所以应设置比此数较小的周期进行刷新。计算的最长周期为1037，即设置的刷新的周期应为<=1037,
	@又设置较短的刷新周期会占据动态内存控制器过多的资源。所以设置刷新周期为1037，即0x40d.
	@亦可列出算式：如下
	
	ldr r0, =0x7e001010
	ldr r1, =( 7800 / (1000000000 / 133000000) + 1)
	str r0, [r1]
	
	@CAS latency register 潜伏期寄存器（RAS低电平到CAS低电平的时钟个数）
	ldr r0, =0x7e001014
	ldr r1, =(3 << 1)
	str r1, [r0]
	
	@DQSS register  RAM芯片手册 0.75 - 1.25
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

    ldr r0, =0x7e001024   @t_RC寄存器
    ldr r1, =( ( 68 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]

    ldr r0, =0x7e001028   @t_RCD寄存器
    ldr r1, =( ( 23 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]

    ldr r0, =0x7e00102c   @t_RFC寄存器
    ldr r1, =( ( 80 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]

    ldr r0, =0x7e001030   @t_RP寄存器
    ldr r1, =( ( 23 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]

    ldr r0, =0x7e001034   @t_rrd寄存器
    ldr r1, =( ( 15 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]

    ldr r0, =0x7e001038   @t_wr寄存器
    ldr r1, =( ( 15 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]

    ldr r0, =0x7e00103c   @t_wtr寄存器
    mov r1, #0x07
    str r1, [r0]

    ldr r0, =0x7e001040   @t_xp寄存器
    mov r1, #0x02
    str r1, [r0]

    ldr r0, =0x7e001044   @t_xsr寄存器
    ldr r1, =( ( 120 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]

    ldr r0, =0x7e001048   @t_esr寄存器
    ldr r1, =( ( 120 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]
	
	ldr r0, =0x7e00100c   @内存控制配置寄存器
    ldr r1, =0x00010012   @配置控制器
    str r1, [r0]
	
    ldr r0, =0x7e00104c   @32位DRAM配置控制寄存器:读延时、内存类型、内存宽度、一系列时钟
    ldr r1, =0x0b45
    str r1, [r0]

	ldr r0, =0x7e001200   @片选寄存器  P1_chip_0_cfg  AXI address？
    ldr r1, =0x150f8
    str r1, [r0]

	ldr r0, =0x7e001304   @用户配置寄存器   USER_CONFIG REGISTER  DQS？
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
	
		
		
		
		
		