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
  void reset();

private slots:
  void on_articoliList_customContextMenuRequested(const QPoint &pos);

  void on_nuovoBtn_clicked();

  void on_cancellaBtn_clicked();

private:
  QStandardItemModel* modello;
  QComboBox* creaDestinazioneBox();
};

#endif // DETTAGLIARTICOLOMENU_H
