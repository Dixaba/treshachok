#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QVector>
#include <QVector2D>
#include <QtMath>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  QChart *chart = new QChart();
  chart->legend()->hide();
  ui->ch1->setChart(chart);
  ttt = new QTimer(this);
  kkk = new QTimer(this);
  connect(ttt, SIGNAL(timeout()), this, SLOT(onTimer()));
  connect(kkk, SIGNAL(timeout()), this, SLOT(onTimer2()));
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::restore(int maN)
{
  auto lineOut = new QLineSeries();
  lineOut->setColor(Qt::green);
  auto lineMed = new QLineSeries();
  lineMed->setColor(Qt::red);

  for (int i = 0; i < N * 2; i++)
    {
      double y = out[0].y() / 2 * qCos(out[0].x());

      for (int j = 1; j < maN; j++)
        {
          y += out[j].y() * qCos(j * i * 2 * M_PI / N + out[j].x());
        }

      lineOut->append(i, y);

      if (i != 0 && i < N)
        { lineMed->append(i, (in[i] + in[i - 1]) / 2); }
    }

  auto l1 = ui->ch1->chart()->series()[0];
  ui->ch1->chart()->removeSeries(l1);
  ui->ch1->chart()->removeAllSeries();
  ui->ch1->chart()->addSeries(l1);
  ui->ch1->chart()->addSeries(lineOut);
  ui->ch1->chart()->addSeries(lineMed);
  ui->ch1->chart()->createDefaultAxes();
  ui->progress->setValue(maN);
}

void MainWindow::restoreKKK(int maN)
{
  double balance = 0;
  auto lineOut = new QLineSeries();
  lineOut->setColor(Qt::red);

  for (int i = 0; i < N * 2; i++)
    {
      double y = out[0].y() / 2 * qCos(out[0].x());

      for (int j = 0; j < maN; j++)
        {
          y += out[j].y() * qCos(j * i * 2 * M_PI / N + out[j].x());
        }

      balance += y;
      lineOut->append(i, balance);

      if (i == N)
        { ui->tlout->setText(QString::number(y, 'f', 2)); }
    }

  ui->ch1->chart()->removeAllSeries();
  ui->ch1->chart()->addSeries(lineOut);
  ui->ch1->chart()->createDefaultAxes();
  ui->progress->setValue(maN);
}

void MainWindow::on_pushButton_clicked()
{
  N = 365;
  ui->ch1->chart()->removeAllSeries();
  koefs.clear();
  mults.clear();
  phases.clear();
  in.clear();
  ui->kfsss->clear();
  QString result, format;
  format = "k = %1, w = %2, ph = %3\n";
  count = QRandomGenerator::global()->bounded(1, N);
  result = QString::number(count);
  result += '\n';

  for (int i = 0; i < count; i++)
    {
      koefs.push_back(QRandomGenerator::global()->generateDouble());
      phases.push_back(QRandomGenerator::global()->generateDouble() * 2 * M_PI);
      mults.push_back(QRandomGenerator::global()->bounded(1, N / 2));
      result += format.arg(koefs.last()).arg(mults.last()).arg(phases.last());
    }

  ui->kfsss->appendPlainText(result);
  auto lineIn = new QLineSeries();
  lineIn->setColor(Qt::blue);

  for (int i = 0; i < N; i++)
    {
      in.append(f(1.0 * i));
      lineIn->append(i, f(1.0 * i));
    }

  ui->ch1->chart()->addSeries(lineIn);
  ui->ch1->chart()->createDefaultAxes();
}

void MainWindow::on_pushButton_2_clicked()
{
  out.clear();
  ui->text->clear();
  QString result, format;
  format = "i = %1, mag = %2, ph = %3\n";

  for (int k = 0; k < N / 2; k++) // For each output element
    {
      double sumreal = 0;
      double sumimag = 0;

      if (periodic(k))
        {
          for (int t = 0; t < N; t++) // For each input element
            {
              double angle = 2 * M_PI * t * k / N;
              sumreal += in[t] * qCos(angle);
              sumimag += -in[t] * qSin(angle);
            }
        }

      double mag = sqrt(pow(sumreal, 2) + pow(sumimag, 2)) * 2 / N;
      double ph = qAtan2(sumimag, sumreal);
      QVector2D p(ph, mag);
      out.append(p);

      if (mag > 0.000001)
        {
          result += format.arg(k).arg(mag).arg(ph);
        }
    }

  ui->text->appendPlainText(result);
}

