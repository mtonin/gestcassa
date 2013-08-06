#ifndef DBDIALOG_H
#define DBDIALOG_H

#include "ui_dbdialog.h"

class DBDialog : public QDialog, private Ui::DBDialog
{
      Q_OBJECT
      
   public:
      explicit DBDialog(QMap<QString,QVariant>* configurazione,QWidget *parent = 0);
      QString getPassword() {return password->text();}

   private slots:
      void on_apreBtn_clicked();

      void on_esceBtn_clicked();

private:
      bool createConnection(const QString& nomeFile, const QString& utente, const QString& password);
      void creaDb();
      void leggeConfigurazione();

      QMap<QString,QVariant>* conf;
      QString dbFilePath;
};

#endif // DBDIALOG_H