#include "aboutdlg.h"
#include "svnrev.h"

const QString ABOUTMSG("<center><b><i>GESTCASSA</i></b><br>versione $VERSIONE<br>"
"<p>Programma per la gestione delle postazioni di cassa durante una sagra.<br><br>"
"Per informazioni e licenza d'uso, inviare una mail a <i>gestcassa@yahoo.com</i></p>");

AboutDlg::AboutDlg(QWidget *parent) :
QDialog(parent)
{
  setWindowFlags(Qt::Tool);
  activateWindow();
  setupUi(this);

  QString msg(ABOUTMSG);
  msg.replace(QString("$VERSIONE"),QString(VERSIONE),Qt::CaseInsensitive);
  aboutTxt->setHtml(msg);
}
