#ifndef OPERAZIONIDLG_H
#define OPERAZIONIDLG_H

#include <QDialog>
#include "commons.h"

namespace Ui {
class OperazioniDlg;
}

class OperazioniDlg : public QDialog
{
    Q_OBJECT

public:
    explicit OperazioniDlg(modalitaType modalitaCorrente, QWidget *parent = 0);
    ~OperazioniDlg();

signals:
    void operazioneSelezionata(int);

private slots:
    void pulsanteClicked(int idx);

private:
    Ui::OperazioniDlg *ui;
};

#endif // OPERAZIONIDLG_H
