#include "infowidget.h"

#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QGraphicsOpacityEffect>

infoWidget::infoWidget(const QStringList lista, QWidget *parent) : listaTesto(lista), QWidget(parent)
{
    setupUi(this);

    currentIndex = 0;
    QGraphicsEffect* ge = new QGraphicsOpacityEffect();
    infoLbl->setGraphicsEffect(ge);

    effetto = new QSequentialAnimationGroup(this);

    QPropertyAnimation* anim1 = new QPropertyAnimation(ge, "opacity");
    anim1->setDuration(2000);
    anim1->setStartValue("0");
    anim1->setEndValue("1");
    effetto->addAnimation(anim1);
    QPropertyAnimation* anim2 = new QPropertyAnimation(ge, "opacity");
    anim2->setDuration(2000);
    anim2->setStartValue("1");
    anim2->setEndValue("0");
    effetto->addAnimation(anim2);

    infoLbl->setText(listaTesto.at(currentIndex));

    connect(effetto, SIGNAL(finished()), this, SLOT(ricominciaEffetto()));
    effetto->start();

}

void infoWidget::ricominciaEffetto()
{
    currentIndex++;
    if (currentIndex >= listaTesto.size()) {
        currentIndex = 0;
    }

    infoLbl->setText(listaTesto.at(currentIndex));
    effetto->start();

}

void infoWidget::setListaTesto(const QStringList lista)
{
    effetto->stop();
    listaTesto = lista;
    currentIndex = -1;
    ricominciaEffetto();
}
