#include <QJsonDocument>
#include "commons.h"
#include "alerttesseredlg.h"
#include "messaggiodlg.h"

AlertTessereDlg::AlertTessereDlg(QString address, int porta, int timeout, int tessereOrdine, QWidget *parent) :
    serverName(address),serverPort(porta),serverTimeout(timeout*1000),tessereDaAttivare(tessereOrdine),QDialog(parent)
{
    setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    activateWindow();

    if(tessereDaAttivare>0) {
        tessereOrdineFrame->setVisible(true);
    } else {
        tessereOrdineFrame->setVisible(false);
    }
    tessereDaAttivareTxt->setText(QString("%1").arg(tessereDaAttivare));
    tessereAttivate=0;

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
    LOG_INFO() << "Richiesta stato tessera:" << qPrintable(codiceTesseraRicercaTxt->text());
    QString url=QString("http://%1:%2/msf.php?cmd=balance&id=%3").arg(serverName).arg(serverPort).arg(codiceTesseraRicercaTxt->text());
    QByteArray ba=esegueRichiestaHttp(url);
    if(!ba.isEmpty()) {
        QJsonDocument jsonDoc=QJsonDocument::fromJson(ba);
        QJsonObject jsonObj=jsonDoc.object();
        if("OK"==jsonObj.value("code").toString()) {
            codiceTesseraTxt->setText(jsonObj.value("card_id").toString());
            tipoTesseraTxt->setText(jsonObj.value("type").toString());
            creditoTesseraTxt->setText(jsonObj.value("current_balance").toString());
            if("0"==jsonObj.value("active").toString()) {
                statoTesseraTxt->setText("NON ATTIVATA");
                QPalette palette = statoTesseraTxt->palette();
                palette.setColor(QPalette::Base, Qt::red);
                statoTesseraTxt->setPalette(palette);
                attivatBtn->setEnabled(true);
                disattivaBtn->setEnabled(false);
            } else {
                statoTesseraTxt->setText("ATTIVATA");
                QPalette palette = statoTesseraTxt->palette();
                palette.setColor(QPalette::Base, Qt::green);
                statoTesseraTxt->setPalette(palette);
                attivatBtn->setEnabled(false);
                disattivaBtn->setEnabled(true);
            }
        } else {
            QString msg=QString("Codice tessera %1 non valido").arg(codiceTesseraRicercaTxt->text());
            LOG_ERROR() << qPrintable(msg);
            qDebug(msg.toStdString().c_str());
            MessaggioDlg msgBox("ATTENZIONE", msg,this);
            msgBox.visualizza();
        }
    }
    codiceTesseraRicercaTxt->setText("");
    codiceTesseraRicercaTxt->setFocus();
}

QByteArray AlertTessereDlg::esegueRichiestaHttp(QUrl url ){
    QApplication::setOverrideCursor(Qt::BusyCursor);
    QNetworkRequest req;
    QNetworkReply* reply;
    QEventLoop connection_loop;
    connect(netManager, SIGNAL( finished( QNetworkReply* ) ), &connection_loop, SLOT( quit() ) );
    req.setUrl( url );
    req.setHeader(QNetworkRequest::UserAgentHeader,"CASSA");
    reply = netManager->get( req );
    ReplyTimeout::set(reply,serverTimeout);
    connection_loop.exec();
    QApplication::restoreOverrideCursor();
    if(reply->error()!=QNetworkReply::NoError) {
        QString msg=QString("ERRORE: %1").arg(reply->errorString());
        LOG_ERROR() << qPrintable(msg);
        MessaggioDlg msgBox("ATTENZIONE", msg,this);
        msgBox.visualizza();
        return "";
    }
    reply->deleteLater();
    return reply->readAll();
}

void AlertTessereDlg::on_attivatBtn_clicked(){
    LOG_INFO() << "Richiesta attivazione tessera:" << qPrintable(codiceTesseraTxt->text());
    QString url=QString("http://%1:%2/msf.php?cmd=activate&id=%3").arg(serverName).arg(serverPort).arg(codiceTesseraTxt->text());
    QByteArray ba=esegueRichiestaHttp(url);
    if(!ba.isEmpty()) {
        QJsonDocument jsonDoc=QJsonDocument::fromJson(ba);
        QJsonObject jsonObj=jsonDoc.object();
        if("OK"==jsonObj.value("code").toString()) {
            statoTesseraTxt->setText("ATTIVATA");
            QPalette palette = statoTesseraTxt->palette();
            palette.setColor(QPalette::Base, Qt::green);
            statoTesseraTxt->setPalette(palette);
            attivatBtn->setEnabled(false);
            disattivaBtn->setEnabled(true);
            if(tessereAttivate<tessereDaAttivare) {
                ++tessereAttivate;
                tessereDaAttivareTxt->setText(QString("%1").arg(tessereDaAttivare-tessereAttivate));
            }
        } else {
            QString msg=QString("Errore %1").arg(jsonObj.value("error").toString());
            LOG_ERROR() << qPrintable(msg);
            MessaggioDlg msgBox("ATTENZIONE", msg,this);
            msgBox.visualizza();
        }
    }
}

void AlertTessereDlg::on_disattivaBtn_clicked(){
    LOG_INFO() << "Richiesta disattivazione tessera:" << qPrintable(codiceTesseraTxt->text());
    QString url=QString("http://%1:%2/msf.php?cmd=deactivate&id=%3").arg(serverName).arg(serverPort).arg(codiceTesseraTxt->text());
    QByteArray ba=esegueRichiestaHttp(url);
    if(!ba.isEmpty()) {
        QJsonDocument jsonDoc=QJsonDocument::fromJson(ba);
        QJsonObject jsonObj=jsonDoc.object();
        if("OK"==jsonObj.value("code").toString()) {
            statoTesseraTxt->setText("NON ATTIVATA");
            QPalette palette = statoTesseraTxt->palette();
            palette.setColor(QPalette::Base, Qt::red);
            statoTesseraTxt->setPalette(palette);
            attivatBtn->setEnabled(true);
            disattivaBtn->setEnabled(false);
            if(tessereAttivate>0) {
                --tessereAttivate;
                tessereDaAttivareTxt->setText(QString("%1").arg(tessereDaAttivare-tessereAttivate));
            }
        } else {
            QString msg=QString("Errore %1").arg(jsonObj.value("error").toString());
            LOG_ERROR() << qPrintable(msg);
            MessaggioDlg msgBox("ATTENZIONE", msg,this);
            msgBox.visualizza();
        }
    }
}
