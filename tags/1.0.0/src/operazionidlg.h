#ifndef OPERAZIONIDLG_H
#define OPERAZIONIDLG_H

#include <QDialog>

#include "commons.h"

namespace Ui
{
class OperazioniDlg;
}

class OperazioniDlg : public QDialog
{
    Q_OBJECT

public:
    explicit OperazioniDlg(modalitaType modalitaCorrente, QPoint startPoint, QWidget *parent = 0);
    ~OperazioniDlg();

signals:
    void operazioneSelezionata(int);

protected:
    void paintEvent(QPaintEvent *);

private slots:
    void pulsanteClicked(int idx);
    void init();

    void on_statsBtn_2_clicked();

private:
    Ui::OperazioniDlg *ui;
};

#endif // OPERAZIONIDLG_H
