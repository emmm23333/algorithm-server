#ifndef ALGORITHMSERVICE_BUSINESS_BUSINESS_H_
#define ALGORITHMSERVICE_BUSINESS_BUSINESS_H_

#include <stdint.h>
#include <string>
#include <memory>
#include <atomic>
#include <ustd/singleton.h>
#include <business/task_handle.h>
#include <business/config.h>
namespace algorithmservice
{
namespace business
{

class BusinessMgr
{

public:
    bool init(const algorithmservice::business::Config &config);
    void uinit();

private:
};

using SingletonBusinessMgr = ustd::Singleton<BusinessMgr>;

} // namespace business
} // namespace algorithmservice

#endif //!ALGORITHMSERVICE_BUSINESS_BUSINESS_H_