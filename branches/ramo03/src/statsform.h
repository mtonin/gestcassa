#ifndef STATSFORM_H
#define STATSFORM_H

#include "ui_statsform.h"
#include <statsmodel.h>

#include <QMap>

class StatsForm : public QDialog, private Ui::StatsForm
{
    Q_OBJECT

public:
    explicit StatsForm(QMap<QString, QVariant>* par, QWidget *parent = 0);
    ~StatsForm();

    void stampa(const QTextDocument *doc, const QString descrReport, bool preview);

    void putHeader(QTextCursor cursore, const QString testo);
    void formattaTabella(QTextTable *tabella);

private slots:
    void on_filtraBtn_clicked();
    void ordinaByColumn(int column);
    void on_stampaBtn_clicked();

private:
    StatsModel* statsModel;
    void caricaStats();
    void calcolaTotali();

    int idSessioneCorrente;
    QMap<QString, QVariant>* configurazione;
    QString tsInizio;
    QString tsFine;
    int totOrdini;
    float totImporto;

};

#endif // STATSFORM_H
