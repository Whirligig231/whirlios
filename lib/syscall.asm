	.global _syscall

; int syscall(int ax, int bx, int cx, int dx)
_syscall:
	push bp
	mov bp, sp
	push si ; Callee-save
	push di ; Callee-save

	; Load registers from arguments
	mov ax, [bp + 4]
	mov bx, [bp + 6]
	mov cx, [bp + 8]
	mov dx, [bp + 10]

	; Interrupt 0x21 is used for system calls
	int #0x21

	pop di
	pop si
	pop bp
	ret
