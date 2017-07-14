#include "svnrev.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "repartobtnwidget.h"
#include "articolobtnwidget.h"
#include "dettaglireparto.h"
#include "configurazionedlg.h"
#include "reportform.h"
#include "infowidget.h"
#include "QDigitalClock.h"
#include "statsform.h"
#include "confermadlg.h"
#include "operazionidlg.h"
#include "storicoordini.h"
#include "buonidlg.h"
#include "aboutdlg.h"
//#include "basemsgbox.h"
#include "dbparamdlg.h"
#include "backgroundcontroller.h"
#include "messaggiodlg.h"

#include <QtWidgets>
#include <QHBoxLayout>
#include <QtSql>
#include <QTime>

MainWindow::MainWindow(QMap<QString, QVariant>* configurazione, QSplashScreen &splashScreen, QWidget *parent) : confMap(configurazione), splash(splashScreen),QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(this,SIGNAL(avanzaStato(QString)),&splash,SLOT(showMessage(QString)));

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setWindowState(Qt::WindowFullScreen);
    //showMaximized();
    isHiddenCursor=false;

    setAcceptDrops(true);

    cifratore = new SimpleCrypt(Q_UINT64_C(0x529c2c1779964f9d));

    dettagliRepartoBox = new DettagliReparto;
    dettagliArticoloBox = new DettagliArticolo;
    ordineBox = new Ordine(confMap);

    ui->latoStackedWidget->addWidget(new QFrame);
    ui->latoStackedWidget->addWidget(dettagliRepartoBox);
    ui->latoStackedWidget->addWidget(dettagliArticoloBox);
    ui->latoStackedWidget->addWidget(ordineBox);

    connect(this, SIGNAL(aggiungeArticolo(int, QString, float)), ordineBox, SLOT(nuovoArticolo(int, QString, float)));
    connect(dettagliArticoloBox,SIGNAL(articoloAggiornato()),this,SLOT(impostaUltimoAggiornamentoDB()));
    connect(dettagliRepartoBox,SIGNAL(repartoAggiornato()),this,SLOT(impostaUltimoAggiornamentoDB()));

    QDigitalClock* orologio = new QDigitalClock;
    orologio->SetFormat("dd/MM/yyyy\nHH:mm:ss");
    QFont font = orologio->font();
    font.setBold(true);
    font.setPointSize(10);
    orologio->setFont(font);
    //orologio->SetTextColor(Qt::red);
    orologio->SetAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QHBoxLayout* clockLayout = new QHBoxLayout;
    clockLayout->setContentsMargins(QMargins(0, 0, 0, 0));
    clockLayout->addWidget(orologio);
    ui->clockFrame->setLayout(clockLayout);

    QStringList messaggi = QString("").split(",");
    info = new infoWidget(messaggi);
    creaInfoMessaggi();

    QHBoxLayout* infoLayout = new QHBoxLayout;
    infoLayout->setContentsMargins(QMargins(1, 1, 1, 1));
    infoLayout->addWidget(info);
    ui->infoFrame->setLayout(infoLayout);

    backgroundThread=new QThread;
    dbCheck=new DatabaseController;
    dbCheck->moveToThread(backgroundThread);
    connect(backgroundThread,SIGNAL(started()),dbCheck,SLOT(esegueControllo()));
    connect(dbCheck,SIGNAL(messaggioDB(int,QString)),this,SLOT(setStato(int,QString)));
    backgroundThread->start();

    creaRepartiButtons();
    gestioneModalita(CASSA);

    blinkTimer = new QTimer(this);
    connect(blinkTimer, SIGNAL(timeout()), this, SLOT(lampeggia()));

    richiestaChiusura=false;
    externalProcess=NULL;

}

MainWindow::~MainWindow()
{
    delete ui;
    delete cifratore;
    if(externalProcess)
        delete externalProcess;
    dbCheck->setFlagTermine(true);
    backgroundThread->quit();
    backgroundThread->wait(5000);
    delete backgroundThread;
    delete dbCheck;
}

