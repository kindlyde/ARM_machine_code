.text                  ;�����ļ��������  2014/9/1
.global _start
_start:
		b reset
		ldr pc,_undefined_instruction  ;ldr��װ��ָ��,pc����ת
		ldr pc,_software_interrupt 
		ldr pc,_prefetch_abort 
		ldr pc,_data_abort 
		ldr pc,_not_used 
		ldr pc,_irq 
		ldr pc,_fiq
		ldr pc,_reset
		
		
_undefined_instruction: .word undefined_instruction  ;����һ�����ڴ浥Ԫ������LDR��װ��ָ�������αָ��
_software_interrupt: .word software_interrupt
_prefetch_abort: .word prefetch_abort
_data_abort: .word data_abort   ;0x00000014
_not_used: .word not_used   ;ע���鿴�ֲ�0x00000014��ַδ�����κ��쳣
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
		bic r0, r0, #0x1f  ;��CPSR��[4:0]λ����
		orr r0, r0, #0xd3  ;���������жϣ�����	��Ϊsvcģʽ  I��Fλ��λ
		msr cpsr, r0
		mov pc, lr