#include "pomo_window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  PomoWindow w;
  w.show();

  return a.exec();
}