void MainWindow::gestioneModalita(const modalitaType nuovaModalita)
{
    int idSessione = 0;

    if (GESTIONE == nuovaModalita) {

        if(GESTIONE != modalitaCorrente) {
            ConfermaDlg dlg("Inserire la password per accedere alla modalità amministrativa.", "Password", true);
            while (true) {
                if (QDialog::Accepted != dlg.visualizza()) return;
                if(isPasswordOK(dlg.getValore())) {
                    break;
                }
            }
        }

        if (TEST == modalitaCorrente) {
            idSessione = confMap->value("SESSIONESALVATA").toInt();
            confMap->insert("SESSIONECORRENTE", idSessione);
            confMap->remove("SESSIONESALVATA");
            ordineBox->nuovoOrdine(idSessione);
            exitTest();
        }
        ui->adminFunctBox->setVisible(true);

        ui->latoStackedWidget->setCurrentIndex(0);
        ui->articoliStackedWidget->setVisible(true);

        // attiva tutti i pulsanti dei reparti
        QListIterator<RepartoBtnWidget*> itReparti(repartiList);
        while (itReparti.hasNext()) {
            itReparti.next()->setVisible(true);
        }
        QListIterator<QStackedWidget*> it(articoliList);
        while (it.hasNext()) {
            QStackedWidget* box = it.next();
            box->setCurrentIndex(0);
            ArticoloBtnWidget* btnWidget = (ArticoloBtnWidget*)box->widget(0);
            //abilita drag & drop
            btnWidget->setDragAbilitato(true);
        }

        ui->articoliStackedWidget->setAcceptDrops(true);
        ui->messaggiArea->setText("MODALITA' GESTIONE");
        if(isHiddenCursor) {
          QGuiApplication::restoreOverrideCursor();
          isHiddenCursor=false;
        }

    } else {
        // nuova modalità == CASSA o TEST
        ui->adminFunctBox->setVisible(false);
        ui->latoStackedWidget->setCurrentWidget(ordineBox);

        int primoRepartoAttivo = -1;

        // nasconde i pulsanti dei reparti disabilitati
        QListIterator<RepartoBtnWidget*> itReparti(repartiList);
        while (itReparti.hasNext()) {
            RepartoBtnWidget* reparto = itReparti.next();
            reparto->setVisible(reparto->getAbilitato());
            if (primoRepartoAttivo < 0 && reparto->getAbilitato()) primoRepartoAttivo = reparto->getId();
        }

        // nasconde i pulsanti degli articoli disabilitati
        QListIterator<QStackedWidget*> it(articoliList);
        while (it.hasNext()) {
            QStackedWidget* box = it.next();
            ArticoloBtnWidget* btnWidget = (ArticoloBtnWidget*)box->widget(0);
            //disabilita drag & drop
            btnWidget->setDragAbilitato(false);
            if (!btnWidget->getAbilitato() || btnWidget->getNomeArticolo().isEmpty()) {
                box->setCurrentIndex(1);
            }
        }

        ui->articoliStackedWidget->setAcceptDrops(false);
        if (-1 == primoRepartoAttivo) {
            ui->articoliStackedWidget->setVisible(false);
        } else {
            ui->articoliStackedWidget->setVisible(true);
            ui->articoliStackedWidget->setCurrentIndex(primoRepartoAttivo);
        }

        if (CASSA == nuovaModalita) {
            if (TEST == modalitaCorrente) {
                idSessione = confMap->value("SESSIONESALVATA").toInt();
                confMap->insert("SESSIONECORRENTE", idSessione);
                confMap->remove("SESSIONESALVATA");
                ordineBox->nuovoOrdine(idSessione);
                exitTest();
            }

            ui->messaggiArea->setText("MODALITA' CASSA");
        }
        if (TEST == nuovaModalita) {
            ui->adminFunctBox->setVisible(false);
            idSessione = confMap->value("SESSIONECORRENTE").toInt();
            confMap->insert("SESSIONECORRENTE", ID_SESSIONE_TEST);
            confMap->insert("SESSIONESALVATA", idSessione);
            ordineBox->nuovoOrdine(ID_SESSIONE_TEST);
            enterTest();
        }
        if(confMap->value("NASCONDECURSORE").toBool()) {
            if(!isHiddenCursor)
                QGuiApplication::setOverrideCursor(Qt::BlankCursor);
            isHiddenCursor=true;
        }

    }

    modalitaCorrente = nuovaModalita;
    setFocus();
}

