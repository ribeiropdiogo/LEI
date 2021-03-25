/** @file filesystem.c
 *
 * This file implements a FUSE filesystem with added tracing features.
 * All traces are stored /var/log/trace.log and will be further exported t
 * o Elastic Search to perform data analysis.
 *
 */

#define FUSE_USE_VERSION 31

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define _GNU_SOURCE

#ifdef linux
/* For pread()/pwrite()/utimensat() */
#define _XOPEN_SOURCE 700
#endif

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include "sb.h"
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <errno.h>
#ifdef __FreeBSD__
#include <sys/socket.h>
#include <sys/un.h>
#endif
#include <sys/time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif
#define PORT 12355
#include "filesystem_helpers.h"
#include "trace_log.h"
#include <time.h>
int sock;
static void *fs_init(struct fuse_conn_info *conn,
                      struct fuse_config *cfg)
{
    (void) conn;
    cfg->use_ino = 1;
    cfg->entry_timeout = 0;
    cfg->attr_timeout = 0;
    cfg->negative_timeout = 0;

    return (int *) fuse_get_context()->private_data;
}

static int fs_getattr(const char *path, struct stat *stbuf,
                       struct fuse_file_info *fi)
{
    (void) fi;
    int res;

    res = lstat(path, stbuf);
    if (res == -1)
        return -errno;

    return 0;
}

static int fs_access(const char *path, int mask)
{
    struct timespec tstart={0,0}, tend={0,0};
    int res;
    StringBuilder *sb = sb_create();
    int maybeError=0;
    clock_gettime(CLOCK_MONOTONIC,&tstart);
    res = access(path, mask);
    if (res == -1)
        maybeError=-errno;
    clock_gettime(CLOCK_MONOTONIC, &tend);
    sb_appendf(sb,"access %d %.5f %.5f %d %s %d",fuse_get_context()->pid,(double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec,(double)tend.tv_sec + 1.0e-9*tend.tv_nsec,maybeError,strdup(path),mask);
    char * toSend=sb_concat(sb);
    int * c = (int *) fuse_get_context()->private_data;
    send(*c,toSend,strlen(toSend),0);
    sb_free(sb);
    return maybeError;
}

static int fs_readlink(const char *path, char *buf, size_t size)
{
    int res;

    res = readlink(path, buf, size - 1);
    if (res == -1)
        return -errno;

    buf[res] = '\0';
    return 0;
}


static int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi,
                       enum fuse_readdir_flags flags)
{
    DIR *dp;
    struct dirent *de;
    (void) offset;
    (void) fi;
    (void) flags;
    dp = opendir(path);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0, FUSE_FILL_DIR_PLUS))
            break;
    }
    closedir(dp);
    return 0;
}

