#ifndef ALGORITHMSERVICE_BUSINESS_MODULE_API_H_
#define ALGORITHMSERVICE_BUSINESS_MODULE_API_H_
#include "config.h"
namespace algorithmservice
{
namespace business
{

bool init(const algorithmservice::business::Config &config);
void uinit();

} // namespace business
} // namespace algorithmservice

#endif // ALGORITHMSERVICE_BUSINESS_MODULE_API_H_