#ifndef ALERTTESSEREDLG_H
#define ALERTTESSEREDLG_H

#include "popupdialog.h"

#include "ui_alerttesseredlg.h"

class AlertTessereDlg : public PopupDialog, private Ui::AlertTessereDlg
{
    Q_OBJECT

public:
    explicit AlertTessereDlg(QPoint startPoint, QWidget *parent = 0);
private slots:
    void init();
    void on_chiudeBtn_clicked();
};

#endif // ALERTTESSEREDLG_H
