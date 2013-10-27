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
	sbi 0x1b,7
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
	cbi 0x1b,7
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
	lds r24,conductionCounter
	lds r25,conductionCounter+1
	lds r18,conductionDelay
	lds r19,conductionDelay+1
	movw r20,r24
	subi r20,-1
	sbci r21,-1
	sts conductionCounter+1,r21
	sts conductionCounter,r20
	cp r24,r18
	cpc r25,r19
	brlo .L8
	rcall conductionOn
	rcall stopConductionTimer
	rcall startHoldTimer
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
	out 0x32,__zero_reg__
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
	rcall conductionOff
	out 0x2e,__zero_reg__
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
	.section	.text.measurePeriodLength,"ax",@progbits
.global	measurePeriodLength
	.type	measurePeriodLength, @function
measurePeriodLength:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	sbi 0x18,1
.L14:
	sbis 0x16,2
	rjmp .L14
.L23:
	sbic 0x16,2
	rjmp .L23
	out 0x2c+1,__zero_reg__
	out 0x2c,__zero_reg__
	in r24,0x2e
	ori r24,lo8(4)
	out 0x2e,r24
.L16:
	sbis 0x16,2
	rjmp .L16
.L22:
	sbic 0x16,2
	rjmp .L22
	out 0x2e,__zero_reg__
	in r24,0x2c
	in r25,0x2c+1
	sts periodLength+1,r25
	sts periodLength,r24
	cbi 0x18,1
	ret
	.size	measurePeriodLength, .-measurePeriodLength
	.section	.text.measureZCLength,"ax",@progbits
.global	measureZCLength
	.type	measureZCLength, @function
measureZCLength:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	sbi 0x18,1
.L31:
	sbis 0x16,2
	rjmp .L31
.L40:
	sbic 0x16,2
	rjmp .L40
.L39:
	sbis 0x16,2
	rjmp .L39
	out 0x2c+1,__zero_reg__
	out 0x2c,__zero_reg__
	in r24,0x2e
	ori r24,lo8(1)
	out 0x2e,r24
.L34:
	sbic 0x16,2
	rjmp .L34
	out 0x2e,__zero_reg__
	in r24,0x2c
	in r25,0x2c+1
	sts zcLength+1,r25
	sts zcLength,r24
	cbi 0x18,1
	ret
	.size	measureZCLength, .-measureZCLength
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
	sbi 0x1a,7
	out 0x1b,__zero_reg__
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
	rcall measurePeriodLength
	rcall measureZCLength
/* #APP */
 ;  136 "main.c" 1
	sei
 ;  0 "" 2
/* #NOAPP */
	ldi r24,lo8(32)
	rcall usiTwiSlaveInit
	ldi r28,lo8(44)
	ldi r29,lo8(1)
	ldi r16,lo8(-26)
	ldi r17,0
	ldi r24,lo8(-36)
	mov r14,r24
	mov r15,__zero_reg__
.L60:
	rcall usiTwiDataInReceiveBuffer
	tst r24
	breq .L60
	rcall usiTwiReceiveByte
	cpi r24,lo8(1)
	brne .L50
	sts conductionDelay+1,r15
	sts conductionDelay,r14
	rjmp .L60
.L50:
	cpi r24,lo8(2)
	brne .L52
	sts conductionDelay+1,r17
	sts conductionDelay,r16
	rjmp .L60
.L52:
	cpi r24,lo8(3)
	brne .L53
	sts conductionDelay+1,r29
	sts conductionDelay,r28
	rjmp .L60
.L53:
	cpi r24,lo8(4)
	brne .L54
	lds r24,periodLength+1
	rcall usiTwiTransmitByte
	lds r24,periodLength
	rjmp .L61
.L54:
	cpi r24,lo8(5)
	brne .L60
	lds r24,zcLength
	lds r25,zcLength+1
	mov r24,r25
	rcall usiTwiTransmitByte
	lds r24,zcLength
	lds r25,zcLength+1
.L61:
	rcall usiTwiTransmitByte
	rjmp .L60
	.size	main, .-main
	.comm	zcLength,2,1
.global	periodLength
	.section	.bss.periodLength,"aw",@nobits
	.type	periodLength, @object
	.size	periodLength, 2
periodLength:
	.zero	2
.global	holdDelay
	.section	.data.holdDelay,"aw",@progbits
	.type	holdDelay, @object
	.size	holdDelay, 2
holdDelay:
	.word	75
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
	.word	290
	.ident	"GCC: (GNU) 4.7.0"
.global __do_copy_data
.global __do_clear_bss
