#include "dettaglireparto.h"

#include <QColorDialog>

dettagliReparto::dettagliReparto(QWidget *parent) :
   QWidget(parent)
   {
      setupUi(this);
      connect(testoReparto,SIGNAL(textChanged(QString)),this,SIGNAL(testoChanged(QString)));
}

void dettagliReparto::setColore(const QColor &colore)
{
    coloreBtn->SetButtonColorNormal(colore);
}

void dettagliReparto::setCurrentReparto(RepartoBtnWidget *currentRepartoBtn){

      testoReparto->setText(currentRepartoBtn->getNomeReparto());
      coloreBtn->SetButtonColorNormal(currentRepartoBtn->getColore());
   }

void dettagliReparto::on_coloreBtn_clicked()
{
      QColorDialog dlg;
      QColor colore;
      colore.setNamedColor(currentColore);
      dlg.setCurrentColor(currentColore);
      if(dlg.exec()) {
          colore=dlg.currentColor();
          currentColore=colore.name();
          coloreBtn->SetButtonColorNormal(colore);
          emit coloreChanged(colore);
        }

}
