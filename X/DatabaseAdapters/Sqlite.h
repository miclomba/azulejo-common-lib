#ifndef database_adapters_sqlite_h
#define database_adapters_sqlite_h

#include "config.h"

#include <filesystem>

// forward decl
struct sqlite3;

namespace database_adapters {

class DATABASE_ADAPTERS_DLL_EXPORT Sqlite
{
public:
	Sqlite();
	Sqlite(const Sqlite&);
	Sqlite& operator=(const Sqlite&);
	Sqlite(Sqlite&&);
	Sqlite& operator=(Sqlite&&);
	~Sqlite();

	bool IsOpen() const;
	void Open(const std::filesystem::path& dbPath);
	void Close();

private:
	sqlite3* db_{ nullptr };
};

} // end namespace database_adapters

#endif //  database_adapters_sqlite_h 