void MainWindow::keyPressEvent(QKeyEvent *evt)
{
    switch (evt->key()) {
      case Qt::Key_F11: {
        if(evt->modifiers() & Qt::ControlModifier) {
          /*
          if (isMaximized()) {
            //setWindowFlags(Qt::Window);
            showNormal();
          } else {
            //setWindowFlags(Qt::Window|Qt::FramelessWindowHint);
            showMaximized();
          }
          */
          if(isHiddenCursor) {
            QGuiApplication::restoreOverrideCursor();
          } else {
            QGuiApplication::setOverrideCursor(Qt::BlankCursor);
          }
          isHiddenCursor=!isHiddenCursor;
          return;
        }
      }
      case Qt::Key_X: {
          if(evt->modifiers() & Qt::ControlModifier) {
            qDebug("CTRL+X pressed");
            on_closeBtn_clicked();
            return;
          }
      }
      case Qt::Key_F10: {
          if(evt->modifiers() & Qt::ControlModifier) {
              qDebug("CTRL+F10 pressed (mainwindow)");
              ordineBox->impostaCarattere();
              return;
          }
      }
    }
    QWidget::keyPressEvent(evt);

}

void MainWindow::closeEvent(QCloseEvent *evt)
{
    if(richiestaChiusura) {
      ConfermaDlg dlg("Confermi l'uscita?");
      if (QDialog::Accepted != dlg.visualizza()) evt->ignore();
    } else {
      evt->ignore();
    }
}

void MainWindow::creaRepartiButtons()
{

    // ricarica la cache
    caricaArticoli();

    // resetta l'articolo corrente del form dettagliArticolo
    dettagliArticoloBox->setCurrentArticolo(NULL);

    // cancella i pulsanti degli articoli
    QListIterator<QStackedWidget*> itArticoli(articoliList);
    while (itArticoli.hasNext()) {
        QStackedWidget* articolo = itArticoli.next();
        delete articolo;
    }
    articoliList.clear();

    // cancella i pulsanti dei reparti
    QListIterator<RepartoBtnWidget*> itReparti(repartiList);
    while (itReparti.hasNext()) {
        RepartoBtnWidget* reparto = itReparti.next();
        delete reparto;
    }
    repartiList.clear();

    QLayout* hboxLayout = ui->repartiBox->layout();
    if (!hboxLayout) {
        hboxLayout = new QHBoxLayout(ui->repartiBox);
        hboxLayout->setSpacing(2);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        hboxLayout->setContentsMargins(-1, 5, -1, 5);
    }

    while (ui->articoliStackedWidget->count() > 0) {
        QWidget* w = ui->articoliStackedWidget->widget(0);
        ui->articoliStackedWidget->removeWidget(w);
    }

    for (int i = 0; i < NUM_REPARTI; i++) {

        RepartoBtnWidget* reparto01Btn = new RepartoBtnWidget(i, ui->repartiBox);
        QString msg=QString("Caricamento reparto %1...").arg(reparto01Btn->getNomeReparto());
        emit(avanzaStato(msg));

        repartiList.append(reparto01Btn);
        hboxLayout->addWidget(reparto01Btn);
        creaArticoliPerRepartoButtons(i, reparto01Btn);

        connect(reparto01Btn, SIGNAL(clicked()), this, SLOT(repartoSelezionato()));
    }

    ui->repartiBox->setLayout(hboxLayout);
    //ui->latoStackedWidget->setCurrentIndex(0);

}

