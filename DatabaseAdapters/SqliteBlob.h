/**
 * @file SqliteBlob.h
 * @brief Declaration of the SqliteBlob class for handling SQLite BLOB operations.
 */

#ifndef database_adapters_sqliteblob_h
#define database_adapters_sqliteblob_h

#include "config.h"

#include <string>
#include <vector>

#include "sqlite3.h"

namespace database_adapters {

class Sqlite;

/**
 * @class SqliteBlob
 * @brief A class for managing SQLite BLOB (Binary Large Object) operations.
 *
 * Provides methods for reading, writing, and managing BLOB data in SQLite databases.
 */
class DATABASE_ADAPTERS_DLL_EXPORT SqliteBlob
{
public:
    /**
     * @brief Constructor for the SqliteBlob class.
     * @param db Reference to an Sqlite instance managing the database connection.
     */
    SqliteBlob(Sqlite& db);

    /**
     * @brief Deleted copy constructor to prevent copying.
     */
    SqliteBlob(const SqliteBlob&) = delete;

    /**
     * @brief Deleted copy assignment operator to prevent copying.
     * @return Reference to the updated instance (not used).
     */
    SqliteBlob& operator=(const SqliteBlob&) = delete;

    /**
     * @brief Deleted move constructor to prevent moving.
     */
    SqliteBlob(SqliteBlob&&) = delete;

    /**
     * @brief Deleted move assignment operator to prevent moving.
     * @return Reference to the updated instance (not used).
     */
    SqliteBlob& operator=(SqliteBlob&&) = delete;

    /**
     * @brief Destructor for the SqliteBlob class.
     */
    virtual ~SqliteBlob();

    /**
     * @brief Open a BLOB in the database for operations.
     * @param tableName The name of the table containing the BLOB.
     * @param columnName The name of the column containing the BLOB.
     * @param iRow The row ID of the BLOB to open.
     */
    void Open(const std::string& tableName, const std::string& columnName, const sqlite3_int64 iRow);

    /**
     * @brief Read data from the BLOB.
     * @param numBytes The number of bytes to read.
     * @param offset The offset from which to start reading.
     * @return A vector containing the read bytes.
     */
    std::vector<char> Read(const size_t numBytes, const int offset) const;

    /**
     * @brief Write data to the BLOB.
     * @param buff Pointer to the data buffer to write.
     * @param numBytes The number of bytes to write.
     * @param offset The offset from which to start writing.
     */
    void Write(const char* buff, const size_t numBytes, const int offset);

    /**
     * @brief Insert a new BLOB into the database.
     * @param db Reference to an Sqlite instance managing the database connection.
     * @param sql The SQL statement to execute for inserting the BLOB.
     * @param data Pointer to the BLOB data.
     * @param size The size of the BLOB data in bytes.
     */
    static void InsertBlob(Sqlite& db, const std::string& sql, const void* data, const size_t size);

private:
    /** @brief Pointer to the SQLite3 blob object. */
    sqlite3_blob* blob_{ nullptr };

    /** @brief Reference to the Sqlite instance managing the database connection. */
    Sqlite& db_;
};

} // end namespace database_adapters

#endif //  database_adapters_sqliteblob_h