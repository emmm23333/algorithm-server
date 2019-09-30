#include <fcntl.h>
#include <unistd.h>
#include "lock.h"
namespace json_data
{
    bool lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
    {
        struct flock lock;
        lock.l_type = type;
        lock.l_start = offset;
        lock.l_whence = whence;
        lock.l_len = len;
        return (fcntl(fd, cmd, &lock) != -1);
    }
    #define READ_LOCK(fd, offset, whence, len) \
        lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))
    #define READW_LOCK(fd, offset, whence, len) \
        lock_reg((fd), F_SETLKW, F_RDLCK, (offset), (whence), (len))
    #define WRITE_LOCK(fd, offset, whence, len) \
        lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))
    #define WRITEW_LOCK(fd, offset, whence, len) \
        lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))
    #define UN_LOCK(fd, offset, whence, len) \
        lock_reg((fd), F_SETLK, F_UNLCK, (offset), (whence), (len))

    bool read_lock(int fd)
    {
        return READ_LOCK(fd, 0, SEEK_SET, 0);
    }
    bool read_lock_wait(int fd)
    {
        return READW_LOCK(fd, 0, SEEK_SET, 0);
    }
    bool write_lock(int fd)
    {
        return WRITE_LOCK(fd, 0, SEEK_SET, 0);
    }
    bool write_lock_wait(int fd)
    {
        return WRITEW_LOCK(fd, 0, SEEK_SET, 0);
    }
    bool unlock(int fd)
    {
        return UN_LOCK(fd, 0, SEEK_SET, 0);
    }
}