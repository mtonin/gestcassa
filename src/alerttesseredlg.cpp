#include <QJsonDocument>
#include "alerttesseredlg.h"
#include "messaggiodlg.h"

AlertTessereDlg::AlertTessereDlg(QString address, QPoint pos, QWidget *parent) :
    endpoint(address),QDialog(parent)
{
    setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    activateWindow();

    netManager=new QNetworkAccessManager(this);
    //netManager->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy,"10.30.102.202",8080));
    //avvia(startPoint);
    //connect(this,SIGNAL(popupOk()),this,SLOT(init()));
    //visualizza(pos);
}

void AlertTessereDlg::init()
{

}

void AlertTessereDlg::on_chiudeBtn_clicked()
{
    close();
}

void AlertTessereDlg::on_codiceTesseraRicercaTxt_textEdited(const QString &arg1)
{
    if(arg1.size()>0) {
        queryBtn->setEnabled(true);
    } else {
        queryBtn->setEnabled(false);
    }
}

void AlertTessereDlg::on_queryBtn_clicked()
{
    QString url=QString("http://%1/msf.php?cmd=balance&id=%2").arg(endpoint).arg(codiceTesseraRicercaTxt->text());
    QByteArray ba=esegueRichiestaHttp(url);
    if(!ba.isEmpty()) {
        QJsonDocument jsonDoc=QJsonDocument::fromJson(ba);
        QJsonObject jsonObj=jsonDoc.object();
        if(jsonObj.value("code").toString()=="OK") {
            codiceTesseraTxt->setText(jsonObj.value("card_id").toString());
            tipoTesseraTxt->setText(jsonObj.value("type").toString());
            creditoTesseraTxt->setText(jsonObj.value("current_balance").toString());
            if(0==jsonObj.value("active").toInt()) {
                statoTesseraTxt->setText("NON ATTIVATA");
                attivatBtn->setEnabled(true);
                disattivaBtn->setEnabled(false);
            } else {
                statoTesseraTxt->setText("ATTIVATA");
                attivatBtn->setEnabled(false);
                disattivaBtn->setEnabled(true);
            }
        }
    }
    codiceTesseraRicercaTxt->setText("");
    codiceTesseraRicercaTxt->setFocus();
}

QByteArray AlertTessereDlg::esegueRichiestaHttp(QUrl url ){
    QNetworkRequest req;
    QNetworkReply* reply;
    QEventLoop connection_loop;
    connect(netManager, SIGNAL( finished( QNetworkReply* ) ), &connection_loop, SLOT( quit() ) );
    req.setUrl( url );
    req.setHeader(QNetworkRequest::UserAgentHeader,"CASSA");
    reply = netManager->get( req );
    ReplyTimeout::set(reply,5000);
    connection_loop.exec();
    if(reply->error()!=QNetworkReply::NoError) {
        QString msg=QString("ERRORE: %1").arg(reply->errorString());
        qDebug(msg.toStdString().c_str());
        MessaggioDlg msgBox("ATTENZIONE", msg,this);
        msgBox.visualizza();
        return "";
    }
    reply->deleteLater();
    return reply->readAll();
}
