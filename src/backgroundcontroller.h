#ifndef BACKGROUNDCONTROLLER_H
#define BACKGROUNDCONTROLLER_H

#include "commons.h"

#include <QDateTime>
#include <QThread>
#include <QtSql>
class BackgroundController : public QThread
{
    Q_OBJECT

public:
    BackgroundController();
    void run();
    void setUltimaLetturaDB(const QDateTime par) {ultimaLetturaDB=par;}
    enum statoDatabase {
        Reset=0,
        Disconnesso,
        Connesso,
        DaAggiornare
    };
    Q_ENUM(statoDatabase)

signals:
    void messaggioDB(int codice, QString testo);

private slots:
    void controllaDatabase();

private:
    QDateTime ultimaLetturaDB;
    enum statoDatabase stato;
};

#endif // BACKGROUNDCONTROLLER_H
