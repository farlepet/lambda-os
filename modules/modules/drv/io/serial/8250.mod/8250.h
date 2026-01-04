#ifndef MOD_DRV_IO_SERIAL_8250_8250_H
#define MOD_DRV_IO_SERIAL_8250_8250_H

#include <stdint.h>

typedef enum serial_8250_portaddr {
    SERIAL_COM1 = 0x3F8,
    SERIAL_COM2 = 0x2F8,
    SERIAL_COM3 = 0x3E8,
    SERIAL_COM4 = 0x2E8
} serial_8250_portaddr_e;

enum serial_8250_reg {
    REG_8250_THR = 0, /** Transmit buffer */
    REG_8250_RBR = 0, /** Receive buffer */
    REG_8250_DLL = 0, /** Divisor latch - LSB */
    REG_8250_IER = 1, /** Interrupt Enable */
    REG_8250_DLH = 1, /** Divisor latch - MSB */
    REG_8250_IIR = 2, /** Interrupt Identification */
    REG_8250_FCR = 2, /** FIFO control */
    REG_8250_LCR = 3, /** Line control */
    REG_8250_MCR = 4, /** Modem control */
    REG_8250_LSR = 5, /** Line status */
    REG_8250_MSR = 6, /** Modem status */
    REG_8250_SR  = 7  /** Scratch register */
};

/**
 * \brief Initialize the serial port.
 * 
 * @param port which port to initialize
 */
void serial_8250_init(serial_8250_portaddr_e port, uint32_t baud);

#endif
