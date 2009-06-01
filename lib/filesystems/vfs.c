/* ---------------------------------------------------
* Jonix
*
* vfs.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "filesystems/vfs.h"
#include "filesystems/initrd.h"
#include "c/fcntl.h"

mount_point_t root_fs;


/*  ----------------------------------------------------
 *  Function:           vfs_init
 *  --------------------------------------------------*/
void vfs_init(){

    /*
     * Create an initial ramdisk for the root filesystem.
     */
    block_dev_t *initrd = ramdisk_new(4096, "initrd");
    MALLOC_PANIC(initrd);

    if(!list_add(&hal_block_devs, initrd)){
        PANIC("Failed to add initrd to list of block devices.");
    }

    root_fs.dev = initrd;
    root_rs.fs  = initrd_fs;

}

#if 0
/*  ----------------------------------------------------
 *  Function:           vfs_vopen
 *  --------------------------------------------------*/
int vfs_vopen(const char *path, int flags, va_list ap){

    if(flags & O_CREAT){
        int perms   = va_arg(ap, int);
    }

}


/*  ----------------------------------------------------
 *  Function:           vfs_open
 *  --------------------------------------------------*/
int vfs_open(const char *path, int flags, ...){
    va_list ap;
    int retval;
    va_start(ap, flags);
    retval  = vfs_vopen(path, flags, ap);
    va_end(ap);
    return retval;
}

#endif

/*  ----------------------------------------------------
 *  Function:           vfs_check_open_perms
 *  --------------------------------------------------*/
bool vfs_check_open_perms(int flags, vfs_perms_t *file_perms){
    uint8_t perms;
    // TODO: user==root?
    //
    // Permission bits are low 9 bits of a mode_t
    //
    if     (thr_current->uid == file_perms->uid) perms = (file_perms->mode >> 6) & 7;
    else if(thr_current->gid == file_perms->gid) perms = (file_perms->mode >> 3) & 7;
    else                                         perms = (file_perms->mode >> 0) & 7;

    if(
           ((flags & O_RDONLY || flags & O_RDWR) && !(perms & 4))
        || ((flags & O_WRONLY || flags & O_RDWR) && !(perms & 2))
    ){
        return 0;
    }

    return 1;
}


/*  ----------------------------------------------------
 *  Function:           vfs_check_create_perms
 *  --------------------------------------------------*/
bool vfs_check_create_perms(vfs_perms_t *file_perms){
    uint8_t perms;
    // TODO: user==root?
    //
    // Permission bits are low 9 bits of a mode_t
    //
    if     (thr_current->uid == file_perms->uid) perms = (file_perms->mode >> 6) & 7;
    else if(thr_current->gid == file_perms->gid) perms = (file_perms->mode >> 3) & 7;
    else                                         perms = (file_perms->mode >> 0) & 7;

    if(!(perms & 2)) return 0;

    return 1;
}


/*  ----------------------------------------------------
 *  Function:           path_shift_file
 *  --------------------------------------------------*/
const char * path_shift_file(const char *path, char *file){

    // TODO: check that each path component does not exceed the
    // maximum filename size.

    int i;
    for(i=0; i <= VFS_FILENAME_LEN && *(path+i)!='\0' && *(path+i)!='/'; i++){
        *(file+i)   = *(path+i);
    }
    *(file+i)   = '\0';

    if(*(path+i) == '\0') return path+i;
    else return path+i+1;
}
