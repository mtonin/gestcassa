#ifndef ABSTRACTMSGBOX_H
#define ABSTRACTMSGBOX_H

#include <QWidget>
#include <QGridLayout>
#include <QPixmap>
#include <QToolButton>
#include <QLabel>
#include <QVBoxLayout>

class TitoloBar:public QWidget {
  Q_OBJECT
public:
  explicit TitoloBar(QWidget* parent=0);
  ~TitoloBar();

signals:
  void close();

protected:
  void resizeEvent(QResizeEvent *event);
  void paintEvent(QPaintEvent *event);

private slots:
  void termina();

private:
  QPixmap* m_cache;
  QToolButton m_closeBtn;
};

class BaseMsgBox : public QWidget
{
  Q_OBJECT
public:
  explicit BaseMsgBox(QWidget *parent = 0);
  ~BaseMsgBox();
  void setTesto(QString testo);

signals:

public slots:

protected:
  void paintEvent(QPaintEvent *event);
  void resizeEvent(QResizeEvent *event);

private:
  QGridLayout m_mainLayout;
  QVBoxLayout m_textLayout;
  TitoloBar m_titolo;
  QPixmap* m_cache;
  QLabel m_testo;
};

#endif // ABSTRACTMSGBOX_H
