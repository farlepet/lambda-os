#include <errno.h>
#include <string.h>

#include <fs/fs.h>
#include <fs/procfs.h>
#include <proc/mtask.h>
#include <err/panic.h>
#include <err/error.h>
#include <fs/dirinfo.h>
#include <mm/alloc.h>
#include <mm/mm.h>

kfile_hand_t *proc_fs_hand_by_fd(int desc) {
    if ((desc > MAX_OPEN_FILES) || (desc < 0)) {
        return NULL;
    }

    kthread_t *thread = mtask_get_curr_thread();
    kassert(thread != NULL, "mtask_get_curr_thread returned NULL!");

    return thread->process->open_files[desc];
}

kfile_t *proc_fs_file_by_fd(int desc) {
    if ((desc > MAX_OPEN_FILES) || (desc < 0)) {
        return NULL;
    }

    kthread_t *thread = mtask_get_curr_thread();
    kassert(thread != NULL, "mtask_get_curr_thread returned NULL!");

    kfile_hand_t *hand = thread->process->open_files[desc];
    kassert(hand->file != NULL, "kfile_hand_t::file is NULL!");

    return hand->file;
}

int proc_fs_read(int desc, uint32_t off, uint32_t sz, uint8_t *buff) {
    kfile_hand_t *hand = proc_fs_hand_by_fd(desc);
    if (!hand) {
        return -EBADF;
    }

    return fs_read(hand, off, sz, buff);
}

int proc_fs_read_blk(int desc, uint32_t off, uint32_t sz, uint8_t *buff) {
    kfile_hand_t *hand = proc_fs_hand_by_fd(desc);
    if (!hand) {
        return -EBADF;
    }

    if (hand->file && (hand->file->flags & FS_STREAM) &&
        !(hand->open_flags & OFLAGS_NONBLOCK)) {
        uint32_t count = 0;
        while (count < sz) {
            uint32_t ret = fs_read(hand, off, sz - count, buff + count);
            if(ret > 0x80000000) {
                return count;
            }
            count += ret;
            run_sched(); // Wait
        }
        return count;
    } else {
        kthread_t *thread = mtask_get_curr_thread();

        off = thread->process->file_position[desc];
        uint32_t n = fs_read(hand, off, sz, buff);
        thread->process->file_position[desc] += n;
        return n;
    }
}

int proc_fs_write(int desc, uint32_t off, uint32_t sz, uint8_t *buff) {
    kfile_hand_t *hand = proc_fs_hand_by_fd(desc);
    if (!hand) {
        return -EBADF;
    }

    return fs_write(hand, off, sz, buff);
}

static int _open_check_flags(kfile_t *file, uint32_t flags) {
    if(flags & OFLAGS_DIRECTORY) {
        if(!(file->flags & FS_DIR)) {
            return -ENOTDIR;
        }
    }

    return 0;
}

int proc_fs_open_file(kfile_t *file, uint32_t flags) {
    kthread_t *thread = mtask_get_curr_thread();
    if(!thread) return -EUNSPEC;

    // TODO: Check if file is open!!!
    // TODO: Handle errors!
    // TODO: Make sure flags match up!
    TRY_OR_RET(_open_check_flags(file, flags));

    kfile_hand_t *hand = (kfile_hand_t *)kmalloc(sizeof(kfile_hand_t));
    memset(hand, 0, sizeof(kfile_hand_t));

    hand->open_flags = flags;

    if(fs_open(file, hand)) {
        kdebug(DEBUGSRC_FS, ERR_DEBUG, "proc_fs_open: fs_open of %s failed!", file->name);
        kfree(hand);
        return -EUNSPEC;
    }

    // TODO: Handle errors!
    int ret = proc_add_file(thread->process, hand);
    if (ret > 0) {
        if(!SAFETY_CHECK(hand->open_flags & OFLAGS_OPEN)) {
            kpanic("Open succeeded, but open flag is not set!");
        }
        return ret;
    }

    kfree(hand);

    return -ENFILE;
}

int proc_fs_open(const char *name, uint32_t flags) {
    kthread_t *thread = mtask_get_curr_thread();
    if(!thread) return -EUNSPEC;

    if(thread->process->cwd == NULL) {
        return -EUNSPEC;
    }

    kfile_t *file = fs_find_file(thread->process->cwd, name);
    if(!file) {
        return -ENOENT;
    }

    return proc_fs_open_file(file, flags);
}

