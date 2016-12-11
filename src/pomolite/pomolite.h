#ifndef POMOLITE_H
#define POMOLITE_H

#include <sqlite3.h>

#include <string>

#include "pomoresult.h"

class PomoLite {
public:
  static constexpr int CURRENT_VERSION = 1;

  // Table names
  static const std::string TABLE_INFO;
  static const std::string TABLE_ACTIVITY_TYPE;
  static const std::string TABLE_USER;
  static const std::string TABLE_SESSION;

  // Table columns
  static const std::string COLS_INFO;
  static const std::string COLS_ACTIVITY_TYPE;
  static const std::string COLS_USER;
  static const std::string COLS_SESSION;

  // Column names
  static const std::string TABLE_INFO_COLUMN_VERSION;

  // Report message type
  enum class ReportType {
    NONE,
    INFO,
    WARNING,
    ERROR,
  };

public:
  PomoLite(const std::string& filename);
  virtual ~PomoLite();

  // Disable copy construction and assignment
  PomoLite(const PomoLite&) = delete;
  PomoLite& operator=(const PomoLite&) = delete;

  // Basic DB operations
  PomoResultCode initialize();

  // Accessors/simple checks
  const std::string& message() const;
  ReportType message_type() const;
  const std::string& filename() const;
  bool opened() const;

protected:
  static constexpr int VERSION_FAIL = -1;
  const std::string filename_;

  // Error reporting
  mutable std::string message_ = ""; // Report message, if applicable
  mutable ReportType report_type_ = ReportType::NONE;

  // SQLite
  bool opened_ = false;
  sqlite3* db_ = nullptr;
  mutable int rc_; // Return code for sqlite3 operations
  mutable int version_ = VERSION_FAIL; // Cached version number

  const PomoResult<int> version() const;

private:
  // Callbacks
  static int cb_do_nothing(void*, int, char**, char**);
  static int cb_version(void* version, int count, char** values, char**);

  // Helper functions
  bool check_rc() const;
  bool successful(const PomoResultCode code) const;

  inline void report_error(const std::string& msg) const;
  inline void report_warning(const std::string& msg) const;
  inline void report_info(const std::string& msg) const;
  void report(const std::string& msg, ReportType type) const;
};

#endif // POMOLITE_H
