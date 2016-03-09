.text                  ;启动文件基本框架  2014/9/13  --关闭MMU	
.global _start
_start:
		b reset
		ldr pc,_undefined_instruction  ;ldr做装载指令,pc做跳转
		ldr pc,_software_interrupt 
		ldr pc,_prefetch_abort 
		ldr pc,_data_abort 
		ldr pc,_not_used 
		ldr pc,_irq 
		ldr pc,_fiq
		ldr pc,_reset
		
		
_undefined_instruction: .word undefined_instruction  ;定义一个字内存单元，以让LDR做装载指令而不是伪指令
_software_interrupt: .word software_interrupt
_prefetch_abort: .word prefetch_abort
_data_abort: .word data_abort   ;0x00000014
_not_used: .word not_used   ;注：查看手册0x00000014地址未代表任何异常
_irq: .word irq             ;0x00000018
_fiq: .word fiq
_reset: .word reset

undefined_instruction:
		nop
software_interrupt:
		nop
prefetch_abort:
		nop
data_abort:
		nop
not_used:
		nop
irq:
		nop
fiq:
		nop
reset:
		bl set_svc
		bl disable_watchdog
		bl disable_interrupt
		bl disable_mmu
		
set_svc:
		mrs r0, cpsr
		bic r0, r0, #0x1f  ;将CPSR的[4:0]位清零
		orr r0, r0, #0xd3  ;屏蔽所有中断，并设	置为svc模式  I、F位置位
		msr cpsr, r0
		mov pc, lr
		
#define pWTCON 0x7e004000	;宏定义定义寄存器地址
disable_watchdog:
		ldr r0, =pWTCON
		mov r1,#0x0		;使用MOV装载至寄存器，通过寄存器对地址单元写入数据。。
		str r1, [r0]
		mov pc,lr
		
disable_interrupt:
		mvn r1,0x0      ;数据取反后传送，
		ldr r0,=0x71200014  ;vic0intenclear 中断屏蔽寄存器，写1有效
		str r1,[r0]
		
		ldr r0,=0x71300014  ;vic1intenclear 中断屏蔽寄存器，写1有效
		str r1,[r0]
		mov pc,lr
		
disable_mmu:
		mcr p15,0,r0,c7,c7,0	;使cache中的数据失效
		mrc p15,0,r0,c1,c0,0	;读取c1中的值--control register
		bic r0, r0, #0x00000007 	;[2:0]清零失能
		mcr p15,0,r0,c1,c0,0	;写入
		mov pc, lr
		