#ifndef database_adapters_sqliteblob_h
#define database_adapters_sqliteblob_h

#include "config.h"

#include <string>
#include <vector>

#include "sqlite3.h"

namespace database_adapters {

class Sqlite;

class DATABASE_ADAPTERS_DLL_EXPORT SqliteBlob
{
public:
	SqliteBlob(Sqlite& db);

	SqliteBlob(const SqliteBlob&);
	SqliteBlob& operator=(const SqliteBlob&);
	SqliteBlob(SqliteBlob&&);
	SqliteBlob& operator=(SqliteBlob&&);
	~SqliteBlob();

	void Open(const std::string& tableName, const std::string& columnName, const sqlite3_int64 iRow);
	std::vector<char> Read(const size_t numBytes, const int offset) const;
	void Write(const char* buff, const size_t numBytes, const int offset);
	
	static void InsertBlob(Sqlite& db, const std::string& sql, const void* data, const size_t size);

private:
	sqlite3_blob* blob_{ nullptr };
	Sqlite& db_;
};

} // end namespace database_adapters

#endif //  database_adapters_sqliteblob_h 
