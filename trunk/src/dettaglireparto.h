#ifndef DETTAGLIREPARTO_H
#define DETTAGLIREPARTO_H

#include "ui_dettaglireparto.h"
#include "repartobtnwidget.h"
#include <QColor>
#include <QSqlDatabase>

class dettagliReparto : public QWidget, private Ui::dettagliReparto {
   Q_OBJECT
   
signals:
    void testoChanged(const QString& testo);
    void coloreChanged(const QColor& colore);

public:
   explicit dettagliReparto(QWidget *parent = 0);
    void setColore(const QColor& colore);

public slots:
   void setCurrentReparto(RepartoBtnWidget*);

private slots:
   void on_coloreBtn_clicked();

private:
   QString currentColore;
   QSqlQuery* query;

};

#endif // DETTAGLIREPARTO_H
