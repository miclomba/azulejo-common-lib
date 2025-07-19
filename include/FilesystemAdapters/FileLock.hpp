#ifndef filesystem_adapters_filelock_h
#define filesystem_adapters_filelock_h

#include <mutex>

namespace filesystem_adapters
{
    inline std::recursive_mutex &GetGlobalFileLock()
    {
        static std::recursive_mutex mtx;
        return mtx;
    }
} // end namespace filesystem_adapters

#endif // filesystem_adapters_filelock_h