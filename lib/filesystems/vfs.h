/* ---------------------------------------------------
* Jonix
*
* vfs.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _VFS_H
#define _VFS_H

#include "system.h"
#include "devs/hal.h"
#include "util/id.h"

#define VFS_FILENAME_LEN 255

#define FILETYPE_FILE        0x01
#define FILETYPE_DIRECTORY   0x02
#define FILETYPE_CHARDEVICE  0x03
#define FILETYPE_BLOCKDEVICE 0x04
#define FILETYPE_PIPE        0x05
#define FILETYPE_SYMLINK     0x06
#define FILETYPE_MOUNTPOINT  0x08

typedef int     (*file_open_func_t) (block_dev_t *, const char *, int flags, int mode_if_create);
typedef int     (*file_close_func_t)(int);
typedef ssize_t (*file_read_func_t) (int, void *, size_t);
typedef ssize_t (*file_write_func_t)(int, const void *, size_t);
typedef struct dirent (*file_readdir_func_t)(int);


/*  ----------------------------------------------------
 *  Typedef struct:     filesystem_t
 *  --------------------------------------------------*/
typedef struct {
    file_open_func_t    open;
    file_close_func_t   close;
    file_read_func_t    read;
    file_write_func_t   write;
    file_readdir_func_t readdir;
} filesystem_t;


/*  ----------------------------------------------------
 *  Typedef struct:     mount_point_t
 *  --------------------------------------------------*/
typedef struct {
    filesystem_t    *fs;
    block_dev_t     *dev;
    void            *fs_extra; // Probably points to a specific filesystem structure.
} mount_point_t;


/*  ----------------------------------------------------
 *  Typedef struct:     vfs_perms_t
 *  --------------------------------------------------*/
typedef struct {
    id_t    uid;
    id_t    gid;
    mode_t  mode;
} vfs_perms_t;


/*
 * Functions
 */
extern const char * path_shift_file(const char *path, char *file);
extern bool vfs_check_open_perms(int flags, vfs_perms_t *file_perms);
extern bool vfs_check_create_perms(vfs_perms_t *file_perms);

#endif
