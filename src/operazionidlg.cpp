#include "operazionidlg.h"
#include "ui_operazionidlg.h"
#include <QDesktopWidget>
#include <QSignalMapper>
#include <QPropertyAnimation>
#include <QPainterPath>
#include <QPainterPathStroker>
#include <QPainter>

OperazioniDlg::OperazioniDlg(modalitaType modalitaCorrente, QPoint startPoint, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OperazioniDlg)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    activateWindow();

    switch (modalitaCorrente) {
    case CASSA:
        ui->cassaBtn->setDisabled(true);
        break;
    case TEST:
        ui->testBtn->setDisabled(true);
        //ui->gestioneBtn->setDisabled(true);
        //ui->stornoBtn->setDisabled(true);
        break;
    case GESTIONE:
        ui->gestioneBtn->setDisabled(true);
        //ui->testBtn->setDisabled(true);
        break;
    }

    QPoint pos = startPoint;
    QPropertyAnimation *effetto = new QPropertyAnimation(this, "geometry");
    effetto->setStartValue(QRect(pos.x(), pos.y(), minimumSize().width(), minimumSize().height()));
    effetto->setEndValue(QRect(pos.x(), pos.y() - maximumSize().height(), maximumSize().width(), maximumSize().height()));
    //effetto->setStartValue(QRect(pos.x(),pos.y(),geometry().width(),0));
    //effetto->setEndValue(QRect(pos.x(),pos.y()-geometry().height(),geometry().width(),geometry().height()));
    effetto->setDuration(1000);
    effetto->setEasingCurve(QEasingCurve::OutBack);
    connect(effetto, SIGNAL(finished()), this, SLOT(init()));

    effetto->start(QAbstractAnimation::DeleteWhenStopped);
    /*
    pos.setY(pos.y()-frameGeometry().height());
    QDesktopWidget* desktop=QApplication::desktop();
    int screenHeight=desktop->screenGeometry().height();
    if(pos.y()+height()>screenHeight) {
      pos.setY(screenHeight-height());
    }
    move(pos);
    */

}

OperazioniDlg::~OperazioniDlg()
{
    delete ui;
}

void OperazioniDlg::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(QBrush(Qt::white), 10));
    painter.setBrush(QBrush(Qt::black));
    painter.setOpacity(.7);

    QRect box = this->geometry();
    QPainterPath winRect;
    winRect.addRect(0, 0, box.width(), box.height());
    //winRect.addRoundRect(0,0,box.width(),box.height(),15,15);
    painter.drawPath(winRect);

}

void OperazioniDlg::pulsanteClicked(int idx)
{
    close();
    emit operazioneSelezionata(idx);
}

void OperazioniDlg::init()
{

    //setMaximumSize(size());
    //setMinimumSize(size());

    QSignalMapper* mapper = new QSignalMapper(this);
    connect(ui->gestioneBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(ui->gestioneBtn, 1);
    connect(ui->cassaBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(ui->cassaBtn, 2);
    connect(ui->testBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(ui->testBtn, 3);
    connect(ui->statsBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(ui->statsBtn, 4);
    connect(ui->stornoBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(ui->stornoBtn, 5);

    connect(mapper, SIGNAL(mapped(int)), this, SLOT(pulsanteClicked(int)));

}

void OperazioniDlg::on_statsBtn_2_clicked()
{
    QRect box = geometry();
    QPropertyAnimation *effetto = new QPropertyAnimation(this, "geometry");
    effetto->setStartValue(this->geometry());
    effetto->setEndValue(QRect(box.x(), box.y() + box.height(), minimumSize().width(), minimumSize().height()));
    effetto->setDuration(1000);
    effetto->setEasingCurve(QEasingCurve::InBack);
    connect(effetto, SIGNAL(finished()), this, SLOT(close()));

    effetto->start(QAbstractAnimation::DeleteWhenStopped);
}
