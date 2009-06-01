/* ---------------------------------------------------
* Jonix
*
* stat.h
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#ifndef _STAT_H
#define _STAT_H

#include "user/sys/types.h"

struct stat{
    dev_t   st_dev;
    ino_t     st_ino;
    mode_t    st_mode;
    nlink_t   st_nlink;
    uid_t     st_uid;
    gid_t     st_gid;
    dev_t     st_rdev;
    off_t     st_size;
    time_t    st_atime;
    time_t    st_mtime;
    time_t    st_ctime;
    blksize_t st_blksize;
    blkcnt_t  st_blocks;
};

// Permission bits of mode_t
//
#define S_IRUSR  (1 << 8)
#define S_IWUSR  (1 << 7)
#define S_IXUSR  (1 << 6)

#define S_IRGRP  (1 << 5)
#define S_IWGRP  (1 << 4)
#define S_IXGRP  (1 << 3)

#define S_IROTH  (1 << 2)
#define S_IWOTH  (1 << 1)
#define S_IXOTH  (1 << 0)

#define S_IRWXU  (S_IRUSR | S_IWUSR | S_IXUSR)
#define S_IRWXG  (S_IRGRP | S_IWGRP | S_IXGRP)
#define S_IRWXO  (S_IROTH | S_IWOTH | S_IXOTH)

#define S_ISUID (1 << 9)
#define S_ISGID (1 << 10)

// File type bits of mode_t
// (Bits 11-13)
//
#define S_IFBLK  (1 << 11);
#define S_IFCHR  (2 << 11);
#define S_IFIFO  (3 << 11);
#define S_IFREG  (4 << 11);
#define S_IFDIR  (5 << 11);
#define S_IFLNK  (6 << 11);
#define S_IFSOCK (7 << 11);

#define S_IFMT (7 << 11);

// Mount point bit
//
#define S_IFMNT  (1 << 14);

#define S_ISBLK(m)  ((m) & S_IFBLK)
#define S_ISCHR(m)  ((m) & S_IFCHR)
#define S_ISDIR(m)  ((m) & S_IFDIR)
#define S_ISFIFO(m) ((m) & S_IFIFO)
#define S_ISREG(m)  ((m) & S_IFREG)
#define S_ISLNK(m)  ((m) & S_IFLNK)
#define S_ISSOCK(m) ((m) & S_IFSOCK)
#define S_ISMNT(m)  ((m) & S_IFMNT)

#define S_TYPEISMQ(buf)  0
#define S_TYPEISSEM(buf) 0
#define S_TYPEISSHM(buf) 0


extern int    chmod(const char *, mode_t);
extern int    fchmod(int, mode_t);
extern int    fstat(int, struct stat *);
extern int    lstat(const char *restrict, struct stat *restrict);
extern int    mkdir(const char *, mode_t);
extern int    mkfifo(const char *, mode_t);
extern int    stat(const char *restrict, struct stat *restrict);
extern mode_t umask(mode_t);


#endif
