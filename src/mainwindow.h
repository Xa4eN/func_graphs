#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QString>

#include "tinyexpr.h"
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_drawButton_clicked();

    void on_clearButton_clicked();

    void on_defaultButton_clicked();

    void on_exitButton_clicked();

private:
    Ui::MainWindow *ui;

    double start, stop, step = 0.01, xPosition;
    QVector<double> args, vals;
};
#endif // MAINWINDOW_H
