#include <errno.h>
#include <libgen.h>
#include <string.h>

#include "data/cobuff.h"
#include "err/panic.h"
#include "fs/fs.h"
#include "fs/procfs.h"
#include "ipc/ipc.h"
#include "mm/alloc.h"
#include "proc/cond.h"
#include "proc/mtask.h"
#include "proc/atomic/lock.h"
#include "proc/atomic/tlock.h"

static int _ipc_open (kfile_t *, kfile_hand_t *);
static int _ipc_close(kfile_hand_t *);

static const file_ops_t _ipc_file_ops = {
    .open    = &_ipc_open,
    .mkdir   = NULL,
    .create  = NULL,
    .finddir = NULL
};

static const file_hand_ops_t _ipc_file_hand_ops = {
    .close   = &_ipc_close,
    .read    = NULL,
    .write   = NULL,
    .ioctl   = NULL,
    .readdir = NULL
};

/* TODO: Make configurable, selectable by creator, or dynamic */
#define IPC_QUEUE_LEN 8

int sys_ipc_create(const char *ipc_path) {
    /* TODO: Check path first */
    kproc_t *process = mtask_get_curr_process();
    kassert(process != NULL, "mtask_get_curr_process returned NULL!");

    char *path = strdup(ipc_path);
    if (!path) {
        return -ENOMEM;
    }

    char *name = basename(path);
    name[-1] = '\0';

    kfile_t *dir;
    if (*path) {
        dir = fs_find_file(process->cwd, path);
        if (!dir) {
            kfree(path);
            return -ENOENT;
        }
        if (!(dir->flags & FS_DIR)) {
            kfree(path);
            return -ENOTDIR;
        }
    } else {
        dir = fs_get_root();
    }

    kfile_t *file = kmalloc(sizeof(kfile_t) + sizeof(ipc_chan_t));
    ipc_chan_t *chan = (ipc_chan_t *)((uintptr_t)file + sizeof(*file));
    memset(file, 0, sizeof(*file));
    memset(chan, 0, sizeof(*chan));
    file->info = chan;

    strncpy(file->name, name, FILE_NAME_MAX);
    kfree(path);

    chan->pid = process->pid;
    if (cobuff_allocate(&chan->queue, sizeof(lambda_ipc_message_t), IPC_QUEUE_LEN)) {
        kfree(file);
        return -ENOMEM;
    }
    llist_init(&chan->resp_chans);

    /* TODO: Lock down more - creating process can open up permissions as desired */
    file->pflags = 0666;
    file->flags = FS_FILE; /* TODO: Should this be a special type? */
    // TODO: Creation time

    file->ops = &_ipc_file_ops;

    fs_add_file(file, NULL);

    /* TODO: Handle error */
    return proc_fs_open_file(file, OFLAGS_READ | OFLAGS_WRITE);
}

static ipc_resp_chan_t *get_resp_chan(ipc_chan_t *chan, int pid) {
    ipc_resp_chan_t *rchan;
    llist_iterator_t iter;
    llist_iterator_init(&chan->resp_chans, &iter);
    while (llist_iterate(&iter, (void **)&rchan)) {
        if (rchan->pid == pid) {
            return rchan;
        }
    }

    return NULL;
}

int sys_ipc_send(int fd, lambda_ipc_message_t *msg) {
    kfile_t *file = proc_fs_file_by_fd(fd);
    if (!file) {
        return -EBADF;
    }
    if (file->ops != &_ipc_file_ops) {
        return -ENXIO;
    }

    ipc_chan_t *chan = file->info;
    int pid = get_pid();

    /* How to deal with responses? */
    if (chan->pid == pid) {
        /* Owner responding to request */
        /* Lock so thread waiting for response cannot close file while we are
         * writing response */
        lock(&chan->resp_chans.lock);
        ipc_resp_chan_t *rchan = get_resp_chan(chan, msg->pid);
        if (!rchan) {
            /* Process has closed file, or owner has manipulated PID field */
            unlock(&chan->resp_chans.lock);
            return -ESRCH;
        }

        tlock_acquire(&rchan->lock);

        memcpy(&rchan->response, msg, sizeof(*msg));
        rchan->updated = 1;

        if (cond_signal(&rchan->cond)) {
            tlock_release(&rchan->lock);
            unlock(&chan->resp_chans.lock);
            return -EUNSPEC;
        }

        tlock_release(&rchan->lock);
        unlock(&chan->resp_chans.lock);

        return 0;
    } else {
        /* Other process sending to owner */
        msg->pid = pid;

        /* TOOD: Allow blocking? */
        return cobuff_put(&chan->queue, msg);
    }
}

