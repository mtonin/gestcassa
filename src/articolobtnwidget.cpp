#include "commons.h"
#include "articolobtnwidget.h"
#include <QtSql>
#include <QPainter>

ArticoloBtnWidget::ArticoloBtnWidget(int id, int idRep, int numRiga, int numColonna, QMap<QString, QVariant>* articoloMap, QWidget *parent) :
    idPulsante(id),
    idReparto(idRep),
    riga(numRiga),
    colonna(numColonna),
    visualizzaPrezzo(false),
    QPictureButton(parent)
{
    if (!articoloMap->isEmpty()) {
        idArticolo = articoloMap->value("idarticolo").toInt();
        nomeArticolo = articoloMap->value("nome").toString();
        prezzo = articoloMap->value("prezzo").toFloat();
        abilitato = articoloMap->value("abilitato").toBool();
        repartoStampa = articoloMap->value("repartoStampa").toString();
        gestioneMenu = articoloMap->value("gestioneMenu").toBool();
    } else {
        //idArticolo=riga*6+colonna+1;
        idArticolo = 0;
        //nomeArticolo=QString("ARTICOLO %1").arg(riga*6+colonna+1);
        nomeArticolo = "";
        prezzo = 0;
        abilitato = true;
        repartoStampa = "";
        gestioneMenu = false;
    }
    setText(nomeArticolo);
    QSizePolicy buttonSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    buttonSizePolicy.setHorizontalStretch(0);
    buttonSizePolicy.setVerticalStretch(0);
    buttonSizePolicy.setHeightForWidth(sizePolicy().hasHeightForWidth());
    setSizePolicy(buttonSizePolicy);
    setDragAbilitato(true);
}

void ArticoloBtnWidget::setNomeArticolo(const QString nome)
{
    nomeArticolo = nome;
    setText(nomeArticolo);
}

void ArticoloBtnWidget::setPrezzo(float nuovoPrezzo)
{
    prezzo = nuovoPrezzo;
    update();
}

void ArticoloBtnWidget::setRepartoStampa(const QString nome)
{
    repartoStampa = nome;
}

void ArticoloBtnWidget::setPos(int r, int c)
{
    //   aggiorna il database dopo lo spostamento dei pulsanti
    riga = r;
    colonna = c;
    QSqlQuery stmt;
    if (0 == idArticolo) {
        // posizione scambiata con un pulsante vuoto
        if(!stmt.prepare("delete from pulsanti where idreparto=? and riga=? and colonna=?")) {
          QSqlError errore=stmt.lastError();
          QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
          QMessageBox::critical(this,"Errore",msg);
          return;
        }
        stmt.addBindValue(idReparto);
        stmt.addBindValue(r);
        stmt.addBindValue(c);
    } else {
        if(!stmt.prepare("update or insert into pulsanti (idreparto,riga,colonna,idarticolo,abilitato) values(?,?,?,?,?)")) {
          QSqlError errore=stmt.lastError();
          QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
          QMessageBox::critical(this,"Errore",msg);
          return;
        }
        stmt.addBindValue(idReparto);
        stmt.addBindValue(r);
        stmt.addBindValue(c);
        stmt.addBindValue(idArticolo);
        stmt.addBindValue(abilitato);
    }
    if (!stmt.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"),
                              stmt.lastError().text());
        return;
    }
}

void ArticoloBtnWidget::setDragAbilitato(bool valore)
{
    setAcceptDrops(valore);
    dragAbilitato = valore;
}

void ArticoloBtnWidget::setButtonFont(const QFont &font)
{
    setFont(font);
}

void ArticoloBtnWidget::setColore(const QColor &colore)
{
    SetButtonColorNormal(colore);
    SetButtonColorHot(colore);
}

void ArticoloBtnWidget::setColoreText(const QColor &colore)
{
    SetTextColorEnabled(colore);
}

