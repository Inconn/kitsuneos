.section .text
.extern test_user_fn_test
.global _jump_usermode
.type _jump_usermode, @function
_jump_usermode:
	add $4, %esp
	pop %ebx

	cli

	mov $(4 * 8) | 3, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	mov %esp, %eax
	push $(4 * 8) | 3
	push %ebx
	pushf
	pop %eax
	or $0x200, %eax
	push %eax
	push $(3 * 8) | 3

	push %ebx
	iret
.global _switch_to_user_mode
.type _switch_to_user_mode, @function
_switch_to_user_mode:
	cli

	mov $(4 * 8) | 3, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	mov %esp, %eax
	push $(4 * 8) | 3
	push %ebx
	pushf
	pop %eax
	or $0x200, %eax
	push %eax
	push $(3 * 8) | 3

	push $1f
	iret
1:
	ret
