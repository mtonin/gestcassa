#include "confermadlg.h"

ConfermaDlg::ConfermaDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
}

ConfermaDlg::ConfermaDlg(const QString &descrizione, const QString &descrizioneCampoInput, bool isPassword, QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    descrizioneLbl->setText(descrizione);
    if (descrizioneCampoInput.isEmpty()) {
        descrizioneInputLbl->setVisible(false);
        valoreInputTxt->setVisible(false);
    } else {
        descrizioneInputLbl->setText(descrizioneCampoInput);
        if (isPassword) valoreInputTxt->setEchoMode(QLineEdit::Password);
    }
}

int ConfermaDlg::visualizza()
{
    setWindowFlags(Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    //setWindowFlags(Qt::Tool);
    activateWindow();
    valoreInputTxt->selectAll();
    valoreInputTxt->setFocus();
    return exec();

}
