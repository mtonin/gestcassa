#ifndef ALERTTESSEREDLG_H
#define ALERTTESSEREDLG_H

#include <QtNetwork>
#include "popupdialog.h"

#include "ui_alerttesseredlg.h"

class ReplyTimeout : public QObject {
  Q_OBJECT
  QBasicTimer m_timer;
public:
  ReplyTimeout(QNetworkReply* reply, const int timeout) : QObject(reply) {
    Q_ASSERT(reply);
    if (reply && reply->isRunning())
      m_timer.start(timeout, this);
  }
  static void set(QNetworkReply* reply, const int timeout) {
    new ReplyTimeout(reply, timeout);
  }
protected:
  void timerEvent(QTimerEvent * ev) {
    if (!m_timer.isActive() || ev->timerId() != m_timer.timerId())
      return;
    auto reply = static_cast<QNetworkReply*>(parent());
    if (reply->isRunning()) {
      reply->close();
    }
    m_timer.stop();
  }
};

class AlertTessereDlg : public QDialog, private Ui::AlertTessereDlg
{
    Q_OBJECT

public:
    explicit AlertTessereDlg(QString address,QPoint pos, QWidget *parent = 0);

private slots:
    void init();
    void on_chiudeBtn_clicked();
    void on_codiceTesseraRicercaTxt_textEdited(const QString &arg1);
    void on_queryBtn_clicked();

    void on_attivatBtn_clicked();

    void on_disattivaBtn_clicked();

private:
    QString endpoint;
    QNetworkAccessManager* netManager;
    QByteArray esegueRichiestaHttp(QUrl url);
};

#endif // ALERTTESSEREDLG_H
