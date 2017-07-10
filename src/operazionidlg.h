#ifndef OPERAZIONIDLG_H
#define OPERAZIONIDLG_H

#include "popupdialog.h"

#include "commons.h"

namespace Ui
{
class OperazioniDlg;
}

class OperazioniDlg : public PopupDialog
{
    Q_OBJECT

public:
    explicit OperazioniDlg(modalitaType modalitaCorrente, QPoint startPoint, QWidget *parent = 0);
    ~OperazioniDlg();

signals:
    void operazioneSelezionata(int);

private slots:
    void pulsanteClicked(int idx);
    void init();

private:
    Ui::OperazioniDlg *ui;
};

#endif // OPERAZIONIDLG_H