static int fs_mknod(const char *path, mode_t mode, dev_t rdev)
{
    struct timespec tstart={0,0}, tend={0,0};
    int res;
    StringBuilder *sb = sb_create();
    int maybeError=0;
    clock_gettime(CLOCK_MONOTONIC,&tstart);
    res = mknod(path,mode,rdev);
    if (res == -1)
        maybeError=-errno;
    clock_gettime(CLOCK_MONOTONIC, &tend);
    sb_appendf(sb,"mknod %d %.5f %.5f %d %s %d",fuse_get_context()->pid,(double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec,(double)tend.tv_sec + 1.0e-9*tend.tv_nsec,maybeError,strdup(path),mode);
    char * toSend=sb_concat(sb);
    int * c = (int *) fuse_get_context()->private_data;
    send(*c,toSend,strlen(toSend),0);
    sb_free(sb);
    return maybeError;
}

static int fs_mkdir(const char *path, mode_t mode)
{
    int res;
    res = mkdir(path, mode);
    if (res == -1)
        return -errno;

    return 0;
}

static int fs_unlink(const char *path)
{
    int res;

    res = unlink(path);
    if (res == -1)
        return -errno;

    return 0;
}

static int fs_rmdir(const char *path)
{
    int res;

    res = rmdir(path);
    if (res == -1)
        return -errno;

    return 0;
}

static int fs_symlink(const char *from, const char *to)
{
    int res;

    res = symlink(from, to);
    if (res == -1)
        return -errno;

    return 0;
}

static int fs_rename(const char *from, const char *to, unsigned int flags)
{
    int res;

    if (flags)
        return -EINVAL;

    res = rename(from, to);
    if (res == -1)
        return -errno;

    return 0;
}

static int fs_link(const char *from, const char *to)
{
    int res;

    res = link(from, to);
    if (res == -1)
        return -errno;

    return 0;
}

static int fs_chmod(const char *path, mode_t mode,
                     struct fuse_file_info *fi)
{
    (void) fi;
    int res;

    res = chmod(path, mode);
    if (res == -1)
        return -errno;

    return 0;
}

static int fs_chown(const char *path, uid_t uid, gid_t gid,
                     struct fuse_file_info *fi)
{
    (void) fi;
    int res;

    res = lchown(path, uid, gid);
    if (res == -1)
        return -errno;

    return 0;
}

static int fs_truncate(const char *path, off_t size,
                        struct fuse_file_info *fi)
{
    int res;

    if (fi != NULL)
        res = ftruncate(fi->fh, size);
    else
        res = truncate(path, size);
    if (res == -1)
        return -errno;

    return 0;
}

#ifdef HAVE_UTIMENSAT
static int fs_utimens(const char *path, const struct timespec ts[2],
		       struct fuse_file_info *fi)
{
	(void) fi;
	int res;

	/* don't use utime/utimes since they follow symlinks */
	res = utimensat(0, path, ts, AT_SYMLINK_NOFOLLOW);
	if (res == -1)
		return -errno;

	return 0;
}
#endif

static int fs_create(const char *path, mode_t mode,
                      struct fuse_file_info *fi)
{
    int res;

    res = open(path, fi->flags, mode);
    if (res == -1)
        return -errno;

    fi->fh = res;
    return 0;
}

static int fs_open(const char *path, struct fuse_file_info *fi)
{
    struct timespec tstart={0,0}, tend={0,0};
    int res;
    StringBuilder *sb = sb_create();
    int maybeError=0;
    clock_gettime(CLOCK_MONOTONIC,&tstart);
    res = open(path, fi->flags);
    if (res == -1)
        maybeError=-errno;
    fi->fh = res;
    clock_gettime(CLOCK_MONOTONIC, &tend);
    sb_appendf(sb,"open %d %.5f %.5f %d %s %d",fuse_get_context()->pid,(double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec,(double)tend.tv_sec + 1.0e-9*tend.tv_nsec,maybeError,strdup(path),fi->flags);
    char * toSend=sb_concat(sb);
    int * c = (int *) fuse_get_context()->private_data;
    send(*c,toSend,strlen(toSend),0);
    sb_free(sb);
    return maybeError;
}

static int fs_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi)
{
    int fd;
    struct timespec tstart={0,0}, tend={0,0};
    int res;
    StringBuilder *sb = sb_create();
    int maybeError=0;
    clock_gettime(CLOCK_MONOTONIC,&tstart);
    if(fi == NULL)
        fd = open(path, O_RDONLY);
    else
        fd = fi->fh;
    if (fd == -1)
        maybeError=-errno;
    else if (fi==NULL)
        close(fd);
        else
            res = pread(fd, buf, size, offset);
    clock_gettime(CLOCK_MONOTONIC, &tend);
    sb_appendf(sb,"pread %d %.5f %.5f %d %s %d %d",fuse_get_context()->pid,(double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec,(double)tend.tv_sec + 1.0e-9*tend.tv_nsec,maybeError,strdup(path),size,offset);
    char * toSend=sb_concat(sb);
    int * c = (int *) fuse_get_context()->private_data;
    send(*c,toSend,strlen(toSend),0);
    sb_free(sb);
    return res;
}

static int fs_write(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi)
{
    int fd;
    struct timespec tstart={0,0}, tend={0,0};
    int res;
    StringBuilder *sb = sb_create();
    int maybeError=0;
    clock_gettime(CLOCK_MONOTONIC,&tstart);
    if(fi == NULL)
        fd = open(path, O_RDONLY);
    else
        fd = fi->fh;
    if (fd == -1)
        maybeError=-errno;
    else if (fi==NULL)
        close(fd);
        else
            res = pwrite(fd, strdup(buf), size, offset);
    clock_gettime(CLOCK_MONOTONIC, &tend);
    sb_appendf(sb,"pwrite %d %.5f %.5f %d %s %d %d",fuse_get_context()->pid,(double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec,(double)tend.tv_sec + 1.0e-9*tend.tv_nsec,maybeError,strdup(path),size,offset);
    char * toSend=sb_concat(sb);
    int * c = (int *) fuse_get_context()->private_data;
    send(*c,toSend,strlen(toSend),0);
    sb_free(sb);
    return res;
}

static int fs_statfs(const char *path, struct statvfs *stbuf)
{
    int res;

    res = statvfs(path, stbuf);
    if (res == -1)
        return -errno;

    return 0;
}

static int fs_release(const char *path, struct fuse_file_info *fi)
{
    (void) path;
    close(fi->fh);
    return 0;
}

static int fs_fsync(const char *path, int isdatasync,
                     struct fuse_file_info *fi)
{
    /* Just a stub.	 This method is optional and can safely be left
       unimplemented */

    (void) path;
    (void) isdatasync;
    (void) fi;
    return 0;
}

#ifdef HAVE_POSIX_FALLOCATE
static int fs_fallocate(const char *path, int mode,
			off_t offset, off_t length, struct fuse_file_info *fi)
{
	int fd;
	int res;

	(void) fi;

	if (mode)
		return -EOPNOTSUPP;

	if(fi == NULL)
		fd = open(path, O_WRONLY);
	else
		fd = fi->fh;

	if (fd == -1)
		return -errno;

	res = -posix_fallocate(fd, offset, length);

	if(fi == NULL)
		close(fd);
	return res;
}
#endif

#ifdef HAVE_SETXATTR
/* xattr operations are optional and can safely be left unimplemented */
static int fs_setxattr(const char *path, const char *name, const char *value,
			size_t size, int flags)
{
	int res = lsetxattr(path, name, value, size, flags);
	if (res == -1)
		return -errno;
	return 0;
}

static int fs_getxattr(const char *path, const char *name, char *value,
			size_t size)
{
	int res = lgetxattr(path, name, value, size);
	if (res == -1)
		return -errno;
	return res;
}

static int fs_listxattr(const char *path, char *list, size_t size)
{
	int res = llistxattr(path, list, size);
	if (res == -1)
		return -errno;
	return res;
}

static int fs_removexattr(const char *path, const char *name)
{
	int res = lremovexattr(path, name);
	if (res == -1)
		return -errno;
	return 0;
}
#endif /* HAVE_SETXATTR */

#ifdef HAVE_COPY_FILE_RANGE
static ssize_t fs_copy_file_range(const char *path_in,
				   struct fuse_file_info *fi_in,
				   off_t offset_in, const char *path_out,
				   struct fuse_file_info *fi_out,
				   off_t offset_out, size_t len, int flags)
{
	int fd_in, fd_out;
	ssize_t res;

	if(fi_in == NULL)
		fd_in = open(path_in, O_RDONLY);
	else
		fd_in = fi_in->fh;

	if (fd_in == -1)
		return -errno;

	if(fi_out == NULL)
		fd_out = open(path_out, O_WRONLY);
	else
		fd_out = fi_out->fh;

	if (fd_out == -1) {
		close(fd_in);
		return -errno;
	}

	res = copy_file_range(fd_in, &offset_in, fd_out, &offset_out, len,
			      flags);
	if (res == -1)
		res = -errno;

	if (fi_out == NULL)
		close(fd_out);
	if (fi_in == NULL)
		close(fd_in);

	return res;
}
#endif

static off_t fs_lseek(const char *path, off_t off, int whence, struct fuse_file_info *fi)
{
    int fd;
    off_t res;
    addTrace(strdup("fez lseek"));
    if (fi == NULL)
        fd = open(path, O_RDONLY);
    else
        fd = fi->fh;

    if (fd == -1)
        return -errno;

    res = lseek(fd, off, whence);
    if (res == -1)
        res = -errno;

    if (fi == NULL)
        close(fd);
    return res;
}

static const struct fuse_operations fs_oper = {
        .init       = fs_init,
        .getattr	= fs_getattr,
        .access		= fs_access,
        .readlink	= fs_readlink,
        .readdir	= fs_readdir,
        .mknod		= fs_mknod,
        .mkdir		= fs_mkdir,
        .symlink	= fs_symlink,
        .unlink		= fs_unlink,
        .rmdir		= fs_rmdir,
        .rename		= fs_rename,
        .link		= fs_link,
        .chmod		= fs_chmod,
        .chown		= fs_chown,
        .truncate	= fs_truncate,
#ifdef HAVE_UTIMENSAT
        .utimens	= xmp_utimens,
#endif
        .open		= fs_open,
        .create 	= fs_create,
        .read		= fs_read,
        .write		= fs_write,
        .statfs		= fs_statfs,
        .release	= fs_release,
        .fsync		= fs_fsync,
#ifdef HAVE_POSIX_FALLOCATE
        .fallocate	= xmp_fallocate,
#endif
#ifdef HAVE_SETXATTR
        .setxattr	= fs_setxattr,
	    .getxattr	= fs_getxattr,
	    .listxattr	= fs_listxattr,
	    .removexattr= fs_removexattr,
#endif
#ifdef HAVE_COPY_FILE_RANGE
        .copy_file_range = fs_copy_file_range,
#endif
        .lseek		= fs_lseek,
};

int main(int argc, char *argv[])
{
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
    umask(0);
    int * socket_info = malloc(sizeof(int));
    *socket_info=sock;
    StringBuilder *sb = sb_create();
    sb_appendf(sb,"--%p--",socket_info);
    addTrace(sb_concat(sb));
    return fuse_main(argc, argv, &fs_oper,socket_info);
}