void MainWindow::creaArticoliPerRepartoButtons(int numReparto, RepartoBtnWidget* repartoBtn)
{

    QColor coloreSfondo = repartoBtn->buttonColorNormal();
    QColor coloreCarattere = repartoBtn->textColorEnabled();
    QFont currentFont = repartoBtn->getFont();
    bool adattaFont=repartoBtn->getAdattaFont();
    QGridLayout* griglia = new QGridLayout;
    griglia->setSpacing(2);

    QFrame* pagina = new QFrame;
    ui->articoliStackedWidget->addWidget(pagina);
    pagina->setLayout(griglia);

    bool visualizzaPrezzo = confMap->value("VISUALIZZAZIONEPREZZO").toBool();
    for (int riga = 0; riga < NUM_RIGHE_ART; riga++) {
        for (int col = 0; col < NUM_COLONNE_ART; col++) {

            QStackedWidget* stackedBox = new QStackedWidget;
            int idPulsante = numReparto * NUM_RIGHE_ART * NUM_COLONNE_ART + riga * NUM_COLONNE_ART + col;

            QMap<QString, QVariant> articoloMap = articoliCache[idPulsante];
            ArticoloBtnWidget* btn = new ArticoloBtnWidget(idPulsante,repartoBtn->getId(),riga,col, &articoloMap);

            btn->SetButtonColorNormal(coloreSfondo);
            btn->SetButtonColorHot(coloreSfondo);
            btn->SetTextColorEnabled(coloreCarattere);
            btn->setFont(currentFont);
            btn->setVisualizzaPrezzo(visualizzaPrezzo);
            btn->SetAdjustFont(adattaFont);
            connect(btn, SIGNAL(swapSignal(int, int)), this, SLOT(scambia(int, int)));
            stackedBox->addWidget(btn);
            QFrame* blankFrame = new QFrame;
            stackedBox->addWidget(blankFrame);
            articoliList.append(stackedBox);

            griglia->addWidget(stackedBox, riga, col);
            connect(btn, SIGNAL(clicked()), this, SLOT(articoloSelezionato()));
            connect(repartoBtn, SIGNAL(cambiaColore(QColor)), btn, SLOT(setColore(QColor)));
            connect(repartoBtn, SIGNAL(cambiaFont(QFont)), btn, SLOT(setButtonFont(QFont)));
            connect(repartoBtn, SIGNAL(cambiaColoreText(QColor)), btn, SLOT(setColoreText(QColor)));
            connect(repartoBtn,SIGNAL(adattaFont(bool)),btn,SLOT(SetAdjustFont(bool)));

            //articoliBtnList.append(btn);
        }
    }
}

void MainWindow::creaInfoMessaggi()
{
    QStringList messaggi = QString("GESTIONE\nCASSA,versione\n%1").arg(VERSIONE).split(",");
    QString descrizione = confMap->value("DESCRMANIFESTAZIONE").toString();
    if (!descrizione.isEmpty()) {
        messaggi.insert(0, descrizione);
    }
    info->setListaTesto(messaggi);

    QString nomeCassa=QString("CASSA: %1").arg(confMap->value("NOMECASSA").toString());
    ui->nomaCassaText->setText(nomeCassa);

}

void MainWindow::repartoSelezionato()
{
    RepartoBtnWidget* btn = qobject_cast<RepartoBtnWidget*>(sender());
    //btn->setDown(true);
    ui->articoliStackedWidget->setCurrentIndex(btn->getId());
    if (GESTIONE == modalitaCorrente) {
        dettagliRepartoBox->setCurrentReparto(btn);
        //dettagliRepartoBox->disconnect();
        ui->latoStackedWidget->setCurrentWidget(dettagliRepartoBox);
    }
}

void MainWindow::articoloSelezionato()
{
    ArticoloBtnWidget* btn = qobject_cast<ArticoloBtnWidget*>(sender());
    //btn->setDown(true);
    if (GESTIONE == modalitaCorrente) {
        dettagliArticoloBox->setCurrentArticolo(btn);
        ui->latoStackedWidget->setCurrentWidget(dettagliArticoloBox);
    } else {
        emit aggiungeArticolo(btn->getId(), btn->getNomeArticolo().simplified(), btn->getPrezzo());
    }
}

void MainWindow::on_configurazioneBtn_clicked()
{
    ConfigurazioneDlg* dlg = new ConfigurazioneDlg(confMap);
    connect(dlg, SIGNAL(resetOrdini(int)), ordineBox, SLOT(nuovoOrdine(int)));
    connect(dlg, SIGNAL(resetArticoli()), this, SLOT(creaRepartiButtons()));
    connect(dlg, SIGNAL(cambiaVisualizzaPrezzo(bool)), this, SLOT(visualizzaPrezzo(bool)));
    connect(dlg, SIGNAL(configurazioneCambiata()),this,SLOT(impostaUltimoAggiornamentoDB()));
    connect(dlg, SIGNAL(cambiaStatoSconto(bool)),ordineBox,SLOT(setStatoSconto(bool)));
    dlg->exec();

    creaInfoMessaggi();
    delete dlg;
}

