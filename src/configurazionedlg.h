#ifndef CONFIGURAZIONEDLG_H
#define CONFIGURAZIONEDLG_H

#include "simplecrypt.h"
#include "ui_configurazionedlg.h"
#include <QMap>
#include <QPrinter>

class ConfigurazioneDlg : public QDialog, private Ui::ConfigurazioneDlg
{
    Q_OBJECT

signals:
    void resetOrdini(const int);
    void resetArticoli();
    void configurazioneCambiata();
    void cambiaVisualizzaPrezzo(bool);

public:
    explicit ConfigurazioneDlg(QMap<QString, QVariant>* par, QWidget *parent = 0);
    ~ConfigurazioneDlg();
    QMap<QString, QVariant> getConfigurazione() {
        return *configurazioneAttuale;
    }

private slots:
    void on_printerSelectBtn_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_attivaRestoCheck_toggled(bool checked);

    void on_stampanteChk_toggled(bool checked);

    void on_pdfChk_toggled(bool checked);

    void on_cartellaPdfBtn_clicked();

    void on_cartellaPdfTxt_editingFinished();

    void on_nomeCassaTxt_editingFinished();

    void on_durataRestoTxt_editingFinished();

    void on_descrManifestazioneTxt_editingFinished();

    void on_intestazioneScontrinoTxt_textChanged();

    void on_fondoScontrinoTxt_textChanged();

    void on_visualizzaPrezzoBox_clicked(bool checked);

    void on_cancellaOrdiniBtn_clicked();

    void on_stampanteBox_activated(const QString &arg1);

    void on_exportOrdiniBtn_clicked();

    void on_exportArticoliBtn_clicked();

    void on_importArticoliBtn_clicked();

    void on_resetDbBtn_clicked();

    void on_logoIntestazioneBtn_clicked();

    void on_logoCheckBox_clicked(bool checked);

    void on_logoFondoBtn_clicked();

    void on_logoFondoCheckBox_clicked(bool checked);

    void on_intestazioneCheckBox_clicked(bool checked);

    void on_fondoCheckBox_clicked(bool checked);

    void on_resetBuoniBtn_clicked();

    void on_cancellaLogoBtn_clicked();

    void on_cancellaLogoFondoBtn_clicked();

    void on_nascondeCursoreBox_clicked(bool checked);

    void on_adminPasswordTxt_editingFinished();

    void on_serieRitiroTxt_currentIndexChanged(int index);

private:
    void esportaInFile(const QString&);
    QVariant valutaStringa(const QString&);

    QMap<QString, QVariant>* configurazioneAttuale;
    QMap<QString, QVariant>* nuovaConfigurazione;
    SimpleCrypt* cifratore;
    QString pwdInChiaro;

    QPrinter printer;
    bool isChanged;

    void keyPressEvent(QKeyEvent *);
    void execParametriAvanzati();
    void setCaratteriRimanenti();

    void selezionaLogo(const QString nomePar);
    bool aggiornaConfigurazioneDaDB(const QString nomePar);
};

#endif // CONFIGURAZIONEDLG_H
