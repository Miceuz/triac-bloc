	.file	"main.c"
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__SREG__ = 0x3f
__tmp_reg__ = 0
__zero_reg__ = 1
	.section	.text.stopConductionTimer,"ax",@progbits
.global	stopConductionTimer
	.type	stopConductionTimer, @function
stopConductionTimer:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	out 0x39,__zero_reg__
	out 0x33,__zero_reg__
	sts conductionCounter+1,__zero_reg__
	sts conductionCounter,__zero_reg__
	ret
	.size	stopConductionTimer, .-stopConductionTimer
	.section	.text.conductionOn,"ax",@progbits
.global	conductionOn
	.type	conductionOn, @function
conductionOn:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	sbi 0x18,0
	ret
	.size	conductionOn, .-conductionOn
	.section	.text.conductionOff,"ax",@progbits
.global	conductionOff
	.type	conductionOff, @function
conductionOff:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	cbi 0x18,0
	ret
	.size	conductionOff, .-conductionOff
	.section	.text.conduct,"ax",@progbits
.global	conduct
	.type	conduct, @function
conduct:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	rjmp conductionOn
	.size	conduct, .-conduct
	.section	.text.startHoldTimer,"ax",@progbits
.global	startHoldTimer
	.type	startHoldTimer, @function
startHoldTimer:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	sbi 0xc,1
	lds r24,holdDelay
	lds r25,holdDelay+1
	out 0x2a+1,r25
	out 0x2a,r24
	in r24,0x2e
	ori r24,lo8(8)
	out 0x2e,r24
	in r24,0x2e
	ori r24,lo8(1)
	out 0x2e,r24
	ret
	.size	startHoldTimer, .-startHoldTimer
	.section	.text.stopHoldTimer,"ax",@progbits
.global	stopHoldTimer
	.type	stopHoldTimer, @function
stopHoldTimer:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	out 0x2e,__zero_reg__
	ret
	.size	stopHoldTimer, .-stopHoldTimer
	.section	.text.__vector_6,"ax",@progbits
.global	__vector_6
	.type	__vector_6, @function
__vector_6:
	push r1
	push r0
	in r0,__SREG__
	push r0
	clr __zero_reg__
	push r18
	push r19
	push r20
	push r21
	push r22
	push r23
	push r24
	push r25
	push r26
	push r27
	push r30
	push r31
/* prologue: Signal */
/* frame size = 0 */
/* stack size = 15 */
.L__stack_usage = 15
	rcall conductionOff
	out 0x2e,__zero_reg__
/* epilogue start */
	pop r31
	pop r30
	pop r27
	pop r26
	pop r25
	pop r24
	pop r23
	pop r22
	pop r21
	pop r20
	pop r19
	pop r18
	pop r0
	out __SREG__,r0
	pop r0
	pop r1
	reti
	.size	__vector_6, .-__vector_6
	.section	.text.__vector_11,"ax",@progbits
.global	__vector_11
	.type	__vector_11, @function
__vector_11:
	push r1
	push r0
	in r0,__SREG__
	push r0
	clr __zero_reg__
	push r18
	push r19
	push r20
	push r21
	push r22
	push r23
	push r24
	push r25
	push r26
	push r27
	push r30
	push r31
/* prologue: Signal */
/* frame size = 0 */
/* stack size = 15 */
.L__stack_usage = 15
	lds r18,conductionCounter
	lds r19,conductionCounter+1
	lds r20,conductionDelay
	lds r21,conductionDelay+1
	movw r24,r18
	adiw r24,1
	sts conductionCounter+1,r25
	sts conductionCounter,r24
	cp r18,r20
	cpc r19,r21
	brlo .L9
	rcall conductionOn
	rcall stopConductionTimer
	rcall startHoldTimer
	rjmp .L8
.L9:
	cpi r24,-105
	cpc r25,__zero_reg__
	brlo .L8
	cbi 0x18,1
.L8:
/* epilogue start */
	pop r31
	pop r30
	pop r27
	pop r26
	pop r25
	pop r24
	pop r23
	pop r22
	pop r21
	pop r20
	pop r19
	pop r18
	pop r0
	out __SREG__,r0
	pop r0
	pop r1
	reti
	.size	__vector_11, .-__vector_11
	.section	.text.startConductionTimer,"ax",@progbits
