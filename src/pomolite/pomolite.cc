#include "pomolite.h"

#include <utility>
#include <vector>

PomoLite::PomoLite(const std::string& filename)
  : filename_(filename)
{
  rc_ = sqlite3_open(filename_.c_str(), &db_);
  if (check_rc()) {
    report_info("Opened database at \"" + filename + "\".");
    opened_ = true;
  } else {
    report_error("Failed to open database at \"" + filename + "\".");
  }
}

PomoLite::~PomoLite()
{
  if (opened_) {
    report_info("Closing database \"" + filename_ + "\".");
    rc_ = sqlite3_close(db_);
  }
}

/**
 * @brief Name of info table in SQLite DB
 */
const std::string PomoLite::TABLE_INFO = "db_info";

/**
 * @brief Name of activity type table in SQLite DB
 */
const std::string PomoLite::TABLE_ACTIVITY_TYPE = "activity_type";

/**
 * @brief Name of session table in SQLite DB
 */
const std::string PomoLite::TABLE_SESSION = "session";

/**
 * @brief Name of user table in SQLite DB
 */
const std::string PomoLite::TABLE_USER = "user";

/**
 * @brief Column structure of info table in SQLite DB
 */
const std::string PomoLite::COLS_INFO =
    "property TEXT NOT NULL UNIQUE PRIMARY KEY,"
    "value TEXT NOT NULL";

/**
 * @brief Column structure of activity type table in SQLite DB
 */
const std::string PomoLite::COLS_ACTIVITY_TYPE =
    "id INTEGER NOT NULL PRIMARY KEY ASC AUTOINCREMENT,"
    "short_name TEXT NOT NULL UNIQUE,"
    "full_name TEXT NOT NULL,"
    "description TEXT";

/**
 * @brief Column structure of session table in SQLite DB
 */
const std::string PomoLite::COLS_SESSION =
    "id INTEGER NOT NULL PRIMARY KEY ASC AUTOINCREMENT,"
    "user_id INTEGER NOT NULL,"
    "activity_type_id INTEGER NOT NULL,"
    "start_time DATETIME NOT NULL,"
    "end_time DATETIME NOT NULL,"
    "rating INTEGER,"
    "FOREIGN KEY(user_id) REFERENCES user(id),"
    "FOREIGN KEY(activity_type_id) REFERENCES activity_type(id)";

/**
 * @brief Column structure of user table in SQLite DB
 */
const std::string PomoLite::COLS_USER =
    "id INTEGER NOT NULL PRIMARY KEY ASC AUTOINCREMENT,"
    "full_name TEXT NOT NULL UNIQUE";

/**
 * @brief Check whether an SQLite DB was opened
 * @return Whether an SQLite DB has been successfully opened
 */
bool PomoLite::opened() const
{
  return opened_;
}

/**
 * @brief Get DB filename
 * @return Filename
 */
const std::string& PomoLite::filename() const
{
  return filename_;
}

/**
 * @brief Get version of database
 * @return PomoResult with version number
 */
const PomoResult<int> PomoLite::version() const
{
  // If not opened, return a fail value
  if (!opened_) {
    return PomoResult<int>();
  }

  const std::string stm_check_table = "SELECT value FROM " + TABLE_INFO + " WHERE property = 'version'";
  char* error_msg;
  rc_ = sqlite3_exec(db_,
                     stm_check_table.c_str(),
                     cb_version,
                     &version_,
                     &error_msg);

  if (version_ == VERSION_FAIL) {
    return PomoResult<int>();
  }

  return PomoResult<int>(PomoResultCode::SUCCESS, version_);
}

/**
 * @brief (Callback) Convenience callback that does nothing
 * @return SQLITE_OK
 */
int PomoLite::cb_do_nothing(void*, int, char**, char**)
{
  return SQLITE_OK;
}

/**
 * @brief (Callback) Set database version member variable
 * @param version Pointer to version
 * @param count Rows found in info table
 * @param values Values for found rows
 * @return SQLITE_OK if successful, else SQLITE_FAIL
 */
int PomoLite::cb_version(void* version, int count, char** values, char** col_names)
{
  auto version_ptr = reinterpret_cast<int*>(version);

  if (count == 0) {
    *version_ptr = VERSION_FAIL;
    return SQLITE_FAIL;
  }

  // Locate and set version number
  for (int i = 0; i < count; ++i) {
    if (std::string(col_names[i]) == "version") {
      try {
        *version_ptr = std::stoi(std::string(values[i]));
        return SQLITE_OK;
      } catch (...) {
        *version_ptr = VERSION_FAIL;
        return SQLITE_FAIL;
      }
    }
  }

  return SQLITE_FAIL;
}

