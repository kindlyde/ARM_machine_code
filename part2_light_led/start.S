.text                 
.global _start
_start:
		b reset
		ldr pc,_undefined_instruction 
		ldr pc,_software_interrupt 
		ldr pc,_prefetch_abort 
		ldr pc,_data_abort 
		ldr pc,_not_used 
		ldr pc,_irq 
		ldr pc,_fiq
		ldr pc,_reset
		
		
_undefined_instruction: .word undefined_instruction  
_software_interrupt: .word software_interrupt
_prefetch_abort: .word prefetch_abort
_data_abort: .word data_abort   
_not_used: .word not_used 
_irq: .word irq
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
		bl set_peri_port
		bl disable_watchdog
		bl disable_interrupt
		bl disable_mmu
		bl light_led
		
		
set_svc:
		mrs r0, cpsr
		bic r0, r0, #0x1f
		orr r0, r0, #0xd3
		msr cpsr, r0
		mov pc, lr
		
set_peri_port:
		ldr r0, =0x70000000
		orr  r0, r0, #0x13
		mcr p15,0,r0,c15,c2,4
		mov pc, lr
		
#define pWTCON 0x7e004000
disable_watchdog:
		ldr r0, =pWTCON


		mov r1,#0x0
		str r1, [r0]
		mov pc,lr
		
disable_interrupt:
		mvn r1,#0x0
		ldr r0,=0x71200014  
		str r1,[r0]
		
		ldr r0,=0x71300014
		str r1,[r0]
		mov pc,lr
		
disable_mmu:
		mcr p15,0,r0,c7,c7,0
		mrc p15,0,r0,c1,c0,0
		bic r0, r0, #0x00000007
		mcr p15,0,r0,c1,c0,0
		mov pc, lr
		
#define GPMCON 0x7F008820
#define GPMDAT 0x7F008824
light_led:
		ldr r0, =GPMCON
		ldr r1, =0x1111  @设置输出功能
		str r1, [r0]
		
		ldr r0, =GPMDAT
		ldr r1, =0x30	@低电平二极管导通
		str r1, [r0]
		mov pc, lr
		
