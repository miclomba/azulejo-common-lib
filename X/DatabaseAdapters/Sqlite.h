#ifndef database_adapters_sqlite_h
#define database_adapters_sqlite_h

#include "config.h"

#include <filesystem>
#include <functional>
#include <string>

#include <boost/optional.hpp>

// forward decl
struct sqlite3;

namespace database_adapters {

class DATABASE_ADAPTERS_DLL_EXPORT Sqlite
{
public:
	using RowCallbackType = std::function<int(int numCols, char** colValues, char** colNames)>;

	Sqlite();
	Sqlite(const Sqlite&);
	Sqlite& operator=(const Sqlite&);
	Sqlite(Sqlite&&);
	Sqlite& operator=(Sqlite&&);
	~Sqlite();

	void Execute(const std::string& sql, boost::optional<RowCallbackType> rowCallback = boost::none);

	bool IsOpen() const;
	void Open(const std::filesystem::path& dbPath);
	void Close();
	std::filesystem::path GetPath() const;

	// TODO control access of this
	sqlite3* GetSqlite3();

private:
	void FreeErrorMessage(char* const ec);

	sqlite3* db_{ nullptr };
	std::filesystem::path dbPath_;
};

} // end namespace database_adapters

#endif //  database_adapters_sqlite_h 
