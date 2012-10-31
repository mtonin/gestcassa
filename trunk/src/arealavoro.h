#ifndef AREALAVORO_H
#define AREALAVORO_H

#include "ui_arealavoro.h"

class AreaLavoro : public QWidget, private Ui::AreaLavoro
{
  Q_OBJECT

public:
  explicit AreaLavoro(QWidget *parent = 0);
};

#endif // AREALAVORO_H
