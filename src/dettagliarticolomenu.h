#ifndef DETTAGLIARTICOLOMENU_H
#define DETTAGLIARTICOLOMENU_H

#include "ui_dettagliarticolomenu.h"
#include <QStandardItemModel>

class DettagliArticoloMenu : public QWidget, private Ui::DettagliArticoloMenu
{
    Q_OBJECT
    
  public:
    explicit DettagliArticoloMenu(QWidget *parent = 0);
  private slots:
    void on_toolButton_2_clicked();
    void on_toolButton_3_clicked();

    void on_toolButton_clicked();

  private:
    QStandardItemModel* modello;
};

#endif // DETTAGLIARTICOLOMENU_H
