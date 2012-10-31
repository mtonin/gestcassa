#ifndef FORM01_H
#define FORM01_H

#include <QWidget>

namespace Ui {
  class Form01;
}

class Form01 : public QWidget
{
  Q_OBJECT
  
public:
  explicit Form01(QWidget *parent = 0);
  ~Form01();
  
private:
  Ui::Form01 *ui;
};

#endif // FORM01_H
