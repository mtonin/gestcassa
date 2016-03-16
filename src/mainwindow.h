#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "commons.h"
#include "dettaglireparto.h"
#include "dettagliarticolo.h"
#include "ordine.h"
#include "infowidget.h"
#include "simplecrypt.h"

#include <QMainWindow>
#include <QList>
#include <QMap>
#include <QTimer>
#include <QCache>
#include <QSplashScreen>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void aggiungeArticolo(const int idArticolo, const QString& nomeArticolo, float prezzo);
    void avanzaStato(const QString msg);

public:
    explicit MainWindow(QMap<QString, QVariant>* configurazione, QSplashScreen& splashScreen,QWidget *parent = 0);
    ~MainWindow();

private slots:
    void creaRepartiButtons();
    void repartoSelezionato();
    void articoloSelezionato();
    void on_configurazioneBtn_clicked();
    void on_funzioniBtn_clicked();
    void on_reportBtn_clicked();
    void on_statsBtn_clicked();
    void on_stornoBtn_clicked();
    void execCassa();
    void execGestione();
    void visualizzaPrezzo(bool visualizza);
    void scambia(int id1, int id2);
    void lampeggia();
    void esegueOperazione(int idx);

    void execTest();
    void execBuoni();
    void execAbout();

    void on_closeBtn_clicked();

    void ricaricaArchivio();

private:
    Ui::MainWindow *ui;
    Ordine* ordineBox;
    DettagliReparto* dettagliRepartoBox;
    DettagliArticolo* dettagliArticoloBox;
    infoWidget* info;
    SimpleCrypt* cifratore;
    QTimer* blinkTimer;
    QString colore;
    QCache<int, QMap<QString, QVariant>> articoliCache;
    QSplashScreen& splash;
    bool isHiddenCursor;
    bool richiestaChiusura;

    //QList<ArticoloBtnWidget*> articoliBtnList;
    QList<RepartoBtnWidget*> repartiList;
    QList<QStackedWidget*> articoliList;
    modalitaType modalitaCorrente;
    QMap<QString, QVariant>* confMap;

    void keyPressEvent(QKeyEvent *);
    void closeEvent(QCloseEvent *);

    void gestioneModalita(const modalitaType nuovaModalita);
    void creaArticoliPerRepartoButtons(int, RepartoBtnWidget*);
    void creaInfoMessaggi();
    void enterTest();
    void exitTest();
    void caricaArticoli();

    bool aggiornaConfigurazioneDaDB(const QString nomePar);
    bool isPasswordOK(const QString pwd);
};

#endif // MAINWINDOW_H
