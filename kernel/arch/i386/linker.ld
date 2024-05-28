ENTRY(_start)

SECTIONS
{
	. = 1M;

	kernel_start = .;
	.multiboot.data : {
		*(.multiboot.data)
	}

	multiboot_text_start = .;
	.multiboot.text : {
		*(.multiboot.text)
	}
	multiboot_end = .;

	.text BLOCK(4K) : ALIGN(4K)
	{
		text_start = .;
		*(.text)
	}
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		rodata_start = .;
		*(.rodata)
	}
	.data BLOCK(4K) : ALIGN(4K)
	{
		data_start = .;
		*(.data)
	}
	.bss BLOCK(4K) : ALIGN(4K)
	{
		bss_start = .;
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}
	
	kernel_end = .;
}
