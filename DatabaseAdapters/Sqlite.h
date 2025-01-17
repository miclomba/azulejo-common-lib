/**
 * @file Sqlite.h
 * @brief Declaration of the Sqlite class for managing SQLite database interactions.
 */

#ifndef database_adapters_sqlite_h
#define database_adapters_sqlite_h

#include "config.h"

#include <functional>
#include <string>
#include "Config/filesystem.h"

#include <boost/optional.hpp>

#include "sqlite3.h"

namespace database_adapters {

/**
 * @class Sqlite
 * @brief A helper class for managing SQLite database operations.
 */
class DATABASE_ADAPTERS_DLL_EXPORT Sqlite
{
public:
    /**
     * @brief Type definition for a callback function used to process database rows.
     */
    using RowCallbackType = std::function<int(int numCols, char** colValues, char** colNames)>;

    /**
     * @brief Default constructor for the Sqlite class.
     */
    Sqlite();

    /**
     * @brief Copy constructor.
     * @param other The Sqlite instance to copy from.
     */
    Sqlite(const Sqlite& other);

    /**
     * @brief Copy assignment operator.
     * @param other The Sqlite instance to copy from.
     * @return Reference to the updated Sqlite instance.
     */
    Sqlite& operator=(const Sqlite& other);

    /**
     * @brief Move constructor.
     * @param other The Sqlite instance to move from.
     */
    Sqlite(Sqlite&& other);

    /**
     * @brief Move assignment operator.
     * @param other The Sqlite instance to move from.
     * @return Reference to the updated Sqlite instance.
     */
    Sqlite& operator=(Sqlite&& other);

    /**
     * @brief Destructor for the Sqlite class.
     */
    virtual ~Sqlite();

    /**
     * @brief Execute an SQL command.
     * @param sql The SQL command to execute.
     * @param rowCallback Optional callback function to process query rows.
     */
    void Execute(const std::string& sql, boost::optional<RowCallbackType> rowCallback = boost::none);

    /**
     * @brief Check if a database is currently open.
     * @return True if the database is open, false otherwise.
     */
    bool IsOpen() const;

    /**
     * @brief Open a SQLite database file.
     * @param dbPath The path to the SQLite database file.
     */
    void Open(const Path& dbPath);

    /**
     * @brief Close the currently opened database.
     */
    void Close();

    /**
     * @brief Get the path of the currently opened database file.
     * @return The path to the SQLite database file.
     */
    Path GetPath() const;

    /**
     * @brief Get the raw SQLite3 database pointer.
     * @return Pointer to the SQLite3 database.
     */
    sqlite3* GetSqlite3();

private:
    /**
     * @brief Free an error message returned by SQLite.
     * @param ec Pointer to the error message.
     */
    void FreeErrorMessage(char* const ec);

    /** @brief Pointer to the SQLite3 database. */
    sqlite3* db_{ nullptr };

    /** @brief Path to the SQLite database file. */
    Path dbPath_;
};

} // end namespace database_adapters

#endif //  database_adapters_sqlite_h