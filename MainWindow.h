#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QtCharts>
#include <QString>

#include "ImpedanceReading.h"
#include "IMatrixValueCalculator.h"
#include "MatrixValueCalculators.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_AddReadingButton_clicked();

    void on_graphViewWidget_chartRemoved(const size_t &);

    void on_magnitudeMatrixCalculatorChange_clicked();

    void on_phaseMatrixCalculatorChange_clicked();

    void on_useDefaultMagnitudeCalculator_clicked();

    void on_useDefaultPhaseCalculator_clicked();

private:
    void updateMagnitudeMatrix();
    void updatePhaseMatrix();

private:
    Ui::MainWindow *ui;
    IMatrixValueCalculator* calculatorMagnitude;
    IMatrixValueCalculator* calculatorPhase;
};
#endif // MAINWINDOW_H
