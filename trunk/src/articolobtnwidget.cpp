#include "articolobtnwidget.h"
#include <QtSql>
#include <QPainter>

void ArticoloBtnWidget::setNomeArticolo(const QString nome){
  nomeArticolo=nome;
  setText(nomeArticolo);
}

void ArticoloBtnWidget::setPrezzo(float nuovoPrezzo)
{
  prezzo=nuovoPrezzo;
}

void ArticoloBtnWidget::setRepartoStampa(const QString nome)
{
  repartoStampa=nome;
}

void ArticoloBtnWidget::setButtonFont(const QFont &font)
{
  setFont(font);
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
  int Align = Qt::AlignLeft|Qt::TextWordWrap;

  QString prezzoLabel=QString("%1 %2 ").arg(QChar(0x20AC)).arg(this->prezzo,5,'f',2);
  // Paint
  QRect fontRect=painter.boundingRect(PainterRect,Align,prezzoLabel);
  //QRect FontRect(Left, Top, Width, Height);
  painter.setBackground(QBrush(Qt::white));
  painter.setBackgroundMode(Qt::OpaqueMode);
  QRect bottomFontRect(Left+Width-fontRect.width()-1,Top+Height-fontRect.height()-1,fontRect.width(),fontRect.width());
  painter.drawText(bottomFontRect, Align, prezzoLabel,&bottomFontRect);
  painter.drawRect(bottomFontRect);
}

void ArticoloBtnWidget::paintEvent(QPaintEvent *evt)
{
  QPictureButton::paintEvent(evt);
  if(!nomeArticolo.isEmpty() && visualizzaPrezzo) {
    PaintPrezzo();
  }
}

ArticoloBtnWidget::ArticoloBtnWidget(int id,int numRiga, int numColonna,QWidget *parent) :
  idReparto(id),
  riga(numRiga),
  colonna(numColonna),
  visualizzaPrezzo(false),
  QPictureButton(parent)
{

  QSqlQuery stmt;
  stmt.prepare("select a.idreparto,a.riga,a.colonna,a.idarticolo,a.abilitato,b.descrizione,b.prezzo,b.destinazione,b.gestionemenu from pulsanti a,articoli b where a.idarticolo=b.idarticolo and a.idreparto=? and a.riga=? and a.colonna=?");
  stmt.addBindValue(idReparto);
  stmt.addBindValue(riga);
  stmt.addBindValue(colonna);
  if(!stmt.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                          stmt.lastError().text());
    return;
  }
  int numColIdArticolo=stmt.record().indexOf("idarticolo");
  int numColDescr=stmt.record().indexOf("descrizione");
  int numColprezzo=stmt.record().indexOf("prezzo");
  int numColAbilitato=stmt.record().indexOf("abilitato");
  int numColDestStampa=stmt.record().indexOf("destinazione");
  int numColGestioneMenu=stmt.record().indexOf("gestioneMenu");
  if(stmt.next()) {
    idArticolo=stmt.value(numColIdArticolo).toInt();
    nomeArticolo=stmt.value(numColDescr).toString();
    prezzo=stmt.value(numColprezzo).toFloat();
    abilitato=stmt.value(numColAbilitato).toBool();
    repartoStampa=stmt.value(numColDestStampa).toString();
    gestioneMenu=stmt.value(numColGestioneMenu).toBool();
  } else {
    //idArticolo=riga*6+colonna+1;
    idArticolo=0;
    //nomeArticolo=QString("ARTICOLO %1").arg(riga*6+colonna+1);
    nomeArticolo="";
    prezzo=0;
    abilitato=true;
    repartoStampa="";
    gestioneMenu=false;
  }
  setText(nomeArticolo);

  QSizePolicy buttonSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  buttonSizePolicy.setHorizontalStretch(0);
  buttonSizePolicy.setVerticalStretch(0);
  buttonSizePolicy.setHeightForWidth(sizePolicy().hasHeightForWidth());
  setSizePolicy(buttonSizePolicy);

}
