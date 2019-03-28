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
  auto lineErr = new QLineSeries();
  lineErr->setColor(Qt::red);

  for (int i = 0; i < N * 2; i++)
    {
      double y = out[0].y() / 2 * qCos(out[0].x());

      for (int j = 1; j < maN; j++)
        {
          y += out[j].y() * qCos(j * i * 2 * M_PI / N + out[j].x());
        }

      lineOut->append(i, y);

      if (i < N)
        {
          lineErr->append(i, y - in[i]);
        }
    }

  auto l1 = ui->ch1->chart()->series()[0];
  ui->ch1->chart()->removeSeries(l1);
  ui->ch1->chart()->removeAllSeries();
  ui->ch1->chart()->addSeries(l1);
  ui->ch1->chart()->addSeries(lineOut);
  ui->ch1->chart()->addSeries(lineErr);
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
  count = QRandomGenerator::global()->bounded(1, 10);
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

      if (k == 0 || periodic(k))
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
  return ui->all->isChecked()
         || x == 1
         || x == round(1.0 * N / 10)
         || x == round(1.0 * N / 15)
         || x == round(1.0 * N / (7.0 / 2))
         || x == round(1.0 * N / 7)
         || x == round(1.0 * N / (7 * 2))
         || x == round(1.0 * N / (7 * 3))
         || x == round(1.0 * N / (7 * 4))
         || x == round(1.0 * N / (7 * 6))
         || x == round(1.0 * N / (7 * 8))
         || x == round(1.0 * N / (7 * 12))
         || x == round(1.0 * N / (7 * 16))
         || x == round(1.0 * N / (7 * 24))
         || x == round(1.0 * N / (365.0 / 12))
         || x == round(1.0 * N / (365.0 / 12 * 2))
         || x == round(1.0 * N / (365.0 / 12 * 3))
         || x == round(1.0 * N / (365.0 / 12 * 4))
         || x == round(1.0 * N / (365.0 / 12 * 6))
         ;
}

void MainWindow::on_pushButton_3_clicked()
{
  ui->ch1->chart()->removeAllSeries();
  ui->kfsss->clear();
  transactions.clear();
  freqs.clear();
  QString result, format;
  format = "d = %1, a = %2\n";
  QFile data("a.csv");
  bool plus = ui->plus->isChecked();
  bool minus = ui->minus->isChecked();

  if (data.open(QFile::ReadOnly))
    {
      QTextStream ds(&data);
      QString line;
      in.clear();
      int trcount = 0;

      while (ds.readLineInto(&line))
        {
          trcount++;
          QStringList tokens = line.split(',');
          double value = tokens[0].toDouble();

          if ((plus && tokens[2] == "1") || (minus && tokens[2] == "2"))
            {
              if (tokens[2] == "2")
                { value = -value; }

              if (!freqs.contains(value))
                {
                  freqs[value] = 1;
                }
              else
                {
                  freqs[value] += 1;
                }
            }
        }

      QList<int> values = freqs.values();
      QString aba;
      std::sort(values.rbegin(), values.rend());
      double v0 = freqs.key(values[0]);
      double v1 = freqs.key(values[1]);
      double v2 = freqs.key(values[2]);
      double v3 = freqs.key(values[3]);
      double v4 = freqs.key(values[4]);
      ds.seek(0);
      int minfreq = ui->minFreq->value();

      while (ds.readLineInto(&line))
        {
          QStringList tokens = line.split(',');
          QDate date = QDate::fromString(tokens[1], Qt::ISODate);

          if (!transactions.contains(date))
            { transactions[date] = 0; }

          double value = tokens[0].toDouble();

          if (tokens[2] == "2")
            { value = -value; }

          if (false
              || value == v0
              || value == v1
              || value == v2
              || value == v3
              || value == v4
              || true
             )
            if (freqs[value] >= minfreq)
              if ((plus && tokens[2] == "1") || (minus && tokens[2] == "2"))
                {
                  transactions[date] += value;
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
  lastKKK = false;
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
  lastKKK = false;
  currN = 2;
  ttt->start(50);
  ui->progress->setMaximum(N / 2);
}

void MainWindow::on_pushButton_6_clicked()
{
  lastKKK = true;
  kkk->stop();
  ui->progress->setMaximum(N / 2);
  restoreKKK(N / 2);
}

void MainWindow::on_pushButton_7_clicked()
{
  lastKKK = true;
  currN = 2;
  kkk->start(50);
  ui->progress->setMaximum(N / 2);
}

void MainWindow::on_progress_valueChanged(int value)
{
  if (lastKKK)
    { restoreKKK(value); }
  else
    { restore(value); }
}