void ArticoloBtnWidget::PaintPrezzo()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect PainterRect = painter.window();

    // Text position
    int Top    = PainterRect.top();
    int Left   = PainterRect.left();
    int Height = PainterRect.height();
    int Width  = PainterRect.width();
    int Align = Qt::AlignLeft | Qt::TextWordWrap;

    QString prezzoLabel = QString("%1 %L2 ").arg(QChar(0x20AC)).arg(this->prezzo, 5, 'f', 2);
    // Paint
    QFont font = painter.font();
    //font.setPointSize(font.pointSize() - 2);
    font.setPointSize(10);
    painter.setFont(font);
    QRect fontRect = painter.boundingRect(PainterRect, Align, prezzoLabel);
    //QRect FontRect(Left, Top, Width, Height);
    painter.setBackground(QBrush(Qt::white));
    painter.setBackgroundMode(Qt::OpaqueMode);
    QRect bottomFontRect(Left + Width - fontRect.width() - 1, Top + Height - fontRect.height() - 1, fontRect.width(), fontRect.width());
    painter.drawText(bottomFontRect, Align, prezzoLabel, &bottomFontRect);
    painter.drawRect(bottomFontRect);
}

void ArticoloBtnWidget::PaintDivieto(int posX)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect PainterRect = painter.window();

    QPixmap divieto(":/GestCassa/divieto");
    painter.drawPixmap(posX, height() - 20, 20, 20, divieto);
}

void ArticoloBtnWidget::PaintAttenzione(int posX)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect PainterRect = painter.window();

    QPixmap warning(":/GestCassa/warning");
    painter.drawPixmap(posX, height() - 20, 20, 20, warning);
}

void ArticoloBtnWidget::paintEvent(QPaintEvent *evt)
{
    QPictureButton::paintEvent(evt);
    if (idArticolo > 0 && visualizzaPrezzo) {
        PaintPrezzo();
    }

    int posSimboli=0;
    if (idArticolo > 0 && !abilitato) {
        PaintDivieto(posSimboli);
        posSimboli+=20;
    }
    if (idArticolo > 0 && nomeArticolo.isEmpty()) {
        PaintAttenzione(posSimboli);
    }
}

void ArticoloBtnWidget::mousePressEvent(QMouseEvent *e)
{
    // impementazione per drag & drop
    if (Qt::LeftButton == e->button()) {
        dragStartPos = e->pos();
    }
    QPictureButton::mousePressEvent(e);
}

void ArticoloBtnWidget::mouseMoveEvent(QMouseEvent *e)
{
    QString msg = QString("mouseMoveEvent %1").arg(getIdPulsante());
    qDebug(msg.toUtf8());
    // impementazione per drag & drop
    if (!(e->buttons()&Qt::LeftButton)) {
        return;
    }
    if (!dragAbilitato)
        return;
    if ((dragStartPos - e->pos()).manhattanLength() < QApplication::startDragDistance()) {
        return;
    }

    QDrag* dragOp = new QDrag(this);
    QMimeData* mimeData = new QMimeData;
    mimeData->setText(QString("%1").arg(getIdPulsante()));
    dragOp->setMimeData(mimeData);
    dragOp->setPixmap(QPixmap(":/GestCassa/drag"));
    dragOp->exec();
}

void ArticoloBtnWidget::dragEnterEvent(QDragEnterEvent *e)
{
    QString msg = QString("dragEnterEvent %1").arg(getIdPulsante());
    qDebug(msg.toUtf8());
    if (e->mimeData()->text().toInt() == getIdPulsante()) {
        e->ignore();
        return;
    }
    e->acceptProposedAction();
}

void ArticoloBtnWidget::dragLeaveEvent(QDragLeaveEvent *e)
{
    QString msg = QString("dragLeaveEvent %1").arg(getIdPulsante());
    qDebug(msg.toUtf8());
    e->ignore();
}

void ArticoloBtnWidget::dropEvent(QDropEvent *e)
{
    int idSrc = e->mimeData()->text().toInt();
    int idDst = getIdPulsante();
    emit swapSignal(idSrc, idDst);
}

