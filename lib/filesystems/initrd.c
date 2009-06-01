/* ---------------------------------------------------
* Jonix
*
* initrd.c
*
* Author(s): Jonathan Haigh
* Last edited by: Jonathan Haigh
* --------------------------------------------------*/

#include "filesystems/initrd.h"
#include "filesystems/vfs.h"
#include "c/errno.h"

bool initrd_get_file_hdr(block_dev_t *dev, initrd_fs_hdr_t *fs_hdr, initrd_file_hdr_t *file_hdr, uint8_t index);
bool initrd_get_fs_hdr(block_dev_t *dev, initrd_fs_hdr_t *fs_hdr);

filesystem_t initrd_fs;
initrd_fs->open     = initrd_open;
initrd_fs->close    = initrd_close;
initrd_fs->read     = initrd_read;
initrd_fs->write    = initrd_write;
initrd_fs->readdir  = initrd_readdir;


/*  ----------------------------------------------------
 *  Function:       initrd_open
 *  --------------------------------------------------*/
int initrd_open(
    id_t        dev,
    const char  *path,
    int         flags,
    mode_t      mode,
    vfs_perms_t *parent_perms
){
    initrd_fs_hdr_t fs_hdr;
    if(!initrd_get_fs_hdr(dev, &fs_hdr)) return -1;

    return _initrd_open(dev, &fs_hdr, path, flags, mode, parent_perms, 0);
}


/*  ----------------------------------------------------
 *  Function:       _initrd_open
 *  --------------------------------------------------*/
int _initrd_open(
    id_t            dev,
    initrd_fs_hdr_t *fs_hdr,
    const char      *path,
    int             flags,
    mode_t          mode,
    vfs_perms_t     *parent_perms,
    uint8_t         root_id
){

    int file_id;

    char filename[VFS_FILENAME_LEN+1];
    char *new_path;

    if(*path = '\0'){
        filename[0] = '\0';
        new_path    = path;
        file_id     = fs_hdr->file_cnt > 0? root_id : -1;
    }
    else{
        new_path = path_shift_file(path, filename);
        file_id  = initrd_find_in_dir(dev, &fs_hdr, root_id, filename);
    }
        
    // If file does not exist.
    //
    if(file_id < 0){

        if(*new_path == '\0' && flags & O_CREAT && vfs_check_create_perms(parent_perms)){
            file_id = initrd_create_file(dev, fs_hdr, filename, mode, root_id);
            if(file_id == -1) return -1;
        }
        else return -1;
    }
    else if(flags & O_EXCL){
        return -1;
    }


    initrd_file_hdr_t file_hdr;
    if(!initrd_get_file_hdr(dev, &fs_hdr, &file_hdr, file_id)) return -1;

    if(file_hdr->type & FILETYPE_DIRECTORY){

        if(file_hdr->type & FILETYPE_MOUNTPOINT){

            mount_point_t *mp   = (mount_point_t *)file_hdr->link;

            id_t new_dev = (dev->open)(mp->dev);
            if(id_t == -1) return -1;

            vfs_perms_t pp;
            pp->uid     = file_hdr->uid; 
            pp->gid     = file_hdr->gid;
            pp->mode    = file_hdr->mode;

            int retval  =  mp->fs->open(new_dev, new_path, flags, mode, pp);

            (dev->close)(new_dev);

            return retval;
        }

        if(new_path[0] != '\0'){
            vfs_perms_t pp;
            pp->uid     = file_hdr->uid; 
            pp->gid     = file_hdr->gid;
            pp->mode    = file_hdr->mode;
            return _initrd_open(dev, fs_hdr, new_path, flags, mode, pp, file_id);
        }
    }

    if(file_hdr->mode & S_IFMT != filetype) return -1;
asdf
    //
    // TODO: perms, open flags etc.
    //
    return initrd_open_file
    
    
}


/*  ----------------------------------------------------
 *  Function:       initrd_find_in_dir
 *  --------------------------------------------------*/
int initrd_find_in_dir(block_dev_t *dev, initrd_fs_hdr_t *fs_hdr, uint8_t dir_id, const char *name){

    uint8_t file_id;

    // Get the directory's header
    //
    initrd_file_hdr_t dir_hdr;
    if(!initrd_get_file_hdr(dev, fs_hdr, dir_hdr, dir_id)) return -1;

    // Loop through directory's files
    //
    int offset;
    for(offset=fs_hdr->files[dir_id] + sizeof(initrd_file_hdr_t); i < dir_hdr.size; i++){

        if(!(dev->read)(&file_id, 1, offset)) return -1;

        initrd_file_hdr_t file_hdr;
        if(!(dev->read)(&file_hdr, sizeof(initrd_file_hdr_t), hdr.files[id])) return -1;

        if(strcmp(file_hdr->name, name) == 0) return file_id;
    }

    return -1;
}


/*  ----------------------------------------------------
 *  Function:       initrd_get_file_hdr
 *  --------------------------------------------------*/
bool initrd_get_file_hdr(
    block_dev_t         *dev,
    initrd_fs_hdr_t        *fs_hdr,
    initrd_file_hdr_t   *file_hdr,
    uint8_t             index
){
    return (dev->read)(file_hdr, sizeof(initrd_file_hdr_t), fs_hdr->files[index]);
}


/*  ----------------------------------------------------
 *  Function:       initrd_get_fs_hdr
 *  --------------------------------------------------*/
bool initrd_get_fs_hdr(
    block_dev_t         *dev,
    initrd_fs_hdr_t        *fs_hdr
){
    return (dev->read)(fs_hdr, sizeof(initrd_fs_hdr_t), 0);
}
