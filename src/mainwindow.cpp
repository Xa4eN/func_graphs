#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //изменение имени окна
    this->setWindowTitle("Графики функций");

    //установка стандартного масштаба при запуске программы
    ui->graphicWidget->xAxis->setRange(-5, 5);
    ui->graphicWidget->yAxis->setRange(-5, 5);

    //установка возможности масштабирования и смещения графика при помощи мыши
    ui->graphicWidget->setInteraction(QCP::iRangeZoom, true);
    ui->graphicWidget->setInteraction(QCP::iRangeDrag, true);

    //настройка цветовой палитры графика
    QFont font("Fixedsys", 9, QFont::Bold);
    ui->graphicWidget->setBackground(QBrush(QColor(75, 75, 75)));
    ui->graphicWidget->xAxis->grid()->setPen(QPen(QColor(204, 129, 0), 1, Qt::SolidLine));
    ui->graphicWidget->yAxis->grid()->setPen(QPen(QColor(204, 129, 0), 1, Qt::SolidLine));
    ui->graphicWidget->xAxis->grid()->setZeroLinePen(QPen(QColor(255, 190, 77), 2, Qt::SolidLine));
    ui->graphicWidget->yAxis->grid()->setZeroLinePen(QPen(QColor(255, 190, 77), 2, Qt::SolidLine));
    ui->graphicWidget->xAxis->setLabelFont(font);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//функция нажатия кнопки "Нарисовать"
void MainWindow::on_drawButton_clicked()
{
    //контроль входных данных
    QString warning_empty = "", warning_error = "", warning_syntax = "", warning_logic = "";

    if (ui->startEdit->text().isEmpty() || ui->stopEdit->text().isEmpty() || ui->funcEdit->text().isEmpty())
        warning_empty = " *все поля должны быть заполнены";

    if (ui->startEdit->text().toInt() == false && ui->startEdit->text() != "0" && ui->startEdit->text() != "")
        warning_syntax = " *диапазон значений функции должен быть задан целыми числами";

    if (ui->stopEdit->text().toInt() == false && ui->stopEdit->text() != "0" && ui->stopEdit->text() != "")
        warning_syntax = " *диапазон значений функции должен быть задан целыми числами";

    if (ui->startEdit->text().toInt() >= ui->stopEdit->text().toInt() && warning_syntax == "" && !(ui->stopEdit->text().isEmpty() || ui->funcEdit->text().isEmpty()))
        warning_logic = " *верхняя граница диапазона значений функции не должна превосходить нижнюю";

    //блок построения графика в случае отсутствия ошибок ввода
    if (warning_empty == "" && warning_syntax == "" && warning_logic == "")
    {

        //инициализация промежуточных переменных для расчёта значений функции и обработки точек разрыва 2-го рода
        QString result;
        QString xPositionText;
        int error = 0;
        bool infinity = false, error_all = false;
        int graphNum = 0;
        int infinity_missed = 0;
        const char* expression;

        //предварительная очистка виджета
        ui->graphicWidget->clearGraphs();
        args.clear();
        vals.clear();

        //присваивание начального и конечного значения промежутка расчёта
        start = ui->startEdit->text().toInt();
        stop = ui->stopEdit->text().toInt();

        xPosition = start;

        //расчёт векторных списков аргументов и значений функции и построение графика
        while (xPosition <= stop && error == 0)
        {
            do
            {
                //сохранение в переменной введённой пользователем функции и замена в ней аргумента "х" на текущее значение
                xPositionText.setNum(xPosition);
                result = ui->funcEdit->text();
                result.replace('x', xPositionText);
                expression = qPrintable(result);

                //расчёт значения функции, соответвующего текущему значению аргумента
                double finalValue;
                finalValue = te_interp(expression, &error);

                if (error != 0) error_all = true;   //проверка возможности интерпретации введённого текста в математическое выражение

                //расчёт значения функции, соответствующего следующему значению аргумента (смещение на один шаг) для обработки точек разрыва
                xPositionText.setNum(xPosition + step);
                result = ui->funcEdit->text();
                result.replace('x', xPositionText);
                expression = qPrintable(result);

                //проверка наличия точки разрыва 2-го рода путём нахождения модуля мгновенной производной функции в текущей точке
                if (abs((finalValue - te_interp(expression, 0))/step) > 1000) infinity = true;
                    else infinity_missed = 0;

                //добавление в векторный массив текущих значений аргумента и функции
                args.push_back(xPosition);
                vals.push_back(finalValue);

                //смещение аргумента на 1 шаг
                xPosition += step;
            }
            while (infinity == false && xPosition <= stop);  //завершение вложенного цикла при возникновении точки разрыва или при окончании промежутка построения

            //смещение аргумента на 1 шаг, выполняемое только в случае обнаружения точки разрыва или при окончании промежутка построения
            xPosition += step;

            //организация проверки для исключения возможности бесконечного добавления частей графика при неверном обнаружении точки разрыва
            if (infinity_missed < 3)
            {
                //добавление новой части графика после обхода точки разрыва
                ui->graphicWidget->addGraph();
                ui->graphicWidget->graph(graphNum)->addData(args, vals);
                ui->graphicWidget->graph(graphNum)->setPen(QPen(QColor(255, 31, 31), 2, Qt::SolidLine));
                infinity_missed += 1;

                //итерация переменной, хранящей текущий номер части графика
                graphNum += 1;

                //очистка векторных массивов значений агрументов и функций для последующего занесения данных, соответсвующих новой части графика
                args.clear();
                vals.clear();
            }

            infinity = false;
        }

        //сохранение информации о некорректности ввода в случае возврата ошибки при попытке вычисления текущего значения функции
        if (error_all == true)
            warning_error = " *поле <функция> содержит некорректную запись";

        //построение графика в случае отсутствия ошибок ввода
        if (warning_empty == "" && warning_error == "" && warning_logic == "" && warning_syntax == "") ui->graphicWidget->replot();

    }

    //вывод сообщения об ошибках ввода в случае их наличия
    if (!(warning_empty == "" && warning_error == "" && warning_logic == "" && warning_syntax == "")) QMessageBox::warning(this, "Неверные данные",
                   "При заполнении параметров функции были допущены следующие ошибки: "
                      + warning_empty + warning_error + warning_logic + warning_syntax +
                          ". Пожалуйста, исправьте ошибки и повторите ввод.", QMessageBox::Ok);
}


//функция нажатия кнопки "Очистить"
void MainWindow::on_clearButton_clicked()
{
    ui->graphicWidget->clearGraphs();

    ui->graphicWidget->replot();

    ui->funcEdit->clear();
    ui->startEdit->clear();
    ui->stopEdit->clear();
}


//функция нажатия кнопки "Центрировать"
void MainWindow::on_defaultButton_clicked()
{
    ui->graphicWidget->xAxis->setRange(-5, 5);
    ui->graphicWidget->yAxis->setRange(-5, 5);
    ui->graphicWidget->replot();
}


//функция нажатия кнопки "Выход"
void MainWindow::on_exitButton_clicked()
{
    qApp->exit();
}

