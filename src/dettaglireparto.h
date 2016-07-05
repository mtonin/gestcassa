#ifndef DETTAGLIREPARTO_H
#define DETTAGLIREPARTO_H

#include "ui_dettaglireparto.h"
#include "repartobtnwidget.h"
#include <QColor>
#include <QSqlDatabase>
#include <QFont>

class DettagliReparto : public QWidget, private Ui::dettagliReparto
{
    Q_OBJECT

signals:
    void repartoAggiornato();

public:
    explicit DettagliReparto(QWidget *parent = 0);

public slots:
    void setCurrentReparto(RepartoBtnWidget*);
    void aggiornaReparto();

private slots:
    void on_sfondoBtn_clicked();
    void on_carattereBtn_clicked();
    void on_fontBtn_clicked();
    void on_testoReparto_textEdited(const QString &arg1);
    void on_adattaFontFlag_clicked(bool checked);
    void on_disattivaFlag_clicked(bool checked);

private:
    RepartoBtnWidget* repartoBtn;
};

#endif // DETTAGLIREPARTO_H
