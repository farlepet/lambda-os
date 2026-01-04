#include <string.h>

#include <lambda/mod/module.h>

#include <err/error.h>
#include <fs/stream.h>
#include <mm/alloc.h>

#include "tty.h"

#define BUFFER_SZ 64

static int _dev_in(tty_handle_t *hand, uint8_t b) {
    fs_write(&hand->buff_in, 0, 1, &b);
    return 0;
}

tty_handle_t *tty_create() {
    tty_handle_t *hand;

    hand = (tty_handle_t *)kmalloc(sizeof(tty_handle_t));
    if(!hand) {
        return NULL;
    }

    memset(hand, 0, sizeof(tty_handle_t));

    kfile_t *_in  = stream_create(BUFFER_SZ);
    if(!_in) {
        kfree(hand);
        return NULL;
    }
    kfile_t *_out = stream_create(BUFFER_SZ);
    if(!_out) {
        /* TODO: Destroy _in */
        kfree(hand);
        return NULL;
    }

    hand->buff_in.open_flags  = OFLAGS_READ | OFLAGS_WRITE;
    hand->buff_out.open_flags = OFLAGS_READ | OFLAGS_WRITE;

    /* TODO: Check result, and potentially create two open files */
    fs_open(_in,  &hand->buff_in);
    fs_open(_out, &hand->buff_out);

    /* Default device input handler function */
    hand->dev_in = _dev_in;

    return hand;
}

int mod_func(uint32_t func, void *data __unused) {
    switch(func) {
        case LAMBDA_MODFUNC_START:
            kerror(ERR_BOOTINFO, "Initializing TTY driver\n");
            break;
        case LAMBDA_MODFUNC_STOP:
            kerror(ERR_BOOTINFO, "BIG DETEST\n");
            break;
        default:
            return -1;
    }

    return 0;
}