void MainWindow::on_funzioniBtn_clicked()
{
    QPoint btnPoint = ui->funzioniBtn->mapToGlobal(ui->funzioniBtn->pos());
    OperazioniDlg* dlg=new OperazioniDlg(modalitaCorrente, btnPoint,this);
    connect(dlg, SIGNAL(operazioneSelezionata(int)), this, SLOT(esegueOperazione(int)));
    dlg->show();
    // close();
}

void MainWindow::on_reportBtn_clicked()
{
    QString descrizione = confMap->value("DESCRMANIFESTAZIONE").toString();
    ReportForm form(descrizione);
    form.exec();
}

void MainWindow::on_statsBtn_clicked()
{
  int idSessione = confMap->value("SESSIONECORRENTE").toInt();
  if (ID_SESSIONE_TEST == idSessione) {
      idSessione = confMap->value("SESSIONESALVATA").toInt();
  }
  QString nomeCassa=confMap->value("NOMECASSA").toString();
  QString descrizione = confMap->value("DESCRMANIFESTAZIONE").toString();
  StatsForm form(idSessione, nomeCassa,descrizione, this);
  //form.setWindowState(Qt::WindowMaximized);
  form.exec();

}

void MainWindow::execCassa()
{
    gestioneModalita(CASSA);
}

void MainWindow::execGestione()
{
    if (ordineBox->isInComposizione()) {
        MessaggioDlg msgBox("ATTENZIONE", "Completare o annullare l'ordine corrente prima di cambiare modalità operativa",this);
        msgBox.visualizza();
        return;
    }
    gestioneModalita(GESTIONE);
}

bool MainWindow::isPasswordOK(const QString pwd) {

    QTime adesso=QTime::currentTime();
    QString emergencyPwd=QString("paperinik%1%2").arg(adesso.minute(),2,10,QChar('0')).arg(adesso.hour(),2,10,QChar('0'));
    if(0==pwd.compare(emergencyPwd)) {
      return true;
    }
    if(!aggiornaConfigurazioneDaDB("ADMINPASSWORD"))
      return false;
    QString adminPassword = confMap->value("ADMINPASSWORD").toString();
    if (adminPassword.isEmpty()) {
        adminPassword = "12345";
    } else {
        adminPassword = cifratore->decryptToString(adminPassword);
    }
    if(0==pwd.compare(adminPassword)) {
      return true;
    } else {
        MessaggioDlg msgBox("Accesso", "Password errata",this);
        msgBox.visualizza();
        return false;
    }
}

void MainWindow::visualizzaPrezzo(bool visualizza)
{
    QListIterator<QStackedWidget*> it(articoliList);
    while (it.hasNext()) {
        QStackedWidget* box = it.next();
        ArticoloBtnWidget* btnWidget = (ArticoloBtnWidget*)box->widget(0);
        btnWidget->setVisualizzaPrezzo(visualizza);
    }
}

void MainWindow::scambia(int id1, int id2)
{
    QString msg = QString("Scambia %1 e %2").arg(id1).arg(id2);
    qDebug(msg.toLatin1());

    QStackedWidget* box1 = articoliList.value(id1);
    ArticoloBtnWidget* btnWidget1 = (ArticoloBtnWidget*)box1->widget(0);
    int w1riga = btnWidget1->getRiga();
    int w1colonna = btnWidget1->getColonna();
    QStackedWidget* box2 = articoliList.value(id2);
    ArticoloBtnWidget* btnWidget2 = (ArticoloBtnWidget*)box2->widget(0);
    int w2riga = btnWidget2->getRiga();
    int w2colonna = btnWidget2->getColonna();

    QGridLayout* griglia = (QGridLayout*)ui->articoliStackedWidget->currentWidget()->layout();
    griglia->addWidget(box1, w2riga, w2colonna);
    griglia->addWidget(box2, w1riga, w1colonna);
    btnWidget1->setPos(w2riga, w2colonna);
    btnWidget2->setPos(w1riga, w1colonna);

    dettagliArticoloBox->setCurrentArticolo(btnWidget1);
    ui->latoStackedWidget->setCurrentWidget(dettagliArticoloBox);

}

