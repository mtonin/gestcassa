#ifndef DETTAGLIARTICOLOMENU_H
#define DETTAGLIARTICOLOMENU_H

#include "ui_dettagliarticolomenu.h"
#include <QStandardItemModel>
#include <QComboBox>

class DettagliArticoloMenu : public QWidget, private Ui::DettagliArticoloMenu
{
    Q_OBJECT
    
  public:
    explicit DettagliArticoloMenu(QWidget *parent = 0);
  private slots:
    void cancella();
    void on_toolButton_3_clicked();

    void on_articoliList_customContextMenuRequested(const QPoint &pos);

  private:
    QStandardItemModel* modello;
    QComboBox* creaDestinazioneBox();
};

#endif // DETTAGLIARTICOLOMENU_H
