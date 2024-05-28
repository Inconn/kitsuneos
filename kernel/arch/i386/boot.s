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
stack_bottom:
.skip 16384
stack_top:

.section .bss, "aw", @nobits
.align 4096
.global _boot_page_directory
_boot_page_directory:
.skip 4096
.global _boot_page_table1
_boot_page_table1:
.skip 4096

.section .multiboot.text, "a"
.global _start
.type _start, @function
_start:
	mov $stack_top, %esp
	and 16, %esp

	push %ebx
	push %eax

	call _init

	call kernel_main

	cli
1:	hlt
	jmp 1b

//.size _start, . - _start
