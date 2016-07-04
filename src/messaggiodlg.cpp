#include "messaggiodlg.h"

MessaggioDlg::MessaggioDlg(const QString &titolo, const QString &descrizione, QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    setWindowTitle(titolo);
    descrizioneLbl->setText(descrizione);
}

int MessaggioDlg::visualizza()
{
    setWindowFlags(Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    //setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    //setWindowFlags(Qt::Tool);
    activateWindow();
    return exec();

}