void MainWindow::execTest()
{
    if (ordineBox->isInComposizione()) {
        MessaggioDlg msgBox("ATTENZIONE", "Completare o annullare l'ordine corrente prima di cambiare modalità operativa",this);
        msgBox.visualizza();
        return;
    }
    gestioneModalita(TEST);
}

void MainWindow::lampeggia()
{
    QString coloreCarattere;
    if (colore == "red") {
        colore = "white";
        coloreCarattere = "black";
    } else {
        colore = "red";
        coloreCarattere = "white";
    }
    QString stylesheet = QString("background-color: %1; color: %2;").arg(colore).arg(coloreCarattere);
    ui->messaggiArea->setStyleSheet(stylesheet);
}

void MainWindow::esegueOperazione(int idx)
{
    switch (idx) {
    case 1:
        execGestione();
        break;
    case 2:
        execCassa();
        break;
    case 3:
        execTest();
        break;
    case 4:
        ricaricaArchivio();
        break;
    case 5:
        break;
    case 6:
        execBuoni();
        break;
    case 7:
          execAbout();
          break;
    }

}

void MainWindow::enterTest()
{
    ui->messaggiArea->setText("MODALITA' TEST");
    lampeggia();
    blinkTimer->start(1000);
}

void MainWindow::exitTest()
{
    blinkTimer->stop();
    ui->messaggiArea->setText("");
    ui->messaggiArea->setStyleSheet("background-color: white; color: black;");
}

void MainWindow::on_stornoBtn_clicked()
{
    int idSessione = confMap->value("SESSIONECORRENTE").toInt();
    if (ID_SESSIONE_TEST == idSessione) {
        idSessione = confMap->value("SESSIONESALVATA").toInt();
    }
    StoricoOrdini dlg(idSessione);
    dlg.exec();
    return;
}

void MainWindow::execBuoni()
{
    if(nullptr==externalProcess) {
        externalProcess=new SingleProcess(this);
    }

    QString comando=confMap->value("EXTERNALPROGRAM").toString();
    if(!comando.isEmpty())
    externalProcess->esegue(comando);

    /*
    BuoniDlg dlg(confMap,this);
    dlg.exec();
    */
    return;
}

void MainWindow::execAbout()
{
  AboutDlg dlg(this);
  dlg.exec();
  return;
}

void MainWindow::caricaArticoli()
{

    QString sql;
    QSqlQuery stmt;
    sql="select valore from configurazione where chiave='ultimoaggiornamento'";
    if (!stmt.exec(sql)) {
        MessaggioDlg msgBox("Database Error",stmt.lastError().text(),this);
        msgBox.visualizza();
        return;
    }
    if (stmt.next()) {
        dbCheck->setUltimaLetturaDB(stmt.value(0).toDateTime());
    }

    articoliCache.clear();

    sql="select a.idreparto,a.riga,a.colonna,a.idarticolo,a.abilitato,b.descrizione,b.prezzo,b.destinazione,b.gestionemenu from pulsanti a,articoli b where a.idarticolo=b.idarticolo";
    if (!stmt.exec(sql)) {
        MessaggioDlg msgBox("Database Error",stmt.lastError().text(),this);
        msgBox.visualizza();
        return;
    }
    int numColReparto = stmt.record().indexOf("idreparto");
    int numColColonna = stmt.record().indexOf("colonna");
    int numColRiga = stmt.record().indexOf("riga");
    int numColIdArticolo = stmt.record().indexOf("idarticolo");
    int numColDescr = stmt.record().indexOf("descrizione");
    int numColprezzo = stmt.record().indexOf("prezzo");
    int numColAbilitato = stmt.record().indexOf("abilitato");
    int numColDestStampa = stmt.record().indexOf("destinazione");
    int numColGestioneMenu = stmt.record().indexOf("gestioneMenu");
    while (stmt.next()) {
        QMap<QString, QVariant> articoloMap;
        int numReparto = stmt.value(numColReparto).toInt();
        int riga = stmt.value(numColRiga).toInt();
        int colonna = stmt.value(numColColonna).toInt();
        int idPulsante = numReparto * NUM_RIGHE_ART * NUM_COLONNE_ART + riga * NUM_COLONNE_ART + colonna;
        articoloMap.insert("idarticolo", stmt.value(numColIdArticolo));
        articoloMap.insert("nome", stmt.value(numColDescr));
        articoloMap.insert("prezzo", stmt.value(numColprezzo));
        articoloMap.insert("abilitato", stmt.value(numColAbilitato));
        articoloMap.insert("repartoStampa", stmt.value(numColDestStampa));
        articoloMap.insert("gestioneMenu", stmt.value(numColGestioneMenu));
        articoloMap.insert("riga", stmt.value(numColRiga));
        articoloMap.insert("colonna", stmt.value(numColColonna));
        articoloMap.insert("reparto", stmt.value(numColReparto));
        articoliCache[idPulsante]=articoloMap;
    }

}

