#include "articolobtnwidget.h"

ArticoloBtnWidget::ArticoloBtnWidget(QWidget *parent) :
   QPictureButton(parent)
   {
   }

void ArticoloBtnWidget::setNomeArticolo(const QString nome){
    nomeArticolo=nome;
    setText(nomeArticolo);
   }
