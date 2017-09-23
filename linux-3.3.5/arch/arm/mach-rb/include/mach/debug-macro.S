
#if 1
	.macro	addruart, rp, rv, tmp
	// ipq806x
	ldr     \rp, =0x16640000
	ldr     \rv, =0xf6640000
	.endm

	.macro	senduart, rd, rx
	@ Write the 1 character to UARTDM_TF
	str	\rd, [\rx, #0x70]
	.endm

	.macro	waituart, rd, rx
	@ check for TX_EMT in UARTDM_SR
	ldr	\rd, [\rx, #0x08]
	tst	\rd, #0x08
	bne	1002f
	@ wait for TXREADY in UARTDM_ISR
1001:	ldr	\rd, [\rx, #0x14]
	tst	\rd, #0x80
	beq 	1001b
1002:
	@ Clear TX_READY by writing to the UARTDM_CR register
	mov	\rd, #0x300
	str	\rd, [\rx, #0x10]
	@ Write 0x1 to NCF register
	mov 	\rd, #0x1
	str	\rd, [\rx, #0x40]
	@ UARTDM reg. Read to induce delay
	ldr	\rd, [\rx, #0x08]
	.endm

	.macro	busyuart, rd, rx
	.endm
#endif

