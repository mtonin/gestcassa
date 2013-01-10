#include <QApplication>
#include <QDesktopWidget>
#include <QRect>
#include <QFontDialog>
#include <QColorDialog>
#include "dettagliorepartodlg.h"

DettaglioRepartoDlg::DettaglioRepartoDlg(QWidget *parent) :
  QDialog(parent)
{
  setupUi(this);
  setWindowFlags(Qt::Tool);
  activateWindow();
}

int DettaglioRepartoDlg::showAtMousePosition() {

  QRect screenRect=QApplication::desktop()->geometry();
  move(QCursor::pos());
  if(frameGeometry().x()+frameGeometry().width()>screenRect.width())
    move(QCursor::pos().x()-frameGeometry().width(),QCursor::pos().y());
  return exec();
}

void DettaglioRepartoDlg::on_pushButton_clicked()
{
    QFontDialog dlg;
    dlg.setCurrentFont(currentFont);
    if(dlg.exec()) {
        currentFont=dlg.currentFont();
      }
}

void DettaglioRepartoDlg::on_toolButton_clicked()
{
    QColorDialog dlg;
    QColor colore;
    colore.setNamedColor(currentColor);
    dlg.setCurrentColor(currentColor);
    if(dlg.exec()) {
        currentColor=dlg.currentColor().name();
      }
}
