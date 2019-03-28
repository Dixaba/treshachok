#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QtCharts>
#include <QTimer>

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void restore(int maN);
    void restoreKKK(int maN);

  private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void onTimer();
    void onTimer2();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

  private:
    Ui::MainWindow *ui;

    double f(double x);
    double f(QDate date);
    bool periodic(int x);

    int N = 1000;
    int currN, count;
    QVector<double> in;
    QVector<QVector2D> out;
    QVector<int> mults;
    QVector<double> koefs;
    QVector<double> phases;
    QMap<QDate, double> transactions;
    QMap<double, int> freqs;
    QTimer *ttt;
    QTimer *kkk;

};

#endif // MAINWINDOW_H
