#ifndef MESSAGGIODLG_H
#define MESSAGGIODLG_H

#include "ui_messaggiodlg.h"

class MessaggioDlg : public QDialog, private Ui::MessaggioDlg
{
    Q_OBJECT

public:
    explicit MessaggioDlg(const QString &titolo, const QString &descrizione, QWidget *parent);
    int visualizza();
};

#endif // MESSAGGIODLG_H
