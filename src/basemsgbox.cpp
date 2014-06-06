#include "basemsgbox.h"
#include <QPainter>

BaseMsgBox::BaseMsgBox(QWidget *parent) :
    QWidget(parent)
{
    resize(512, 512);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);

    m_mainLayout.setSpacing(0);
    m_mainLayout.setMargin(0);
    setLayout(&m_mainLayout);

    m_mainLayout.addWidget(&m_titolo, 0, 0, 1, 1);
    m_mainLayout.setRowStretch(1, 1);

    m_textLayout.setMargin(10);
    m_textLayout.setSpacing(0);
    m_mainLayout.addLayout(&m_textLayout, 1, 0, 1, 1);

    m_textLayout.addWidget(&m_testo);
    m_testo.setAlignment(Qt::AlignCenter);
    QFont font = m_testo.font();
    font.setBold(true);
    font.setPointSize(20);
    m_testo.setFont(font);
    m_testo.setStyleSheet("QLabel { color : white; }");
    m_testo.setWordWrap(true);

    m_cache = NULL;

    connect(&m_titolo, SIGNAL(close()), this, SLOT(close()));
}

BaseMsgBox::~BaseMsgBox()
{
    delete m_cache;
}

void BaseMsgBox::setTesto(QString testo)
{
    m_testo.setText(testo);
}

void BaseMsgBox::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (m_cache !=  NULL)   {
        QPainter painter(this);
        painter.drawPixmap(0 , 0 , *m_cache);
    }

}

void BaseMsgBox::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    delete m_cache;

    m_cache =  new  QPixmap(size());

    m_cache -> fill(Qt::transparent);

    QPainter painter(m_cache);

    QColor darkblue(23 ,   23 ,   34);
    QColor lightblue(177 , 177 , 203);

    QPolygon background;

    background <<  QPoint(0 ,             16)
               <<  QPoint(16 ,              0)
               <<  QPoint(width() -   1 ,              0)
               <<  QPoint(width() -   1 , height() -  33)
               <<  QPoint(width() -  17 , height() -  17)
               <<  QPoint(272 , height() -  17)
               <<  QPoint(256 , height() -   1)
               <<  QPoint(16 , height() -   1)
               <<  QPoint(16 ,            272)
               <<  QPoint(0 ,            256);

    painter.setPen(QPen(darkblue));
    painter.setBrush(QBrush(darkblue));

    painter.drawPolygon(background);
    QPolygon frame;

    frame <<  QPoint(4 ,             20)
          <<  QPoint(20 ,              4)
          <<  QPoint(width() -   4 ,              4)
          <<  QPoint(width() -   4 , height() -  37)
          <<  QPoint(width() -  20 , height() -  21)
          <<  QPoint(268 , height() -  21)
          <<  QPoint(252 , height() -   5)
          <<  QPoint(20 , height() -   5)
          <<  QPoint(20 ,            268)
          <<  QPoint(4 ,            252);

    painter.setPen(QPen(lightblue));
    painter.setBrush(Qt :: NoBrush);

    painter.drawPolygon(frame);

}


TitoloBar::TitoloBar(QWidget *parent): QWidget(parent), m_closeBtn(this)
{
    setFixedHeight(35);
    //setFixedSize(100,35);
    m_cache = NULL;
    m_closeBtn.setText("X");
    connect(&m_closeBtn, SIGNAL(clicked()), this, SLOT(termina()));
}

TitoloBar::~TitoloBar()
{
    delete m_cache;
}

void TitoloBar::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    delete m_cache;

    m_cache =  new  QPixmap(size());
    qDebug(QString("size=%1x%2").arg(m_cache->size().width()).arg(m_cache->size().height()).toAscii());

    m_cache -> fill(Qt :: transparent);

    QPainter painter(m_cache);

    QColor lightBlue(177 , 177 , 203 , 255);
    QColor gradientStart(0 ,    0 ,    0 ,    0);
    QColor gradientEnd(0 ,    0 ,    0 , 220);

    QLinearGradient linearGrad(QPointF(0 , 0), QPointF(0 , height()));
    linearGrad.setColorAt(0 , gradientStart);
    linearGrad.setColorAt(1 , gradientEnd);

    QPolygon frame;

    frame <<  QPoint(20 ,   4)
          <<  QPoint(width() -  4 ,   4)
          <<  QPoint(width() -  4 , 32)
          <<  QPoint(4 , 32)
          <<  QPoint(4 , 20);

    painter.setPen(QPen(lightBlue));
    painter.setBrush(QBrush(linearGrad));

    painter.drawPolygon(frame);

    QPolygon buttons;

    buttons <<  QPoint(width() -  80 ,   4)
            <<  QPoint(width() -   4 ,   4)
            <<  QPoint(width() -   4 , 32)
            <<  QPoint(width() -  88 , 32)
            <<  QPoint(width() -  88 , 12);

    painter.setPen(QPen(lightBlue));
    painter.setBrush(QBrush(lightBlue));

    painter.drawPolygon(buttons);

    m_closeBtn.move(width() - 32, 2);
    m_closeBtn.resize(30, 30);
}

void TitoloBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (m_cache !=  NULL)   {
        QPainter painter(this);
        painter.drawPixmap(0 , 0 , *m_cache);
    }

}

void TitoloBar::termina()
{
    QWidget::close();
    emit close();
}
