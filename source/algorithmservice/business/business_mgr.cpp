#include <business/business_mgr.h>
#include <business/task_handle.h>
#include <version.h>
#include <ustd/log.h>
#include <common/process_pool.h>

namespace algorithmservice
{
namespace business
{

bool init(const algorithmservice::business::Config &config)
{
	return SingletonBusinessMgr::instance().init(config);
}
void uinit()
{
	SingletonBusinessMgr::instance().uinit();
}

bool BusinessMgr::init(const algorithmservice::business::Config &config)
{
	// first create process pool to reduce memory waste
	log_print(kDebug, "businessMgr init!");

	ustd::processpool<TaskHandler, ScheHandler> *pool = ustd::processpool<TaskHandler, ScheHandler>::create(4);
	if (pool)
	{
		pool->run(config);
	}
	return true;
}

void BusinessMgr::uinit()
{
}

} // namespace business
} // namespace algorithmservice