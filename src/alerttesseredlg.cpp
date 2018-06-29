#include <QtNetwork>
#include "alerttesseredlg.h"


AlertTessereDlg::AlertTessereDlg(QPoint pos, QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    activateWindow();

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

void AlertTessereDlg::on_codiceTesseraTxt_textEdited(const QString &arg1)
{
    if(arg1.size()>0) {
        queryBtn->setEnabled(true);
    } else {
        queryBtn->setEnabled(false);
    }
}

void AlertTessereDlg::on_queryBtn_clicked()
{
    qDebug("on_queryBtn_clicked");
    QNetworkRequest req;
    req.setHeader(QNetworkRequest::UserAgentHeader,"cassa");
}
