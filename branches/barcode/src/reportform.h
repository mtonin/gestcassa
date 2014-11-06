#ifndef REPORTFORM_H
#define REPORTFORM_H

#include "ui_reportform.h"
#include <QTextCursor>
#include <QMap>
class ReportForm : public QDialog, private Ui::ReportForm
{
    Q_OBJECT

public:
    explicit ReportForm(QMap<QString, QVariant>* par, QWidget *parent = 0);
private slots:
    void stampa(bool preview);

    void on_anteprimaBtn_clicked();

    void on_esportaBtn_clicked();
    void on_prenotazioniBtn_clicked();

private:
    QTextDocument* creaDocumentTutto();
    QTextDocument* creaDocumentPerReparti();
    QTextDocument* creaDocumentPerDestinazione();
    QTextDocument* creaDocumentMenu();
    QTextDocument* creaDocumentDestinazione(const QString& nomeDestinazione);
    QTextDocument* creaFoglioPrenotazioni();
    void stampa(const QTextDocument* doc, const QString descrReport, bool preview);
    void putHeader(QTextCursor cursore, const QString testo);
    void formattaTabella(QTextTable*, QVector<QString> colWidth);

    QMap<QString, QVariant>* configurazione;

};

#endif // REPORTFORM_H
