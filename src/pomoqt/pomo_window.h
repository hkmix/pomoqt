#ifndef POMO_WINDOW_H
#define POMO_WINDOW_H

#include <QMainWindow>

namespace Ui {
  class PomoWindow;
}

class PomoWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit PomoWindow(QWidget *parent = 0);
  ~PomoWindow();

private:
  Ui::PomoWindow *ui;
};

#endif // POMO_WINDOW_H
