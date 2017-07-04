#include "controlliordine.h"
#include "ui_controlliordine.h"
#include <QPropertyAnimation>

ControlliOrdine::ControlliOrdine(QWidget *parent) :
    idArticolo(0),
    QWidget(parent),
    ui(new Ui::ControlliOrdine)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window|Qt::Popup);

    effetto = new QPropertyAnimation(this, "windowOpacity",this);
    effetto->setStartValue("1");
    effetto->setEndValue("0");
    effetto->setDuration(1000);
    effetto->setEasingCurve(QEasingCurve::Linear);
    connect(effetto, SIGNAL(finished()), this, SIGNAL(effettoTerminato()));

    timer=new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(3000);
    connect(timer,SIGNAL(timeout()),effetto,SLOT(start()));

}

ControlliOrdine::~ControlliOrdine(){
    delete ui;
}

void ControlliOrdine::show(){
    QWidget::show();
    avviaTimer();
}

void ControlliOrdine::on_toolButton_clicked(){
    emit incrementa(idArticolo);
    avviaTimer();
}

void ControlliOrdine::on_toolButton_2_clicked(){
    emit decrementa(idArticolo, index);
    avviaTimer();
}

void ControlliOrdine::avviaTimer() {
    timer->stop();
    effetto->stop();
    setWindowOpacity(1);
    timer->start();
}
