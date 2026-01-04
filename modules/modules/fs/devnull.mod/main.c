#include <lambda/mod/module.h>
#include <err/error.h>
#include <string.h>

#include "fs/devfs.mod/devfs.h"

static devfs_module_handle_t *_devfs_handle = NULL;
static struct {
    devfs_file_handle_t *null;
    devfs_file_handle_t *zero;
} _devfs_files;

ssize_t _dev_null_read(kfile_hand_t *hand, uint32_t off, uint32_t sz, void *buff) {
    (void)hand;
    (void)off;
    (void)sz;
    (void)buff;
    return 0;
}

static file_hand_ops_t _dev_null_hand_ops = {
    .read  = _dev_null_read
};

ssize_t _dev_zero_read(kfile_hand_t *hand, uint32_t off, uint32_t sz, void *buff) {
    (void)hand;
    (void)off;
    memset(buff, 0, sz);
    return sz;
}

static file_hand_ops_t _dev_zero_hand_ops = {
    .read  = _dev_zero_read
};


int _dev_null_open(kfile_t *f, kfile_hand_t *hand) {
    hand->open_flags = OFLAGS_OPEN;
    hand->file       = f;
    hand->ops        = &_dev_null_hand_ops;

    return 0;
}

static file_ops_t _dev_null_ops = {
    .open = _dev_null_open
};

int _dev_zero_open(kfile_t *f, kfile_hand_t *hand) {
    hand->open_flags = OFLAGS_OPEN;
    hand->file       = f;
    hand->ops        = &_dev_zero_hand_ops;

    return 0;
}

static file_ops_t _dev_zero_ops = {
    .open = _dev_zero_open
};

int mod_func(uint32_t func, void *data __unused) {
    switch(func) {
        case LAMBDA_MODFUNC_START:
            if((devfs_register_module(&_devfs_handle) < 0)                          ||
               (devfs_register_file(_devfs_handle, &_devfs_files.null, "null") < 0) ||
               (devfs_register_file(_devfs_handle, &_devfs_files.zero, "zero") < 0)) {
                return -1;
            }
            _devfs_files.null->file.ops = &_dev_null_ops;
            _devfs_files.zero->file.ops = &_dev_zero_ops;
            break;
        case LAMBDA_MODFUNC_STOP:
            if(devfs_deregister_module(_devfs_handle)) {
                return -1;
            }
            break;
        default:
            return -1;
    }

    return 0;
}
