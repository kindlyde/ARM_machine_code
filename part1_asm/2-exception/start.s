.text                  ;启动文件基本框架  2014/9/1
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
set_svc:
		mrs r0, cpsr
		bic r0, r0, #0x1f  ;将CPSR的[4:0]位清零
		orr r0, r0, #0xd3  ;屏蔽所有中断，并设	置为svc模式  I、F位置位
		msr cpsr, r0
		mov pc, lr