#include <intr/intr.h>

#include <arch/io/ioport.h>

#include "modules/tty/tty.mod/tty.h"
#include "8250.h"

tty_handle_t *tty_serial_0;

static int _tty_serial_in(tty_handle_t *tty, uint8_t ch);

void serial_8250_init(serial_8250_portaddr_e port, uint32_t baud) {
    uint16_t divisor = (115200 / baud);

	outb(port + REG_8250_IER, 0x00);
	outb(port + REG_8250_LCR, 0x80);
	outb(port + REG_8250_DLL, (uint8_t)divisor);
	outb(port + REG_8250_DLH, (uint8_t)(divisor >> 8));
	outb(port + REG_8250_LCR, 0x03);
	outb(port + REG_8250_FCR, 0xC7);
	outb(port + REG_8250_MCR, 0x0B);
	outb(port + REG_8250_IER, 0x01);

	/*set_interrupt(INT_SERIALA, &serial_interrupt);
	set_interrupt(INT_SERIALB, &serial_interrupt);
	enable_irq(4);
	enable_irq(3);*/

	/*add_input_dev(&serial_dev, IDRIVER_SERIAL, "ser", 1, 0);
	serial_dev.iev_buff = &_serial_buff;*/

	/* TODO: Support multiple ports */
	tty_serial_0 = tty_create();

	tty_serial_0->data   = (void *)port;
	tty_serial_0->dev_in = _tty_serial_in;
}

static int _tty_serial_in(tty_handle_t *tty, uint8_t ch) {
	uint16_t port = (uint16_t)(uintptr_t)tty->data;
	while(!(inb(port + REG_8250_LSR) & 0x20));
	outb(port + REG_8250_THR, ch);
	return 0;
}
