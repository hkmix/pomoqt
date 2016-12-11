#include "pomo_window.h"
#include "ui_pomo_window.h"

#include <string>

#include "pomolite/pomolite.h"

PomoWindow::PomoWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::PomoWindow)
{
  ui->setupUi(this);

  // Test DB
  const std::string db_path = "/tmp/test.db";

  PomoLite db{db_path};
  db.initialize();

  switch (db.message_type()) {
    case PomoLite::ReportType::NONE:
    case PomoLite::ReportType::INFO:
      setWindowTitle("Success");
      break;

    case PomoLite::ReportType::WARNING:
      setWindowTitle(std::string("Warning: " + db.message()).c_str());
      break;

    case PomoLite::ReportType::ERROR:
      setWindowTitle(std::string("Error: " + db.message()).c_str());
      break;
  }
}

PomoWindow::~PomoWindow()
{
  delete ui;
}
