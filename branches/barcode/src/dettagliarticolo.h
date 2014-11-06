#ifndef DETTAGLIARTICOLO_H
#define DETTAGLIARTICOLO_H

#include "ui_dettagliarticolo.h"
#include "articolobtnwidget.h"

#include <QStandardItemModel>

class DettagliArticolo : public QWidget, private Ui::DettagliArticolo
{
    Q_OBJECT

signals:

public:
    explicit DettagliArticolo(QWidget *parent = 0);
    ~DettagliArticolo();
    void setCurrentArticolo(const ArticoloBtnWidget*);

private slots:
    void on_testoArticolo_textEdited(const QString &arg1);

    void on_prezzoArticolo_textEdited(const QString &arg1);

    void on_disattivaFlag_stateChanged(int checked);

    void on_destinazioneBox_activated(const QString &arg1);

    void on_menuBox_clicked(bool checked);

    void on_nuovoBtn_clicked();

    void on_eliminaBtn_clicked();

    void on_articoliList_clicked(const QModelIndex &index);
    void rimuoveArticolo();

    void on_prezzoArticolo_editingFinished();

    void on_codiceBtn_clicked();

private:
    void aggiornaArticolo();
    ArticoloBtnWidget* articoloBtn;
    QStandardItemModel* modello;
    QStandardItemModel* articoliMenuModello;

    void reset();
    void creaSelezioneArticoloBox();
};

#endif // DETTAGLIARTICOLO_H
