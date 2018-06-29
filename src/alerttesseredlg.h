#ifndef ALERTTESSEREDLG_H
#define ALERTTESSEREDLG_H

#include "popupdialog.h"

#include "ui_alerttesseredlg.h"

class AlertTessereDlg : public QDialog, private Ui::AlertTessereDlg
{
    Q_OBJECT

public:
    explicit AlertTessereDlg(QPoint pos, QWidget *parent = 0);

private slots:
    void init();
    void on_chiudeBtn_clicked();
    void on_codiceTesseraTxt_textEdited(const QString &arg1);
    void on_queryBtn_clicked();
};

#endif // ALERTTESSEREDLG_H
