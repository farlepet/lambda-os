#include <string.h>

#include <lambda/mod/module.h>
#include <err/error.h>

#include "fs/devfs.mod/devfs.h"

static devfs_module_handle_t *_devfs_handle = NULL;
static struct {
    devfs_file_handle_t *mem;
} _devfs_files;

ssize_t _dev_mem_read(kfile_hand_t *hand, uint32_t off, uint32_t sz, void *buff) {
    /* @todo Check given address range
     * @todo Check permissions
     * @todo Map in order to access physical memory */
    void *start = (void *)off;
    memcpy(buff, start, sz);
    (void)hand;
    return sz;
}

static file_hand_ops_t _dev_mem_hand_ops = {
    .read  = _dev_mem_read,
    .write = NULL,
    .ioctl = NULL,
    NULL
};

int _dev_mem_open(kfile_t *f, kfile_hand_t *hand) {
    hand->open_flags = OFLAGS_OPEN;
    hand->file       = f;
    hand->ops        = &_dev_mem_hand_ops;

    return 0;
}

static file_ops_t _dev_mem_ops = {
    .open = _dev_mem_open,
    NULL
};


int mod_func(uint32_t func, void *data __unused) {
    switch(func) {
        case LAMBDA_MODFUNC_START:
            if((devfs_register_module(&_devfs_handle) < 0)                          ||
               (devfs_register_file(_devfs_handle, &_devfs_files.mem, "mem") < 0)) {
                return -1;
            }
            _devfs_files.mem->file.ops = &_dev_mem_ops;
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