int proc_fs_close(int desc) {
    kfile_hand_t *hand = proc_fs_hand_by_fd(desc);
    if (!hand) {
        return -EBADF;
    }

    fs_close(hand);
    kfree(hand);

    kthread_t *thread = mtask_get_curr_thread();
    thread->process->open_files[desc] = NULL;

    return 0; // TODO: Error checking!
}

int proc_fs_mkdir(int desc, char *name, uint32_t perms) {
    kfile_t *file = proc_fs_file_by_fd(desc);
    if (!file) {
        return -EBADF;
    }

    return fs_mkdir(file, name, perms);
}

int proc_fs_create(int desc, char *name, uint32_t perms) {
    kfile_t *file = proc_fs_file_by_fd(desc);
    if (!file) {
        return -EBADF;
    }

    return fs_create(file, name, perms);
}

int proc_fs_ioctl(int desc, int req, void *args) {
    kfile_hand_t *hand = proc_fs_hand_by_fd(desc);
    if (!hand) {
        return -EBADF;
    }

    return fs_ioctl(hand, req, args);
}

int proc_fs_getdirinfo(int desc, struct dirinfo *dinfo) {
    if(dinfo == NULL)         return -EINVAL;

    kfile_t *file = proc_fs_file_by_fd(desc);
    if (!file) {
        return -EBADF;
    }

    dinfo->ino = file->inode;
    if(file->parent) {
        dinfo->parent_ino = file->parent->inode;
    } else {
        dinfo->parent_ino = 0;
    }
    dinfo->n_children = llist_count(&file->children);

    memcpy(dinfo->name, file->name, FILE_NAME_MAX);

    return 0;
}

int proc_fs_readdir(int desc, uint32_t idx, struct user_dirent *buff, uint32_t buff_size) {
    if(buff == NULL)          return -EINVAL;
    if(buff_size == 0)        return -EINVAL;

    kfile_t *file = proc_fs_file_by_fd(desc);
    if (!file) {
        return -EBADF;
    }

    /* @todo Bounds checking */

    if(idx == 0) {
        /** . */
        buff->d_ino = file->inode;
        strcpy(buff->d_name, ".");
    } else if(idx == 1) {
        /** .. */
        if(file->parent != NULL) file = file->parent;
        buff->d_ino = file->inode;
        strcpy(buff->d_name, "..");
    } else {
        idx -= 2;

        llist_item_t *child = llist_get(&file->children, idx);
        if(child == NULL) { return -EUNSPEC; }
        file = (kfile_t *)child->data;

        buff->d_ino = file->inode;
        strcpy(buff->d_name, file->name);
    }

    return (sizeof(struct user_dirent) + strlen(buff->d_name) + 1);
}

int proc_fs_stat(const char *path, kstat_t *buf, uint32_t __unused flags) {
    if(!mm_check_addr(path) ||
       !mm_check_addr(buf)) {
        return -EINVAL;
    }
    
    kthread_t *thread = mtask_get_curr_thread();
    if(!thread) return -EUNSPEC;

    if(thread->process->cwd == NULL) {
        return -EUNSPEC;
    }

    /* TODO: Check permissions */
    struct kfile *file = fs_find_file(thread->process->cwd, path);
    if(!file) {
        return -ENOENT;
    }

    return kfstat(file, buf);
}

int proc_fs_access(int dirfd, const char *pathname, uint32_t __unused mode, uint32_t flags) {
    kthread_t *thread = mtask_get_curr_thread();

    kfile_t *dir = thread->process->cwd;
    if(flags & SYSCALL_ACCESS_FLAG_CWDOVER) {
        if((dirfd < 0) ||
           (dirfd >= MAX_OPEN_FILES) ||
           (thread->process->open_files[dirfd] == NULL)) {
            /* @todo Return error code */
            return -EBADF;
        }
        dir = thread->process->open_files[dirfd]->file;
    } else {
        dir = thread->process->cwd;
    }

    kfile_t *file = fs_find_file(dir, pathname);
    if(file == NULL) {
        return -ENOENT;
    }

    /* @todo Actually check permissions - right now this only check existence! */
    return 0;
}
