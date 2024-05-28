ENTRY(_start)

SECTIONS
{
	. = 0x00100000;

	kernel_start = .;
	.multiboot.data : {
		*(.multiboot.data)
	}

	multiboot_text_start = .;
	.multiboot.text : {
		*(.multiboot.text)
	}
	multiboot_end = .;

	. += 0xC0000000;
	.text ALIGN(4K) : AT (ADDR(.text) - 0xC0000000)
	{
		text_start = .;
		*(.text)
	}
	.rodata ALIGN(4K) : AT (ADDR(.rodata) - 0xC0000000)
	{
		rodata_start = .;
		*(.rodata)
		rodata_end = .;
	}
	.data ALIGN(4K) : AT (ADDR(.data) - 0xC0000000)
	{
		data_start = .;
		*(.data)
	}
	.bss ALIGN(4K) : AT (ADDR(.bss) - 0xC0000000)
	{
		bss_start = .;
		*(COMMON)
		*(.bss)
		*(.stack_bootstrap)
	}
	
	kernel_end = .;
}
