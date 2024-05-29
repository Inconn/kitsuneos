.global isr_stub
isr_stub:
	call exception_handler
	iret
