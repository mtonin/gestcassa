#include "repartobtnwidget.h"
#include <QtSql>
#include <QMessageBox>
#include <QAction>

RepartoBtnWidget::RepartoBtnWidget(QWidget *parent):
    QPictureButton(parent)
{
}

void RepartoBtnWidget::setNomeReparto(const QString nuovoReparto)
{
    nomeReparto = nuovoReparto;
    setText(nuovoReparto);
}

void RepartoBtnWidget::setColore(const QColor &colore)
{
    SetButtonColorNormal(colore);
    SetButtonColorHot(colore);
    emit cambiaColore(colore);
}

void RepartoBtnWidget::setColoreText(const QColor &colore)
{
    SetTextColorEnabled(colore);
    emit cambiaColoreText(colore);
}

void RepartoBtnWidget::PaintDivieto()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect PainterRect = painter.window();

    QPixmap divieto(":/GestCassa/divieto");
    painter.drawPixmap(0, height() - 20, 20, 20, divieto);
}

void RepartoBtnWidget::paintEvent(QPaintEvent *evt)
{
    QPictureButton::paintEvent(evt);
    if (!abilitato) {
        PaintDivieto();
    }

}

void RepartoBtnWidget::setButtonFont(const QFont &font)
{
    //currentFont=font;
    setFont(font);
    emit cambiaFont(font);
}

RepartoBtnWidget::RepartoBtnWidget(int id, QWidget *parent) :
    idReparto(id),
    QPictureButton(parent)
{
    QSqlQuery stmt;
    if(!stmt.prepare("select descrizione,font,coloreSfondo,coloreCarattere,abilitato from reparti where idreparto=?")) {
          QSqlError errore=stmt.lastError();
          QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
          QMessageBox::critical(this,"Errore",msg);
          return;
    }
    stmt.addBindValue(idReparto);
    if (!stmt.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"),
                              stmt.lastError().text());
        return;
    }
    int numColDescr = stmt.record().indexOf("descrizione");
    int numColFont = stmt.record().indexOf("font");
    int numColColoreSfondo = stmt.record().indexOf("coloresfondo");
    int numColColoreCarattere = stmt.record().indexOf("colorecarattere");
    int numColAbilitato = stmt.record().indexOf("abilitato");
    if (stmt.next()) {
        nomeReparto = stmt.value(numColDescr).toString();
        QFont currentFont;
        currentFont.fromString(stmt.value(numColFont).toString());
        setFont(currentFont);
        QString coloreSfondo = stmt.value(numColColoreSfondo).toString();
        SetButtonColorNormal(coloreSfondo);
        QString coloreCarattere = stmt.value(numColColoreCarattere).toString();
        SetTextColorEnabled(coloreCarattere);
        setAbilitato(stmt.value(numColAbilitato).toBool());
    } else {
        nomeReparto = QString("REPARTO %1").arg(idReparto);
        setAbilitato(false);
    }
    setText(nomeReparto);
    SetButtonColorHot(buttonColorNormal());
    QSizePolicy buttonSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    buttonSizePolicy.setHorizontalStretch(0);
    buttonSizePolicy.setVerticalStretch(0);
    //buttonSizePolicy.setHeightForWidth(sizePolicy().hasHeightForWidth());
    setSizePolicy(buttonSizePolicy);
    setMaximumWidth(200);

}
