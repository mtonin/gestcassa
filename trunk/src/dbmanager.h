#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QMap>

class DBManager : public QObject
{
    Q_OBJECT
public:
    explicit DBManager(QObject *parent = 0);
    explicit DBManager(QMap<QString, QVariant>* configurazione);
    bool init(const QString, const QString modello);

signals:

public slots:

private:
    bool createConnection(const QString& nomeFile, const QString& utente, const QString& password, const QString modello);
    void creaDb(const QString user, const QString password, const QString modello);
    bool leggeConfigurazione();

    QMap<QString, QVariant>* conf;
    QString dbFilePath;
};

#endif // DBMANAGER_H
