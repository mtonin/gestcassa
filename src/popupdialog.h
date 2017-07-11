#ifndef POPUPDIALOG_H
#define POPUPDIALOG_H

#include <QDialog>

class PopupDialog : public QDialog
{
    Q_OBJECT

public:
    PopupDialog(bool isPopup, QWidget *parent = 0);
    void avvia(QPoint startPoint);

    void visualizza(QPoint pos);
protected:
    void paintEvent(QPaintEvent *);

signals:
    void popupOk();
};

#endif // POPUPDIALOG_H
