#include "dettaglireparto.h"

#include <QColorDialog>
#include <QtSql>

DettagliReparto::DettagliReparto(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
}

void DettagliReparto::setCurrentReparto(RepartoBtnWidget *currentRepartoBtn)
{

    repartoBtn = currentRepartoBtn;
    testoReparto->setText(repartoBtn->getNomeReparto());
    QColor colore = repartoBtn->getColore();
    sfondoBtn->SetButtonColorNormal(colore);
    colore = repartoBtn->getColoreText();
    carattereBtn->SetButtonColorNormal(colore);
    fontBtn->setFont(repartoBtn->getFont());
    disattivaFlag->setChecked(!repartoBtn->getAbilitato());

    testoReparto->selectAll();
    testoReparto->setFocus();

    connect(disattivaFlag, SIGNAL(stateChanged(int)), this, SLOT(on_disattivaFlag_stateChanged(int)));
}

void DettagliReparto::aggiornaReparto()
{
    QSqlQuery query("update or insert into reparti (idreparto,descrizione,font,coloresfondo,colorecarattere,abilitato) values(?,?,?,?,?,?)");
    query.addBindValue(repartoBtn->getId());
    query.addBindValue(testoReparto->text());
    query.addBindValue(repartoBtn->getFont());
    query.addBindValue(repartoBtn->getColore().toRgb());
    query.addBindValue(repartoBtn->getColoreText().toRgb());
    query.addBindValue(repartoBtn->getAbilitato());
    query.exec();
    if (!query.isActive()) {
        QMessageBox::critical(0, QObject::tr("Database Error"),
                              query.lastError().text());
        return;
    }
}

void DettagliReparto::on_sfondoBtn_clicked()
{
    QColorDialog dlg;
    QColor colore = repartoBtn->getColore();
    dlg.setCurrentColor(colore);
    if (dlg.exec()) {
        colore = dlg.currentColor();
        repartoBtn->setColore(colore);
        sfondoBtn->SetButtonColorNormal(colore);
    }
    aggiornaReparto();

}

void DettagliReparto::on_carattereBtn_clicked()
{
    QColorDialog dlg;
    QColor colore = repartoBtn->getColore();
    dlg.setCurrentColor(colore);
    if (dlg.exec()) {
        colore = dlg.currentColor();
        repartoBtn->setColoreText(colore);
        carattereBtn->SetButtonColorNormal(colore);
    }
    aggiornaReparto();

}

void DettagliReparto::on_fontBtn_clicked()
{
    QFontDialog dlg;
    dlg.setCurrentFont(repartoBtn->getFont());
    if (dlg.exec()) {
        QFont currentFont = dlg.currentFont();
        fontBtn->setFont(currentFont);
        repartoBtn->setButtonFont(currentFont);
    }
    aggiornaReparto();
}

void DettagliReparto::on_testoReparto_textEdited(const QString &arg1)
{
    repartoBtn->setNomeReparto(arg1);
    aggiornaReparto();
}

void DettagliReparto::on_disattivaFlag_stateChanged(int checked)
{
    repartoBtn->setAbilitato(!checked);
    aggiornaReparto();
}

