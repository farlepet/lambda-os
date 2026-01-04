#include <string.h>

#include "mm/alloc.h"
#include "fs/fs.h"

#include "devfs.h"

/** Just used for memory allocation */
typedef struct {
    llist_item_t          item;
    devfs_module_handle_t handle;
} module_handle_item_store_t;
/** Just used for memory allocation */
typedef struct {
    llist_item_t        item;
    devfs_file_handle_t handle;
} file_handle_item_store_t;

static llist_t _module_handles;

static uint16_t               _next_modid();
static uint16_t               _next_fileid(devfs_module_handle_t *);
static devfs_module_handle_t *_find_mod(uint16_t);
static devfs_file_handle_t   *_find_file(devfs_module_handle_t *, uint16_t);

kfile_t *_dev_file = NULL;

const char *_dev_name = "dev";

int devfs_init(void) {
    llist_init(&_module_handles);

    _dev_file = kmalloc(sizeof(kfile_t));
    if(_dev_file == NULL) {
        return -1;
    }
    memset(_dev_file, 0, sizeof(kfile_t));

    strncpy(_dev_file->name, _dev_name, FILE_NAME_MAX);

    kfile_t *root = fs_find_file(NULL, "/");

    _dev_file->impl   = root->inode;
    _dev_file->uid    = 0;
    _dev_file->gid    = 0;
    _dev_file->pflags = PERMISSIONS(PERM_RWE, PERM_RE, PERM_RE);

    _dev_file->flags |= FS_DIR;

    _dev_file->ops = NULL; /* @todo */

    fs_add_file(_dev_file, root);

    return 0;
}


int devfs_register_module(devfs_module_handle_t **hand) {
    if(hand == NULL) {
        return -1;
    }

    module_handle_item_store_t *store = kmalloc(sizeof(module_handle_item_store_t));
    if(store == NULL) {
        return -1;
    }

    store->item.data = (void *)&store->handle;
    llist_append(&_module_handles, &store->item);
    
    store->handle._item = &store->item;
    store->handle.modid = _next_modid();
    llist_init(&store->handle.files);

    *hand = &store->handle;

    return store->handle.modid;
}

int devfs_deregister_module(devfs_module_handle_t *hand) {
    if(hand == NULL) {
        return -1;
    }

    llist_iterator_t     iter;
    devfs_file_handle_t *fhand;
    
    llist_iterator_init(&hand->files, &iter);
    while(!llist_iterate(&iter, (void **)&fhand)) {
        devfs_deregister_file(fhand);
    }

    llist_remove(&_module_handles, hand->_item);

    kfree(hand->_item);

    return 0;
}


int devfs_register_file(devfs_module_handle_t *mhand, devfs_file_handle_t **fhand, const char *filename) {
    if((mhand    == NULL) ||
       (fhand    == NULL) ||
       (filename == NULL)) {
        return -1;
    }

    file_handle_item_store_t *store = kmalloc(sizeof(file_handle_item_store_t));
    if(store == NULL) {
        return -1;
    }
    memset(store, 0, sizeof(file_handle_item_store_t));

    store->item.data = (void *)&store->handle;
    llist_append(&mhand->files, &store->item);

    store->handle._item = &store->item;
    store->handle.modid  = mhand->modid;
    store->handle.fileid = _next_fileid(mhand);

    kfile_t *file = &store->handle.file;
    strncpy(file->name, filename, FILE_NAME_MAX);

    file->impl   = _dev_file->inode;
    file->uid    = 0;
    file->gid    = 0;
    file->pflags = PERMISSIONS(PERM_RW, PERM_R, PERM_R);

    file->flags |= FS_DIR;

    file->ops = NULL; /* @todo */

    fs_add_file(file, _dev_file);

    *fhand = &store->handle;

    return store->handle.fileid;
}

int devfs_deregister_file(devfs_file_handle_t *fhand) {
    if(fhand == NULL) {
        return -1;
    }

    devfs_module_handle_t *mhand = _find_mod(fhand->modid);
    if(mhand == NULL) {
        return -1;
    }

    llist_remove(&mhand->files, fhand->_item);

    kfree(fhand->_item);

    return 0;
}



static uint16_t _next_modid() {
    uint16_t               nextid = 1;
    llist_iterator_t       iter;
    devfs_module_handle_t *hand;
    
    llist_iterator_init(&_module_handles, &iter);
    while(!llist_iterate(&iter, (void **)&hand)) {
        if(hand->modid >= nextid) {
            nextid = hand->modid + 1;
        }
    }

    return nextid;
}

static uint16_t _next_fileid(devfs_module_handle_t *mod) {
    uint16_t             nextid = 1;
    llist_iterator_t     iter;
    devfs_file_handle_t *hand;
    
    llist_iterator_init(&mod->files, &iter);
    while(!llist_iterate(&iter, (void **)&hand)) {
        if(hand->fileid >= nextid) {
            nextid = hand->fileid + 1;
        }
    }

    return nextid;
}

static devfs_module_handle_t *_find_mod(uint16_t modid) {
    llist_iterator_t       iter;
    devfs_module_handle_t *hand;
    
    llist_iterator_init(&_module_handles, &iter);
    while(!llist_iterate(&iter, (void **)&hand)) {
        if(hand->modid == modid) {
            return hand;
        }
    }

    return NULL;
}

__unused
static devfs_file_handle_t *_find_file(devfs_module_handle_t *mod, uint16_t fileid) {
    llist_iterator_t     iter;
    devfs_file_handle_t *hand;
    
    llist_iterator_init(&mod->files, &iter);
    while(!llist_iterate(&iter, (void **)&hand)) {
        if(hand->fileid == fileid) {
            return hand;
        }
    }

    return NULL;
}
