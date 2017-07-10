#ifndef POPUPDIALOG_H
#define POPUPDIALOG_H

#include <QDialog>

class PopupDialog : public QDialog
{
    Q_OBJECT

public:
    PopupDialog(QWidget *parent = 0);
    void avvia(bool isPopup, QPoint startPoint);

protected:
    void paintEvent(QPaintEvent *);

signals:
    void popupOk();
};

#endif // POPUPDIALOG_H
