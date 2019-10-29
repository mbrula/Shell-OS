GLOBAL cpu_vendor
GLOBAL write_port
GLOBAL read_port
GLOBAL force_timer_tick
GLOBAL atom_swap

section .text

; void lock_sem(uint64_t * dir, uint64_t value)
atom_swap:
	xchg [rdi], rsi
	ret

; void write_port(char port, char reg)
write_port:
	mov al, sil
	mov dx, di
	out dx, al
	ret

; char read_port(char port)
read_port:
	mov dx, di
	in al, dx
	ret

; void force_timer_tick()
force_timer_tick:
	int 20h
	ret

cpu_vendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret
