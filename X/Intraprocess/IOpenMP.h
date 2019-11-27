#ifndef intraprocess_iopenmp_h
#define intraprocess_iopenmp_h

#include "config.h"

namespace intraprocess {

class INTRAPROCESS_DLL_EXPORT IOpenMP
{
public:
	IOpenMP(const int numThreads, const bool setDynamic, const bool setNested);
	virtual ~IOpenMP();

	IOpenMP(const IOpenMP&);
	IOpenMP& operator=(const IOpenMP&);
	IOpenMP(IOpenMP&&);
	IOpenMP& operator=(IOpenMP&&);

	void Start();

protected:
	virtual void Run() = 0;

	int numThreads_;
	bool setDynamic_; 
	bool setNested_;
};

} // namespace intraprocess
#endif // intraprocess_iopenmp_h

