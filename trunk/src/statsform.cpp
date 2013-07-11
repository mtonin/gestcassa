#include "statsform.h"
#include <QtSql>
#include <QMessageBox>

StatsForm::StatsForm(const int idSessione, QWidget *parent) : idSessioneCorrente(idSessione), QDialog(parent)
{
  setupUi(this);
  setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint);

  fromData->setDate(QDate::currentDate());
  toData->setDate(QDate::currentDate());
  statsModel=new StatsModel;
  statsView->setModel(statsModel);

  statsView->verticalHeader()->hide();
  statsView->horizontalHeader()->setSortIndicatorShown(true);

  statsView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  statsView->horizontalHeader()->setStretchLastSection(true);

  connect(statsView->horizontalHeader(),SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),this,SLOT(ordinaByColumn(int)));

  caricaStats();

  statsView->hideColumn(2);
}

void StatsForm::on_filtraBtn_clicked()
{
  caricaStats();
}

void StatsForm::ordinaByColumn(int column)
{
  if(column==0) {
    column=2;
  }
  statsView->sortByColumn(column);
}

void StatsForm::caricaStats()
{
  graficoPlot->removePlottable(0);
  QCPBars* barPlot=new QCPBars(graficoPlot->xAxis,graficoPlot->yAxis);
  graficoPlot->addPlottable(barPlot);
  QVector<double> keyData;
  QVector<double> valueData;
  QVector<QString> tickLabelsData;
  QVector<double> tickData;
  int numero=0;

  QString sql("SELECT descrizione,sum(quantita) \
              FROM ordinicontenuto \
              where datetime(tsstampa) between ? and ? \
              and tipoArticolo <> ? \
              %1 \
              group by descrizione \
              order by 2 desc");
  QString condSessione;
  if(ultimaSessioneBox->isChecked()) {
    condSessione=QString("and idsessione=%1").arg(idSessioneCorrente);
  }
  sql=sql.arg(condSessione);
  QString tipoArticolo;
  if(expMenuBox->isChecked()) {
      tipoArticolo="M";
  } else {
      tipoArticolo="C";
  }
  QSqlQuery stmt;
  stmt.prepare(sql);

  QString from=QString("%1 %2").arg(fromData->date().toString("yyyy-MM-dd")).arg(fromOra->time().toString("hh:mm:ss"));
  QString to=QString("%1 %2").arg(toData->date().toString("yyyy-MM-dd")).arg(toOra->time().toString("hh:mm:ss"));
  stmt.addBindValue(from);
  stmt.addBindValue(to);
  stmt.addBindValue(tipoArticolo);
  if(!stmt.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),stmt.lastError().text());
    return;
  }
  statsModel->clear();
  while(stmt.next()) {
    QString nomeArticolo=stmt.value(0).toString();
    int quantita=stmt.value(1).toInt();
    QList<QStandardItem*> riga;
    riga.append(new QStandardItem(nomeArticolo));
    QStandardItem* quantitaItem=new QStandardItem;
    quantitaItem->setData(QVariant(quantita),Qt::EditRole);
    riga.append(quantitaItem);
    riga.append(new QStandardItem(nomeArticolo.toUpper()));
    statsModel->appendRow(riga);

    keyData.append(++numero);
    valueData.append(quantita);
    tickLabelsData.append(nomeArticolo);
    tickData.append(numero);
  }

  QFont font=graficoPlot->xAxis->tickLabelFont();
  font.setPointSize(8);
  graficoPlot->xAxis->setTickLabelFont(font);
  graficoPlot->xAxis->setAutoTicks(false);
  graficoPlot->xAxis->setAutoTickLabels(false);
  barPlot->setData(keyData,valueData);
  graficoPlot->xAxis->setTickVector(tickData);
  graficoPlot->xAxis->setTickVectorLabels(tickLabelsData);
  graficoPlot->xAxis->setTickLabelRotation(60);
  graficoPlot->xAxis->setSubTickCount(0);
  graficoPlot->xAxis->setTickLength(0, 4);
  graficoPlot->xAxis->setGrid(false);
  graficoPlot->xAxis->setRange(0, ++numero);
  graficoPlot->xAxis->setPadding(10);
  graficoPlot->setAutoMargin(true);
  graficoPlot->rescaleAxes();
  graficoPlot->replot();

  graficoPlot->setRangeDrag(Qt::Horizontal|Qt::Vertical);
  graficoPlot->setRangeZoom(Qt::Horizontal|Qt::Vertical);

  statsView->horizontalHeader()->setSortIndicator(1,Qt::DescendingOrder);
}
