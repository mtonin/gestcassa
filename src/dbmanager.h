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
    bool init(const QString);

signals:

public slots:

private:
    bool createConnection(const QString& nomeFile, const QString& utente, const QString& password);
    void creaDb();
    bool leggeConfigurazione();

    QMap<QString, QVariant>* conf;
    QString dbFilePath;
};

#endif // DBMANAGER_H
