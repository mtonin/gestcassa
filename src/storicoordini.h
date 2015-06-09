#ifndef STORICOORDINI_H
#define STORICOORDINI_H

#include "storicoordinimodel.h"
#include "ui_storicoordini.h"

#include <QSqlQueryModel>

class StoricoOrdini : public QDialog, private Ui::StoricoOrdini
{
    Q_OBJECT

public:
    explicit StoricoOrdini(const int idSessione, QWidget *parent = 0);

private slots:
    void caricaArticoliOrdine();
    void on_filtraBtn_5_clicked();
    void on_filtroDateBox_toggled(bool checked);

    void on_sessioneBox_toggled(bool checked);

private:
    storicoOrdiniModel* ordiniModel;
    QSqlQueryModel* articoliOrdineModel;

    QString condizione;
    void caricaSessioni();
};

#endif // STORICOORDINI_H
