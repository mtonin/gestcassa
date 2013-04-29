#ifndef DBDIALOG_H
#define DBDIALOG_H

#include "ui_dbdialog.h"

class DBDialog : public QDialog, private Ui::DBDialog
{
      Q_OBJECT
      
   public:
      explicit DBDialog(QMap<QString,QVariant>* configurazione,QWidget *parent = 0);
      QString getDbFileName() {return dbfile->text();}
      QString getPassword() {return password->text();}

   private slots:
      void on_toolButton_clicked();

      void on_apreBtn_clicked();

      void on_esceBtn_clicked();

      void on_nuovoBtn_clicked();

private:
      bool createConnection(const QString& nomeFile, const QString& utente, const QString& password);
      QMap<QString,QVariant>* conf;
};

#endif // DBDIALOG_H
