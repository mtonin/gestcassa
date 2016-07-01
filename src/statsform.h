#ifndef STATSFORM_H
#define STATSFORM_H

#include "ui_statsform.h"
#include <statsmodel.h>

#include <QMap>

class StatsForm : public QDialog, private Ui::StatsForm
{
    Q_OBJECT

public:
    explicit StatsForm(int idSessione, QMap<QString, QVariant>* par, QWidget *parent = 0);
    ~StatsForm();

    void stampa(const QTextDocument *doc, const QString descrReport, bool preview);

    void putHeader(QTextCursor cursore, const QString testo);
    void formattaTabella(QTextTable *tabella);

private slots:
    void on_filtraBtn_clicked();
    void ordinaByColumn(int column);
    void on_stampaBtn_clicked();

    void xCheckRange(const QCPRange &newRange, const QCPRange &oldRange);
    void yCheckRange(const QCPRange &newRange, const QCPRange &oldRange);
    void mouseClick(QMouseEvent* evt);

    void on_sessioneBox_toggled(bool checked);

    void xAxisChanged(QCPRange range);
    void yAxisChanged(QCPRange range);
private:
    StatsModel* statsModel;
    void caricaStats();
    void calcolaTotali();
    void caricaSessioni();

    int idSessioneCorrente;
    QMap<QString, QVariant>* configurazione;
    QString tsInizio;
    QString tsFine;
    int totOrdini;
    float totImporto;
    double maxXAxis;
    double maxYAxis;

    void impostaLabel(QString valore, QCustomPlot *customPlot, qreal x, qreal y);
    void creaGrafico(const QColor &colore, const QVector<double> chiavi, const QVector<double> valori);
};

#endif // STATSFORM_H