int sys_ipc_recv(int fd, lambda_ipc_message_t *msg) {
    kfile_t *file = proc_fs_file_by_fd(fd);
    if (!file) {
        return -EBADF;
    }
    if (file->ops != &_ipc_file_ops) {
        return -ENXIO;
    }

    ipc_chan_t *chan = file->info;

    int pid = get_pid();

    if (chan->pid == pid) {
        return cobuff_get(&chan->queue, msg, 1);
    } else {
        /* Make sure list cannot be modified while we are searching it */
        lock(&chan->resp_chans.lock);
        ipc_resp_chan_t *rchan = get_resp_chan(chan, msg->pid);
        if (!rchan) {
            /* Shouldn't be possible - if a process can RECV on this channel,
             * then it must have an open handle */
            kpanic("Could not find response channel for process");
        }
        unlock(&chan->resp_chans.lock);

        tlock_acquire(&rchan->lock);

        if (!rchan->updated) {
            if (cond_wait(&rchan->cond, &rchan->lock)) {
                tlock_release(&rchan->lock);
                return -EUNSPEC;
            }
        }

        rchan->updated = 0;
        memcpy(msg, &rchan->response, sizeof(*msg));
        tlock_release(&rchan->lock);

        return 0;
    }
}

static int _ipc_open(kfile_t *file, kfile_hand_t *hand) {
    /* TODO: Check open flags */
    tlock_acquire(&file->file_lock);

    ipc_chan_t *chan = file->info;

    int pid = get_pid();
    if (pid != chan->pid) {
        lock(&chan->resp_chans.lock);

        if (get_resp_chan(chan, pid)) {
            /* Currently we only support a single handle per PID */
            return -EBUSY;
        }

        ipc_resp_chan_t *rchan = kmalloc(sizeof(*rchan));
        if (!rchan) {
            return -ENOMEM;
        }

        memset(rchan, 0, sizeof(*rchan));

        rchan->pid = pid;
        cond_init(&rchan->cond);
        if (tlock_init(&rchan->lock)) {
            kfree(rchan);
            unlock(&chan->resp_chans.lock);

            return -ENOMEM;
        }

        rchan->list_item.data = rchan;
        llist_append(&chan->resp_chans, &rchan->list_item);

        unlock(&chan->resp_chans.lock);
    } else {
        tlock_release(&file->file_lock);
        /* Owner is not allowed to explicitly open file */
        return -EBUSY;
    }

    hand->ops  = &_ipc_file_hand_ops;
    hand->file = file;
    hand->open_flags |= OFLAGS_OPEN;

    tlock_release(&file->file_lock);

    return 0;
}

static int _ipc_close(kfile_hand_t *hand) {
    /* TODO: Detect when all consumers have dropped handles, and the owner no longer exists. */
    tlock_acquire(&hand->lock);

    hand->open_flags = 0;

    ipc_chan_t *chan = hand->file->info;

    /* TODO: When owner process is dead and all handles are closed, fully
     * delete file. */

    int pid = get_pid();
    if (pid != chan->pid) {
        lock(&chan->resp_chans.lock);

        ipc_resp_chan_t *rchan = get_resp_chan(chan, pid);
        if (!rchan) {
            /* Shouldn't be possible - if a process can RECV on this channel,
             * then it must have an open handle */
            kpanic("Could not find response channel for process");
        }

        llist_remove_unlocked(&chan->resp_chans, &rchan->list_item);
        unlock(&chan->resp_chans.lock);

        tlock_destroy(&rchan->lock);
        kfree(rchan);
    } else {
        /* TODO: Remove files from VFS (or otherwise hide from filesystem) */
    }

    tlock_release(&hand->lock);

    return 0;
}

