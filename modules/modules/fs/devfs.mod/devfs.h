#ifndef MOD_FS_DEVFS_DEVFS_H
#define MOD_FS_DEVFS_DEVFS_H

#include <stdint.h>

#include "data/llist.h"
#include "fs/kfile.h"

/**
 * Handle for a module that may control multiple file handles
 */
typedef struct {
    /**
     * Module ID
     * Generated sequentially from 1 each time a new module registers with devfs. */
    uint16_t modid;
    /**
     * List of file handles associated with this module */
    llist_t  files;
    /**
     * Pointer to owning list item */
    llist_item_t *_item;
} devfs_module_handle_t;

/**
 * Handle for a file owned by a module
 */
typedef struct {
    /**
     * File ID
     * Generated sequentially from 1 each time a new file registers with devfs. Module-specific */
    uint16_t fileid;
    /**
     * Module ID
     * ID of module that owns this file */
    uint16_t modid;
    /**
     * File
     * Kernel representation of the file */
    kfile_t file;
    /**
     * Pointer to owning list item */
    llist_item_t *_item;
} devfs_file_handle_t;

/**
 * Initialize devfs module
 */
int devfs_init(void);

/**
 * Register a module with devfs
 * 
 * @param[out] hand Where to store module handle pointer
 * 
 * @return < 0 on error, else module ID
 */
int devfs_register_module(devfs_module_handle_t **hand);

/**
 * Deregister a module from devfs
 * 
 * @param[in] hand Pointer to module handle
 * 
 * @return < 0 on error, else 0
 */
int devfs_deregister_module(devfs_module_handle_t *hand);

/**
 * Register a file with devfs
 * 
 * @param[in]  mhand    Pointer to module handle
 * @param[out] fhand    Where to store file handle pointer
 * @param[in]  filename Filename and path of file to create, relative to devfs mountpoint
 * 
 * @return < 0 on error, else file ID
 */
int devfs_register_file(devfs_module_handle_t *mhand, devfs_file_handle_t **fhand, const char *filename);

/**
 * Deregister a file from devfs
 * 
 * @param[in] fhand Pointer to file handle
 * 
 * @return < 0 on error, else 0
 */
int devfs_deregister_file(devfs_file_handle_t *fhand);

#endif
