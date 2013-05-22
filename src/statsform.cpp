#include "statsform.h"

#include <QtSql>
#include <QMessageBox>
#include <QVector>

StatsForm::StatsForm(QWidget *parent) :
  QDialog(parent)
{
  setupUi(this);

  QCPBars* barPlot=new QCPBars(graficoPlot->xAxis,graficoPlot->yAxis);
  graficoPlot->addPlottable(barPlot);
  QVector<double> keyData;
  QVector<double> valueData;
  QVector<QString> tickLabelsData;
  QVector<double> tickData;
  QSqlQuery stmt("SELECT dettagliordine.descrizione,sum(dettagliordine.quantita) \
                  FROM dettagliordine,ordini \
                  where dettagliordine.numeroordine=ordini.numero \
                                                    group by descrizione \
                                                    order by 2 desc");

  int numero=0;
  if(!stmt.exec()) {
     QMessageBox::critical(0, QObject::tr("Database Error"),
     stmt.lastError().text());
     return;
  }
  while(stmt.next()) {
    QString nomeArt=stmt.value(0).toString();
    int totaleArt=stmt.value(1).toInt();
    keyData.append(++numero);
    valueData.append(totaleArt);
    tickLabelsData.append(nomeArt);
    tickData.append(numero);
  }

  graficoPlot->xAxis->setAutoTicks(false);
  graficoPlot->xAxis->setAutoTickLabels(false);
  barPlot->setData(keyData,valueData);
  graficoPlot->xAxis->setTickVector(tickData);
  graficoPlot->xAxis->setTickVectorLabels(tickLabelsData);
  graficoPlot->xAxis->setTickLabelRotation(90);
  graficoPlot->xAxis->setSubTickCount(0);
  graficoPlot->xAxis->setTickLength(0, 4);
  graficoPlot->xAxis->setGrid(false);
  graficoPlot->xAxis->setRange(0, 10);
  graficoPlot->xAxis->setPadding(10);
  graficoPlot->setAutoMargin(true);
  graficoPlot->rescaleAxes();
  graficoPlot->replot();

}
