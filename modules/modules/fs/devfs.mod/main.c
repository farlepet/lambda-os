#include <lambda/mod/module.h>
/* TODO: This should be moved somewhere under lambda/ */
#include <err/error.h>

#include "devfs.h"

int mod_func(uint32_t func, void *data __unused) {
    switch(func) {
        case LAMBDA_MODFUNC_START:
            kdebug(DEBUGSRC_MODULE, ERR_INFO, "Initializing DevFS");
            devfs_init();
            break;
        case LAMBDA_MODFUNC_STOP:
            kdebug(DEBUGSRC_MODULE, ERR_INFO, "Stopping DevFS");
            break;
        default:
            return -1;
    }

    return 0;
}
