ENTRY (start)

SECTIONS
{
	. = 0x40000000;

	.text : ALIGN(0x1000) {
		*(.text)
	}

	. = 0x80000000;

	.data : ALIGN(0x1000) {
		*(.data)
	}

	.rodata : ALIGN(0x1000) {
		*(.rodata)
	}

	.bss : ALIGN(0x1000) {
		*(.bss)
	}

	.data.other : ALIGN(0x1000) {
		*.*
	}
}
