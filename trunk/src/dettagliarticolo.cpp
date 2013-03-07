#include "dettagliarticolo.h"

DettagliArticolo::DettagliArticolo(QWidget *parent) :
   QWidget(parent)
   {
      setupUi(this);
      connect(testoArticolo,SIGNAL(textChanged(QString)),this,SLOT(testoChanged(QString)));
   }

void DettagliArticolo::setCurrentArticolo(const ArticoloBtnWidget *currentArticoloBtn){
      articoloBtn=(ArticoloBtnWidget*)currentArticoloBtn;
      testoArticolo->setText(articoloBtn->getNomeArticolo());
   }

void DettagliArticolo::testoChanged(const QString& testo)
   {
      articoloBtn->setNomeArticolo(testo);
   }
