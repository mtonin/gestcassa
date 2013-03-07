#ifndef DBDIALOG_H
#define DBDIALOG_H

#include "ui_dbdialog.h"

class DBDialog : public QDialog, private Ui::DBDialog
{
      Q_OBJECT
      
   public:
      explicit DBDialog(QWidget *parent = 0);
      QString getDbFileName() {return dbfile->text();}
      QString getUser() {return utente->text();}
      QString getPassword() {return password->text();}

   private slots:
      void on_toolButton_clicked();

private:
};

#endif // DBDIALOG_H
