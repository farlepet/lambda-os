#include <lambda/mod/module.h>
/* TODO: This should be moved somewhere under lambda/ */
#include <err/error.h>
#include <mod/module.h>
#include <time/time.h>

static void _mod_thread(void *data __unused) {
    for(;;) {
        delay(10);
        kdebug(DEBUGSRC_MODULE, "_mod_thread: Tick!");
    }
}

int mod_func(uint32_t func, void *data) {
    switch(func) {
        case LAMBDA_MODFUNC_START:
            kerror(ERR_BOOTINFO, "BIG TEST\n");
            if(module_start_thread((module_entry_t *)data, _mod_thread, NULL, "Test Module Thread")) {
                return -1;
            }
            break;
        case LAMBDA_MODFUNC_STOP:
            kerror(ERR_BOOTINFO, "BIG DETEST\n");
            break;
        default:
            return -1;
    }

    return 0;
}
