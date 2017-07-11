#include "alerttesseredlg.h"

AlertTessereDlg::AlertTessereDlg(QPoint pos, QWidget *parent) :
    PopupDialog(false,parent)
{
    setupUi(this);
    //avvia(startPoint);
    //connect(this,SIGNAL(popupOk()),this,SLOT(init()));
    visualizza(pos);
}

void AlertTessereDlg::init()
{

}

void AlertTessereDlg::on_chiudeBtn_clicked()
{
    close();
}
