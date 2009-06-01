/* ---------------------------------------------------
* Jonix
*
* initrd.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _INITRD_H
#define _INITRD_H

#include "system.h"
#include "c/dirent.h"
#include "filesystems/vfs.h"

#define INITRD_MAX_FILES 100
#define INITRD_FILENAME_LEN VFS_FILENAME_LEN


/*  ----------------------------------------------------
 *  Typedef struct:     initrd_fs_hdr_t
 *  --------------------------------------------------*/
typedef struct {
    uint32_t    size; // Including size of header
    uint32_t    files[INITRD_MAX_FILES];    
    uint8_t     file_cnt; 
} __attribute__((__packed__)) initrd_fs_hdr_t;


/*  ----------------------------------------------------
 *  Typedef struct:     initrd_file_hdr_t
 *
 *  Information stored about a file in its header.
 *  --------------------------------------------------*/
typedef struct {
    char        name[INITRD_FILENAME_LEN + 1];
    uint32_t    size; // Including size of header
    mode_t      mode;
    uint16_t    uid;
    uint16_t    gid;
    void        *link; // for links and mountpoints
} __attribute__((__packed__)) initrd_file_hdr_t;


extern int     initrd_open(block_dev_t *dev, const char *path, int flags, int perms);
extern int     initrd_close(int);
extern ssize_t initrd_read(int, void *, size_t);
extern ssize_t initrd_write(int, const void *, size_t);
extern struct  dirent initrd_readdir(int);

#endif
