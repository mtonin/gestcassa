#include "alerttesseredlg.h"

AlertTessereDlg::AlertTessereDlg(QPoint startPoint, QWidget *parent) :
    PopupDialog(parent)
{
    setupUi(this);
    avvia(false,startPoint);
    connect(this,SIGNAL(popupOk()),this,SLOT(init()));
}

void AlertTessereDlg::init()
{

}

void AlertTessereDlg::on_chiudeBtn_clicked()
{
    close();
}
