#ifndef LOCK_H_
#define LOCK_H_
#include <iostream>
namespace json_data
{
    bool read_lock(int fd);
    bool read_lock_wait(int fd);
    bool write_lock(int fd);
    bool write_lock_wait(int fd);
    bool unlock(int fd);
}
#endif