.global	startConductionTimer
	.type	startConductionTimer, @function
startConductionTimer:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	in r24,0x39
	ori r24,lo8(1)
	out 0x39,r24
	in r24,0x33
	ori r24,lo8(1)
	out 0x33,r24
	sts conductionCounter+1,__zero_reg__
	sts conductionCounter,__zero_reg__
	ret
	.size	startConductionTimer, .-startConductionTimer
	.section	.text.__vector_1,"ax",@progbits
.global	__vector_1
	.type	__vector_1, @function
__vector_1:
	push r1
	push r0
	in r0,__SREG__
	push r0
	clr __zero_reg__
	push r18
	push r19
	push r20
	push r21
	push r22
	push r23
	push r24
	push r25
	push r26
	push r27
	push r30
	push r31
/* prologue: Signal */
/* frame size = 0 */
/* stack size = 15 */
.L__stack_usage = 15
	sbi 0x18,1
	rcall startConductionTimer
/* epilogue start */
	pop r31
	pop r30
	pop r27
	pop r26
	pop r25
	pop r24
	pop r23
	pop r22
	pop r21
	pop r20
	pop r19
	pop r18
	pop r0
	out __SREG__,r0
	pop r0
	pop r1
	reti
	.size	__vector_1, .-__vector_1
	.section	.text.setupZCInterrupt,"ax",@progbits
.global	setupZCInterrupt
	.type	setupZCInterrupt, @function
setupZCInterrupt:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	in r24,0x35
	ori r24,lo8(3)
	out 0x35,r24
	in r24,0x3b
	ori r24,lo8(64)
	out 0x3b,r24
	ret
	.size	setupZCInterrupt, .-setupZCInterrupt
	.section	.text.startup.main,"ax",@progbits
.global	main
	.type	main, @function
main:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	in r24,0x17
	ori r24,lo8(3)
	out 0x17,r24
	sbi 0x18,0
	ldi r18,lo8(159999)
	ldi r24,hi8(159999)
	ldi r25,hlo8(159999)
	1: subi r18,1
	sbci r24,0
	sbci r25,0
	brne 1b
	rjmp .
	nop
	cbi 0x18,0
	cbi 0x18,1
	ldi r24,lo8(dummy)
	ldi r25,hi8(dummy)
	sts USI_Slave_register_buffer+1,r25
	sts USI_Slave_register_buffer,r24
	ldi r24,lo8(32)
	rcall USI_I2C_Init
	rcall setupZCInterrupt
/* #APP */
 ;  111 "main.c" 1
	sei
 ;  0 "" 2
/* #NOAPP */
.L23:
	lds r24,dummy
	cpi r24,lo8(1)
	brne .L15
	sbi 0x18,0
	rjmp .L24
.L15:
	cpi r24,lo8(2)
	brne .L17
	sbi 0x18,1
	cbi 0x18,0
	rjmp .L16
.L17:
	cpi r24,lo8(3)
	brne .L18
	sbi 0x18,0
	sbi 0x18,1
	rjmp .L16
.L18:
	cbi 0x18,0
.L24:
	cbi 0x18,1
.L16:
	lds r24,conductionCounter
	lds r25,conductionCounter+1
	sbiw r24,11
	brlo .L23
	rcall conductionOff
	rjmp .L23
	.size	main, .-main
.global	dummy
	.section	.data.dummy,"aw",@progbits
	.type	dummy, @object
	.size	dummy, 1
dummy:
	.byte	4
.global	holdDelay
	.section	.data.holdDelay,"aw",@progbits
	.type	holdDelay, @object
	.size	holdDelay, 2
holdDelay:
	.word	5000
.global	conductionCounter
	.section	.bss.conductionCounter,"aw",@nobits
	.type	conductionCounter, @object
	.size	conductionCounter, 2
conductionCounter:
	.zero	2
.global	conductionDelay
	.section	.data.conductionDelay,"aw",@progbits
	.type	conductionDelay, @object
	.size	conductionDelay, 2
conductionDelay:
	.word	300
	.ident	"GCC: (GNU) 4.7.0"
.global __do_copy_data
.global __do_clear_bss
