#ifndef MOD_TTY_TTY_H
#define MOD_TTY_TTY_H

#include <stdint.h>

#include <fs/kfile.h>

typedef struct tty_handle tty_handle_t;

struct tty_handle {
    /* TODO: Think of better function names? */
    int (*dev_in) (tty_handle_t *, uint8_t); /** Device writing to TTY (input) */

    kfile_hand_t buff_in;
    kfile_hand_t buff_out;

    void *data; /** Data - meaning dependant on the underlying driver */
};

tty_handle_t *tty_create();

#endif