#ifndef intraprocess_irunnable_h
#define intraprocess_irunnable_h

#include <memory>
#include <mutex>
#include <thread>

#include "config.h"

namespace intraprocess {

class INTRAPROCESS_DLL_EXPORT IRunnable 
{
public:
	IRunnable();
	virtual ~IRunnable();

	IRunnable(const IRunnable&) = delete;
	IRunnable& operator=(const IRunnable&) = delete;
	IRunnable(IRunnable&&) = delete;
	IRunnable& operator=(IRunnable&&) = delete;

	void Start();
	void Join();

protected:
	virtual void Run() = 0;

private:
	std::mutex mtx_;
	std::shared_ptr<std::thread> thread_;
};

} // namespace intraprocess

#endif // intraprocess_irunnable_h