/**
 * @brief Initialize SQLite DB to most modern iteration
 * @return SUCCESS if successful, else FAIL
 */
PomoResultCode PomoLite::initialize()
{
  // Make sure the database is opened
  if (!opened_) {
    report_error("No database opened.");

    return PomoResultCode::FAIL;
  }

  // Check to see if version is the most recent version available
  auto result = version();
  if (result.successful() && version_ == CURRENT_VERSION) {
    return PomoResultCode::SUCCESS;
  }

  // If version is fail, we need to create the DB
  bool failed = false;
  if (version_ == VERSION_FAIL) {
    // Create tables
    const std::vector<std::pair<std::string, std::string>> tables = {
      {TABLE_INFO, COLS_INFO},
      {TABLE_ACTIVITY_TYPE, COLS_ACTIVITY_TYPE},
      {TABLE_USER, COLS_USER},
      {TABLE_SESSION, COLS_SESSION}, // Depends: activity_type, user
    };

    // Create tables
    for (const auto& table : tables) {
      const std::string stm_create_info_table =
          "CREATE TABLE " + table.first + "(" + table.second + ");";
      char* error_msg;

      rc_ = sqlite3_exec(db_,
                         stm_create_info_table.c_str(),
                         cb_do_nothing,
                         nullptr,
                         &error_msg);

      if (!check_rc()) {
        failed = true;
        report_error("Creation of table \"" + table.first + "\" failed.");
      }
    }

    // Add version entry
    {
      const std::string stm_add_version =
          "INSERT INTO " + TABLE_INFO + "(property, value) VALUES"
          "('version', '" + std::to_string(CURRENT_VERSION) + "');";
      char* error_msg;

      rc_ = sqlite3_exec(db_,
                         stm_add_version.c_str(),
                         cb_do_nothing,
                         nullptr,
                         &error_msg);

      if (!check_rc()) {
        failed = true;
        report_error("Failed to set version number.");
      }
    }

    // Add default activity types
    {
      const std::string stm_add_activities =
          "INSERT INTO " + TABLE_ACTIVITY_TYPE + "(short_name, full_name, description) VALUES"
          "('work', 'Pomodoro', 'Productive work'),"
          "('short_break', 'Short Break', 'Short break between working bursts'),"
          "('long_break', 'Long Break', 'Take a breather!');";
      char* error_msg;

      rc_ = sqlite3_exec(db_,
                         stm_add_activities.c_str(),
                         cb_do_nothing,
                         nullptr,
                         &error_msg);

      if (!check_rc()) {
        failed = true;
        report_error("Failed to add default activities.");
      }
    }
  }

  // Any migrations that need to be done?
  // none yet

  // Check final result
  if (failed) {
    return PomoResultCode::FAIL;
  }

  return PomoResultCode::SUCCESS;
}

/**
 * @brief Check a PomoResultCode
 * @param code PomoResultCode to check
 * @return Whether the result code is SUCCESS
 */
bool PomoLite::successful(const PomoResultCode code) const
{
  return code == PomoResultCode::SUCCESS;
}

/**
 * @brief Check the return code and verify it is SQLITE_OK
 * @return Whether return code is SQLITE_OK
 */
bool PomoLite::check_rc() const
{
  return rc_ == SQLITE_OK;
}

/**
 * @brief Report an error from an operation
 * @param msg The message to report
 */
void PomoLite::report_error(const std::string& msg) const
{
  report(msg, ReportType::ERROR);
}

/**
 * @brief Report a warning from an operation
 * @param msg The message to report
 */
void PomoLite::report_warning(const std::string& msg) const
{
  report(msg, ReportType::ERROR);
}


/**
 * @brief Report an info message from an operation
 * @param msg The message to report
 */
void PomoLite::report_info(const std::string& msg) const
{
  report(msg, ReportType::INFO);
}

/**
 * @brief Report a message from an operation
 * @param msg The message to report
 * @param type The type of message to report
 */
void PomoLite::report(const std::string& msg, const ReportType type) const
{
  // Currently does nothing
  message_ = msg;
  report_type_ = type;

  return;
}

/**
 * @brief Get diagnostic messages, if any
 * @return Diagnostic message
 */
const std::string& PomoLite::message() const
{
  return message_;
}

/**
 * @brief Get the message type, if applicable
 * @return Message type as ReportType
 */
PomoLite::ReportType PomoLite::message_type() const
{
  return report_type_;
}
