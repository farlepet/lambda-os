#ifndef LAMBDA_LIB_FCNTL_H
#define LAMBDA_LIB_FCNTL_H

#define O_RDONLY    0x00000001 /** Read-only */
#define O_WRONLY    0x00000002 /** Write-only */
#define O_RDWR      0x00000003 /** Read/Write */
#define O_APPEND    0x00000004 /** Append to end of file */
#define O_CREAT     0x00000008 /** Create file if it doesn't exist */
#define O_DSYNC     0x00000010 /**  */
#define O_EXCL      0x00000020 /** Fail if file exists */
#define O_NOCTTY    0x00000040 /** Do not hand control to opened terminal */
#define O_NONBLOCK  0x00000080 /** Non-blocking open */
#define O_RSYNC     0x00000100 /**  */
#define O_SYNC      0x00000200 /**  */
#define O_TRUNC     0x00000400 /** Truncate file on open */
#define O_CLOEXEC   0x00000800 /** Close file on exec() call */
#define O_DIRECTORY 0x00001000 /** Fail if file is not a directory */


/**
 * \brief Open a file, possibly creating it
 * 
 * @param name Name of file and directory to open
 * @param flags Access flags, and additional options
 */
int open(const char *name, int flags, ...);

#endif
