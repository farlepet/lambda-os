#include <lambda/mod/module.h>
/* TODO: This should be moved somewhere under lambda/ */
#include <err/error.h>

#include "8250.h"

#define DEFAULT_BAUD 115200

int mod_func(uint32_t func, void *data __unused) {
    switch(func) {
        case LAMBDA_MODFUNC_START:
            kerror(ERR_BOOTINFO, "Initializing 8250\n");
            serial_8250_init(SERIAL_COM1, DEFAULT_BAUD);
            break;
        case LAMBDA_MODFUNC_STOP:
            kerror(ERR_BOOTINFO, "BIG DETEST\n");
            break;
        default:
            return -1;
    }

    return 0;
}