void MainWindow::on_closeBtn_clicked()
{
    richiestaChiusura=true;
    richiestaChiusura=close();
}

void MainWindow::ricaricaArchivio()
{
  if (ordineBox->isInComposizione()) {
      MessaggioDlg msgBox( "ATTENZIONE", "Completare o annullare l'ordine corrente prima di ricaricare l'archivio",this);
      msgBox.visualizza();
      return;
  }

  //qApp->setOverrideCursor(Qt::WaitCursor);

  const QString chiaviConfRemote="DESCRMANIFESTAZIONE,PRINTINTESTAZIONE,INTESTAZIONE,PRINTFONDO,FONDO,PRINTLOGO,LOGOPIXMAP,PRINTLOGOFONDO,LOGOFONDOPIXMAP,SESSIONECORRENTE";
  foreach (QString nomePar,chiaviConfRemote.split(',')) {
    if(!aggiornaConfigurazioneDaDB(nomePar)) {
        //qApp->restoreOverrideCursor();
        return;
    }
  }

  creaRepartiButtons();
  gestioneModalita(CASSA);

  creaInfoMessaggi();
  int idSessione = confMap->value("SESSIONECORRENTE").toInt();
  ordineBox->nuovoOrdine(idSessione);

  //qApp->restoreOverrideCursor();

  MessaggioDlg msgBox("AGGIORNAMENTO", "Archivio ricaricato correttamente.",this);
  msgBox.visualizza();
  return;

}

void MainWindow::impostaUltimoAggiornamentoDB(){

    QString sql;
    QSqlQuery stmt;
    sql="update or insert into configurazione (chiave,valore) values('ultimoaggiornamento',current_timestamp) returning valore";
    if (!stmt.exec(sql)) {
        MessaggioDlg msgBox("Database Error", stmt.lastError().text(),this);
        msgBox.visualizza();
        return;
    }
    if (stmt.next()) {
        QString ultimoAggiornamento=stmt.value(0).toDateTime().toString();
        //qDebug(ultimoAggiornamento.toStdString().c_str());
    }

}

bool MainWindow::aggiornaConfigurazioneDaDB(const QString nomePar) {

  QString sql;
  if(nomePar.contains("PIXMAP",Qt::CaseInsensitive)) {
    sql="select oggetto from risorse where id=?";
  } else {
    sql="select valore from configurazione where chiave=?";
  }
  QSqlQuery stmt;
  if (!stmt.prepare(sql)) {
    QSqlError err=stmt.lastError();
    MessaggioDlg msgBox("Database Error", stmt.lastError().text(),this);
    msgBox.visualizza();
    return false;
  }
  stmt.addBindValue(nomePar);
  if (!stmt.exec()) {
      MessaggioDlg msgBox("Database Error", stmt.lastError().text(),this);
      msgBox.visualizza();
      return false;
  }

  if(stmt.next()) {
    if(nomePar.contains("PIXMAP",Qt::CaseInsensitive)) {
      confMap->insert(nomePar, stmt.value(0).toByteArray());
    } else {
      confMap->insert(nomePar, stmt.value(0).toString());
    }
  }

  return true;
}

void MainWindow::setStato(int stato, QString testo) {

    switch (stato) {
    case DatabaseController::Disconnesso:
        ui->statoDBLbl->setPixmap(QPixmap(":/GestCassa/cancella"));
        break;
    case DatabaseController::DaAggiornare:
        ui->statoDBLbl->setPixmap(QPixmap(":/GestCassa/warning"));
        break;
    default:
        ui->statoDBLbl->setPixmap(QPixmap(":/GestCassa/db_ok"));
        break;
    }
    ui->statoDBLbl->setToolTip(testo);

}
