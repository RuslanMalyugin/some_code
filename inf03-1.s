	.text
	.global	summ
summ:
	mov	r3, #0
	push {r4}

.loop:
	cmp	r3, r1
	bge	.end
	ldr	r4, [r2, r3, lsl #2]
	add	r3, r3, #1
	add	r0, r0, r4
	b	.loop

.end:
	pop {r4}
	bx lr

