.align 4
.arm
.section	.itcm,"ax",%progbits

.equ REG_BASE,	0x04000000
.equ REG_IME,	0x04000208


#include "../ichflysettings.h"

@========== Exception code ====================
.pool
	
.align 4
.arm
.section	.vectors,"ax",%progbits
	
.global irqhandler2
irqhandler2:
b	inter_Reset + 0x01FF8000
b	inter_undefined + 0x01FF8000
b	inter_swi + 0x01FF8000
b	inter_fetch + 0x01FF8000
b	inter_data + 0x01FF8000
b	inter_res + 0x01FF8000
b	inter_irq + 0x01FF8000
b	inter_fast + 0x01FF8000
b	inter_res2 + 0x01FF8000

somethingfailed:

inter_Reset:
inter_res:
inter_fast:
inter_res2:
	str sp,[pc, #0x10]
	str lr,[pc, #0x10]
	ldr sp, =failcpphandler
	ldr lr, =exHandler
	str sp,[lr]
	b dointerwtf
	



.global savedsp
savedsp:
	.word 0
.global savedlr
savedlr:
	.word 0

_exMain_tmpPuplain:
	.word 0
.global spirq
spirq:
	.word __sp_irq
	
.global SPtemp
SPtemp: @lol not realy
	.word 0

@irq handler start
inter_irq:
	stmfd  SP!, {R0-R3,R12,LR}		@save registers to SP_irq
	
	ldr	r1,=0x36333333		@MPU is set free to write everything except this area (vectors)
	mcr	p15, 0, r1, c5, c0, 2
	
	bl IntrMain				@serve IFs that are enabled on IE (NDS IO), r1 alternatively has the remaining IFs from vector table
							@BL is faster than BLX and IntrMain is ARM code (otherwise this would crash inmediately)
							
	
	mov	r0, #0x04000000		@03FFFFFC || 03007FFC(mirror) is user handler
	ldr	r1, [r0, #0x214]	@get IF
	
	ldr	r2, =anytimejmpfilter
	ldr r2, [r2]
	ands r1,r1,r2 			@bankedIE (NDS enabled hardware) & IF
	
	BEQ	irqexitdirect		@ IF > 0 ? continue serving interrupts
	
gba_handler:
	ldr	r1, =0x06300033        @prepare for GBA IRQ handler
	mcr	p15, 0, r1, c5, c0, 2

@acquire and jump to usr irq handler and go back right after
	add    LR,PC,#0            @retadr for USER handler
	ldr    PC,[R0, #-0x4]      @jump to [03FFFFFC] USER handler
	b irqexit
	
irqexitdirect:	
	ldr	r1, =0x06300033        @prepare for GBA IRQ handler
	mcr	p15, 0, r1, c5, c0, 2

irqexit:
	ldmfd  SP!, {R0-R3,R12,LR} @restore registers from SP_irq  
	subs   PC,LR, #0x4         @return from IRQ (PC=LR-4, CPSR=SPSR)

@irq handler end


.global spsvc
spsvc:
	.word __sp_svc
	
inter_swi:
	@ change the PU to nds mode
	ldr	SP,=0x36333333	@ see cpumg.cpp for meanings protections
	mcr	p15, 0, SP, c5, c0, 2
	ldr	SP, =exRegs
	
	str	lr, [SP, #(15 * 4)]	@ save r15 (lr is r15)
	
	@ save the registres 0->12
	stmia	SP, {r0-r12}
	
	@ jump into the personal handler
	ldr	r1, =exHandlerswi
	ldr	r1, [r1]
	
	
	ldr	sp, =spsvc	@ use the new stack
	ldr sp, [sp]
	
	blx	r1 @ichfly change back if possible
	
	
	ldr	r1, =spsvc	@save old stack
	str sp, [r1]
	
	@ restore the registres 0->12
	ldr	lr, =exRegs
	ldmia	lr, {r0-r12}
	
	ldr	lr, [lr, #(15 * 4)] 
	
	subs    pc, lr, #0 @ichfly this is not working	




inter_fetch: @ break function todo

	subs    lr, lr, #0x8000000
	ldr		sp,=rom
	ldr		sp,[sp]
	@thisi4:	
	@ldr r2, [pc,#(rom - thisi4 -8)]
	add		lr,lr,sp
	subs    pc, lr, #4



inter_undefined:
@change the PU to nds mode
	push {r0-r12,lr}
	ldr	r12, =exRegs
	str sp, [r12,#(4*13)]
	
	ldr	SP,=0x36333333	
	mcr	p15, 0, SP, c5, c0, 2	
	
	@ restore nds undef stack context
	ldr	sp, =__sp_undef
	ldr sp, [sp]

	@coto : jump to handler,  make for arm9 is set for -marm ARM code (with interchange THUMB). If you switch back to -mthumb it will be slower & this will have to be BLX
	BL exHandlerundifined

	@save nds swi stack context
	ldr	r1, =__sp_undef
	str sp, [r1]	

	@restore the GBA frame context (this LR is reusable, r13 no because we need to restore it and r15 is the on-exit frame address) 
	ldr	sp, =exRegs
	ldr sp, [sp,#(4*13)]
	pop {r0-r12,lr}
	
	subs	pc, lr, #4



dointerwtf:

inter_data:
	

	
	ldr	SP, =exRegs

	str	lr, [SP, #(15 * 4)]	@ save r15 (lr is r15)


	
	@ save the registres 0->12
	stmia	SP!, {r0-r12}
	
	@ change the PU to nds mode
	ldr	r7,=0x36333333	@ see cpumg.cpp for meanings protections
	mcr	p15, 0, r7, c5, c0, 2

	

	MRS r5,spsr

	
	mov r6,SP
#ifndef directcpu
	ldr r0, =BIOSDBG_SPSR
	str	r1, [r0]	@ charge le SPSR
#endif

	@ change the mode  @ on change de mode (on se mets dans le mode qui était avant l'exception)
	mrs	r3, cpsr
	bic	r4, r3, #0x1F
	and	r1, r5, #0x1F
	
	
	cmp r1,#0x10 @ichfly user is system
	moveq r1,#0x1F
	
	orr	r4, r4, r1
	msr	cpsr, r4	@ hop, c'est fait
	
	stmia r6, {r13-r14} @save the registrers	@ on sauvegarde les registres bankés (r13 et r14)
	msr	cpsr, r3	@ back to normal mode @ on revient au mode "normal"



#ifdef directcpu
	
#ifndef gba_handel_IRQ_correct
	BIC r5,r5,#0x80
#endif
	
	@ldr r1,=exRegs
	sub r1,SP,#13 * 4
	
	lsls r2,r5, #0x1A
	BMI itisTHUMB
	
itisARM:

	ldr r0, [LR, #-0x8]
	ldr	sp, =__sp_undef	@ use the new stack
	BL emuInstrARM 	@ is emuInstrARM(u32 opcode, u32 *R)
	B exitdirectcpu
itisTHUMB:
	ldrh r0, [LR,#-0x8]
	sub LR, #0x2
	str LR, [r1, #15*4]
	ldr	sp, =__sp_undef	@ use the new stack
	BL emuInstrTHUMB @ is emuInstrTHUMB(u16 opcode, u32 *R)
exitdirectcpu:
#else

	ldr	sp, =__sp_undef	@ use the new stack

	@ jump into the personal handler @ on appelle l'handler perso
	ldr	r12, =exHandler
	ldr	r12, [r12]


	blx	r12
	
#endif      

	

	@ restore SPSR @ on restaure les bankés
#ifndef directcpu
	ldr r0, =BIOSDBG_SPSR
	ldr	r5, [r0]	@ charge le SPSR
#endif
	MSR spsr,r5

	@change mode to the saved mode @ on change de mode (on se mets dans le mode qui était avant l'exception)
	mrs	r3, cpsr
	bic	r4, r3, #0x1F
	and	r5, r5, #0x1F
	
	cmp r5,#0x10 @ichfly user is system
	moveq r5,#0x1F	
	
	orr	r4, r4, r5
	msr	cpsr, r4	@ hop, c'est fait
	ldmia r6, {r13-r14}	@restor r13 and r14  @ on restaure les registres bankés (r13 et r14). /!\ N'allons pas croire qu'on les a restauré dans notre contexte: en effet, on a changé de mode là !
	msr	cpsr, r3	@chagne to mode "normal"@ on revient au mode "normal"
	



	
	ldr	SP, =0x06300033
	mcr	p15, 0, SP, c5, c0, 2



	@restore r0-r12 easy
	sub lr,r6,#13 * 4 @ldr	lr, =exRegs
	ldmia	lr, {r0-r12}
			
	@restore PU from the handler @ restaure la protection du PU, comme voulue par l'handler perso
	

	
	
	@my speedup code
	ldr	lr, [lr, #(15 * 4)] 
	
	subs    pc, lr, #4	
	
	.section	.dtcm,"ax",%progbits

.global BIOSDBG_SPSR
BIOSDBG_SPSR:
	.word 0

_exMain_tmpPu:
	.word 0
			
	.global exHandler
exHandler:
	.word	0
	
	.global exHandlerundifined
exHandlerundifined:
	.word	0

	.global exHandlerswi
exHandlerswi:
	.word	0
			
	.global exPuProtection
exPuProtection:
	.word 0

.global MPUPERMBACKUPSET_SWI	@swi mpu save sleep mode
MPUPERMBACKUPSET_SWI:
	.word 0x00000000

	.global exRegs
exRegs:
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
		
	.pool
	.end
