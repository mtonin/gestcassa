#include "popupdialog.h"
#include <QDesktopWidget>
#include <QSignalMapper>
#include <QPropertyAnimation>
#include <QPainterPath>
#include <QPainterPathStroker>
#include <QPainter>

PopupDialog::PopupDialog(bool isPopup, QWidget *parent) :
    QDialog(parent) {

    if(isPopup)
        setWindowFlags(Qt::Popup|Qt::FramelessWindowHint);
    else
        setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
}

void PopupDialog::avvia(QPoint startPoint) {
    activateWindow();

    QPoint pos = startPoint;
    QPropertyAnimation *effetto = new QPropertyAnimation(this, "geometry");
    effetto->setStartValue(QRect(pos.x(), pos.y(), minimumSize().width(), minimumSize().height()));
    effetto->setEndValue(QRect(pos.x(), pos.y() - maximumSize().height(), maximumSize().width(), maximumSize().height()));
    effetto->setDuration(300);
    connect(effetto, SIGNAL(finished()), this, SIGNAL(popupOk()));

    effetto->start(QAbstractAnimation::DeleteWhenStopped);
}

void PopupDialog::visualizza(QPoint pos) {
    activateWindow();
    move(pos);
    show();
}

void PopupDialog::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(QBrush(Qt::white), 5));
    painter.setBrush(QBrush(Qt::black));
    painter.setOpacity(.7);

    QRect box = this->geometry();
    QPainterPath winRect;
    winRect.addRect(0, 0, box.width(), box.height());
    //winRect.addRoundRect(0,0,box.width(),box.height(),15,15);
    painter.drawPath(winRect);
}
