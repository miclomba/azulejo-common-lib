#ifndef intraprocess_iomp_runnable_h
#define intraprocess_iomp_runnable_h

#include <cstddef>

#include "config.h"

namespace intraprocess {

class INTRAPROCESS_DLL_EXPORT IOMPRunnable
{
public:
	IOMPRunnable(const size_t numThreads, const bool setDynamic = true, const bool setNested = true);
	virtual ~IOMPRunnable();

	IOMPRunnable(const IOMPRunnable&);
	IOMPRunnable& operator=(const IOMPRunnable&);
	IOMPRunnable(IOMPRunnable&&);
	IOMPRunnable& operator=(IOMPRunnable&&);

	void Start();

	size_t GetNumThreads() const;
	bool GetDynamicState() const;
	bool GetNestedState() const;

protected:
	virtual void Run() = 0;

	size_t numThreads_;
	bool setDynamic_; 
	bool setNested_;
};

} // namespace intraprocess
#endif // intraprocess_iomp_runnable_h