double MainWindow::f(double x)
{
  x *= 2 * M_PI / N;
  double result = 0.0;

  for (int i = 0; i < count; i++)
    {
      result += koefs[i] * qCos(x * mults[i] + phases[i]);
    }

  return result;
}

double MainWindow::f(QDate date)
{
  return transactions[date];
}

bool MainWindow::periodic(int x)
{
  return false
         || x == N / 2
         || x == N / 15
         || x == N / (7 / 2)
         || x == N / 7
         || x == N / (7 * 2)
         || x == N / (7 * 3)
         || x == N / (7 * 4)
         || x == N / (7 * 6)
         || x == N / (365 / 12)
         || x == N / (365 / 12 * 2)
         || x == N / (365 / 12 * 3)
         || x == N / (365 / 12 * 4)
         || x == N / (365 / 12 * 6)
         || x == N / 30
         || x == N / (30 * 2)
         || x == N / (30 * 3)
         || x == N / (30 * 4)
         || x == N / (30 * 6)
         //         || true
         ;
}

void MainWindow::on_pushButton_3_clicked()
{
  ui->ch1->chart()->removeAllSeries();
  ui->kfsss->clear();
  transactions.clear();
  QString result, format;
  format = "d = %1, a = %2\n";
  QFile data("a.csv");
  bool plus = ui->plus->isChecked();
  bool minus = ui->minus->isChecked();

  if (data.open(QFile::ReadOnly))
    {
      QTextStream ds(&data);
      QString line;
      ds.readLineInto(&line);
      in.clear();

      while (ds.readLineInto(&line))
        {
          QStringList tokens = line.split(',');
          QDate date = QDate::fromString(tokens[1], Qt::ISODate);

          if (!transactions.contains(date))
            { transactions[date] = 0; }

          if (plus && tokens[2] == "1")
            {
              transactions[date] += tokens[0].toDouble();
            }

          if (minus && tokens[2] == "2")
            {
              transactions[date] -= tokens[0].toDouble();
            }
        }
    }

  auto lineIn = new QLineSeries();
  lineIn->setColor(Qt::blue);
  QDate begin = transactions.firstKey();
  int days = begin.daysTo(transactions.lastKey());
  N = days;

  for (int i = 0; i < N; i++)
    {
      in.append(f(begin.addDays(i)));
      lineIn->append(i, f(begin.addDays(i)));
      result += format.arg(begin.addDays(i).toString()).arg(f(begin.addDays(i)));
    }

  ui->kfsss->appendPlainText(result);
  ui->ch1->chart()->addSeries(lineIn);
  ui->ch1->chart()->createDefaultAxes();
}

void MainWindow::on_pushButton_4_clicked()
{
  ttt->stop();
  ui->progress->setMaximum(N / 2);
  restore(N / 2);
}

void MainWindow::onTimer()
{
  currN++;

  if (currN > N / 2)
    {
      ttt->stop();
      return;
    }

  restore(currN);
}

void MainWindow::onTimer2()
{
  currN++;

  if (currN > N / 2)
    {
      kkk->stop();
      return;
    }

  restoreKKK(currN);
}

void MainWindow::on_pushButton_5_clicked()
{
  currN = 2;
  ttt->start(50);
  ui->progress->setMaximum(N / 2);
}

void MainWindow::on_pushButton_6_clicked()
{
  kkk->stop();
  ui->progress->setMaximum(N / 2);
  restoreKKK(N / 2);
}

void MainWindow::on_pushButton_7_clicked()
{
  currN = 2;
  kkk->start(50);
  ui->progress->setMaximum(N / 2);
}
