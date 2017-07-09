#ifndef MESSAGGIOWIDGET_H
#define MESSAGGIOWIDGET_H

#include <QWidget>

namespace Ui {
class MessaggioWidget;
}

class MessaggioWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MessaggioWidget(QWidget *parent = 0);
    ~MessaggioWidget();

private:
    Ui::MessaggioWidget *ui;
};

#endif // MESSAGGIOWIDGET_H
