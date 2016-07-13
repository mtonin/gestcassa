#ifndef BACKGROUNDCONTROLLER_H
#define BACKGROUNDCONTROLLER_H

#include "commons.h"

#include <QDateTime>
#include <QtSql>
class DatabaseController : public QObject
{
    Q_OBJECT

public:
    DatabaseController();
    void setUltimaLetturaDB(const QDateTime par) {ultimaLetturaDB=par;}
    enum statoDatabase {
        Reset=0,
        Disconnesso,
        Connesso,
        DaAggiornare
    };
    Q_ENUM(statoDatabase)

public slots:
    void esegueControllo();
    void setFlagTermine(bool flag) {flagTermine=flag;}

signals:
    void messaggioDB(int codice, QString testo);

private slots:
    void controllaDatabase();

private:
    QDateTime ultimaLetturaDB;
    enum statoDatabase stato;
    bool flagTermine;
};

#endif // BACKGROUNDCONTROLLER_H
