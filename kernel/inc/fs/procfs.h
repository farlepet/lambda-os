#ifndef PROCFS_H
#define PROCFS_H

#include <fs/kfile.h>
#include <fs/dirinfo.h>
#include <sys/stat.h>

struct user_dirent {
    uint32_t d_ino;
    char     d_name[];
};

/**
 * @brief Get file handle from processes file descriptor
 *
 * @param desc File descriptor
 *
 * @return Pointer to file handle if found, else NULL
 */
kfile_hand_t *proc_fs_hand_by_fd(int desc);

kfile_t *proc_fs_file_by_fd(int desc);

/**
 * @brief Open handle for the given file on behalf of process
 *
 * @param file File to open
 * @param flags Flags to open file with
 *
 * @return File descriptor on success, else negative error code
 */
int proc_fs_open_file(kfile_t *file, uint32_t flags);

int proc_fs_read    (int desc, uint32_t off, uint32_t sz, uint8_t *buff);
int proc_fs_write   (int desc, uint32_t off, uint32_t sz, uint8_t *buff);
int proc_fs_open    (const char *name, uint32_t flags);
int proc_fs_close   (int desc);
int proc_fs_mkdir   (int desc, char *name, uint32_t perms);
int proc_fs_create  (int desc, char *name, uint32_t perms);
int proc_fs_ioctl   (int desc, int req, void *args);
int proc_fs_read_blk(int desc, uint32_t off, uint32_t sz, uint8_t *buff);

int proc_fs_getdirinfo(int desc, struct dirinfo *dinfo);

int proc_fs_readdir(int desc, uint32_t idx, struct user_dirent *buff, uint32_t buff_size);

int proc_fs_stat(const char *path, kstat_t *buf, uint32_t flags);

#define SYSCALL_ACCESS_FLAG_CWDOVER (1UL << 0) /** Override CWD with given fd */
int proc_fs_access(int dirfd, const char *pathname, uint32_t mode, uint32_t flags);

#endif
