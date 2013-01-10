#include "dettaglioelementodlg.h"
#include "ui_dettaglioelementodlg.h"

#include <QDesktopWidget>

dettaglioElementoDlg::dettaglioElementoDlg(QWidget *parent) :
  QDialog(parent)
{
  setupUi(this);
  setWindowFlags(Qt::FramelessWindowHint);
  activateWindow();
}

dettaglioElementoDlg::~dettaglioElementoDlg()
{
}

int dettaglioElementoDlg::showAtMousePosition() {

  QRect screenRect=QApplication::desktop()->geometry();
  move(QCursor::pos());
  if(frameGeometry().x()+frameGeometry().width()>screenRect.width())
    move(QCursor::pos().x()-frameGeometry().width(),QCursor::pos().y());
  return exec();
}


void dettaglioElementoDlg::on_buttonBox_accepted()
{
    accept();
}

void dettaglioElementoDlg::on_buttonBox_rejected()
{
    reject();
}
