#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QMap>

class DBManager : public QObject
{
  Q_OBJECT
public:
  explicit DBManager(QObject *parent = 0);
  explicit DBManager(QMap<QString,QVariant>* configurazione);

signals:
  
public slots:
  
private:
      bool createConnection(const QString& nomeFile, const QString& utente, const QString& password);
      void creaDb();
      void leggeConfigurazione();

      QMap<QString,QVariant>* conf;
      QString dbFilePath;
};

#endif // DBMANAGER_H
