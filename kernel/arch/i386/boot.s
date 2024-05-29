.set ALIGN,	1<<0
.set MEMINFO,	1<<1
.set FLAGS,	ALIGN | MEMINFO
.set MAGIC,	0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot.data, "aw"
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .stack_bootstrap
.align 16
.global stack_bottom
stack_bottom:
.skip 16384
.global stack_top
stack_top:

.section .bss, "aw", @nobits
.align 4096
.global _kernel_page_directory
_kernel_page_directory:
.skip 4096
.global _boot_page_table1
_boot_page_table1:
.skip 4096

.section .multiboot.text, "a"
.global _start
.type _start, @function
_start:
	mov $(_boot_page_table1 - 0xC0000000), %edi
	
	mov %esi, 0
1:
	cmpl $kernel_start, %esi
	jl 2f
	cmpl $(kernel_end - 0xC0000000), %esi
	jge 3f

	movl %esi, %edx
	orl $0x001, %edx
	cmp $(text_start - 0xC0000000), %esi
	jl 6f
	cmp $(rodata_end - 0xC0000000), %esi
	jge 6f
	jmp 5f
6:

	orl $0x002, %edx

5:

	movl %edx, (%edi)	

2:
	addl $4096, %esi
	addl $4, %edi
	loop 1b
3:
	movl $(0x000B8000 | 0x003), _boot_page_table1 - 0xC0000000 + 1023 * 4

	movl $(_boot_page_table1 - 0xC0000000 + 0x003), _kernel_page_directory - 0xC0000000 + 0
	movl $(_boot_page_table1 - 0xC0000000 + 0x003), _kernel_page_directory - 0xC0000000 + 768 * 4

	movl $(_kernel_page_directory - 0xC0000000), %ecx
	movl %ecx, %cr3

	movl %cr0, %ecx
	orl $0x80010001, %ecx
	movl %ecx, %cr0

	lea 4f, %ecx
	jmp *%ecx

.section .text

4:
	movl $0, _kernel_page_directory + 0

	movl %cr3, %ecx
	movl %ecx, %cr3

	mov $stack_top, %esp

	call _init

	call kernel_main

	cli
1:	hlt
	jmp 1b

//.size _start, . - _start
