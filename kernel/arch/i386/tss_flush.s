.global tss_flush
.type tss_flush, @function
tss_flush:
	mov $(5 * 8) | 3, %ax

	ltr %ax
	ret
