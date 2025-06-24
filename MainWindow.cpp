#include "MainWindow.h"
#include "./ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    calculatorMagnitude = new MatrixValueCalculatorDefault();
    calculatorPhase = new MatrixValueCalculatorDefault();

    ui->useDefaultMagnitudeCalculator->hide();
    ui->useDefaultPhaseCalculator->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete calculatorMagnitude;
    delete calculatorPhase;
}


void MainWindow::on_AddReadingButton_clicked()
{
    QStringList filePaths = QFileDialog::getOpenFileNames(this,
            tr("Reading file selection"),
            QCoreApplication::applicationDirPath(),
            tr("Text files (*.txt *.csv *.lvm);;All Files (*)"));
    if(filePaths.isEmpty())
        return;
    for(auto& filePath : filePaths)
    {
        ImpedanceReading reading;
        if(ImpedanceReading::getReading(reading, filePath))
        {
            ui->graphViewWidget->addReading(std::move(reading), "Magnitude", "Phase");
            auto list = ui->graphViewWidget->getList();
            ui->magnitudeMatrixWidget->insertRow(list->count() - 1);
            ui->magnitudeMatrixWidget->insertColumn(list->count() - 1);

            ui->phaseMatrixWidget->insertRow(list->count() - 1);
            ui->phaseMatrixWidget->insertColumn(list->count() - 1);

            for(size_t i = 0; i < list->count(); i++)
            {
                auto seriesPair = ui->graphViewWidget->getChartsAt(i);

                try
                {
                    auto value = calculatorMagnitude->getValue(reading.getMagnitudeSeries(), seriesPair.first);
                    ui->magnitudeMatrixWidget->setItem(i, list->count() - 1, new QTableWidgetItem(QString::number(value)));
                    ui->magnitudeMatrixWidget->setItem(list->count() - 1, i, new QTableWidgetItem(QString::number(value)));

                    value = calculatorPhase->getValue(reading.getPhaseSeries(), seriesPair.second);
                    ui->phaseMatrixWidget->setItem(i, list->count() - 1, new QTableWidgetItem(QString::number(value)));
                    ui->phaseMatrixWidget->setItem(list->count() - 1, i, new QTableWidgetItem(QString::number(value)));
                }
                catch(std::runtime_error e)
                {
                    QMessageBox::information(this, "Script runtime erorr", e.what());
                    return;
                }
            }
        }
    }
    recolorTable(ui->magnitudeMatrixWidget);
    recolorTable(ui->phaseMatrixWidget);
}

void MainWindow::on_graphViewWidget_chartRemoved(const size_t & row)
{
    ui->magnitudeMatrixWidget->removeRow(row);
    ui->magnitudeMatrixWidget->removeColumn(row);    

    ui->phaseMatrixWidget->removeRow(row);
    ui->phaseMatrixWidget->removeColumn(row);

    recolorTable(ui->magnitudeMatrixWidget);
    recolorTable(ui->phaseMatrixWidget);
}

void MainWindow::on_magnitudeMatrixCalculatorChange_clicked()
{
    auto filePath = QFileDialog::getOpenFileName(
        this,
        tr("Script selection"),
        QCoreApplication::applicationDirPath(),
        tr("JavaScript Files (*.js);;Text Files (*.txt);;All Files (*)")
        );
    if(filePath == "")
        return;
    try
    {
        IMatrixValueCalculator* newCalculator = new MatrixValueCalculatorExternalScript(filePath);
        delete calculatorMagnitude;
        calculatorMagnitude = newCalculator;
    }
    catch(std::runtime_error e)
    {
        QMessageBox::information(this, "Script parsing error", e.what());
        return;
    }

    updateMagnitudeMatrix();

    ui->useDefaultMagnitudeCalculator->show();
    recolorTable(ui->magnitudeMatrixWidget);
    recolorTable(ui->phaseMatrixWidget);
}

void MainWindow::on_phaseMatrixCalculatorChange_clicked()
{
    auto filePath = QFileDialog::getOpenFileName(
        this,
        tr("Script selection"),
        QCoreApplication::applicationDirPath(),
        tr("JavaScript Files (*.js);;Text Files (*.txt);;All Files (*)")
        );
    if(filePath == "")
        return;
    try
    {
        IMatrixValueCalculator* newCalculator = new MatrixValueCalculatorExternalScript(filePath);
        delete calculatorPhase;
        calculatorPhase = newCalculator;
    }
    catch(std::runtime_error e)
    {
        QMessageBox::information(this, "Script parsing error", e.what());
        return;
    }

    updatePhaseMatrix();

    ui->useDefaultPhaseCalculator->show();
    recolorTable(ui->magnitudeMatrixWidget);
    recolorTable(ui->phaseMatrixWidget);
}

void MainWindow::on_useDefaultMagnitudeCalculator_clicked()
{
    IMatrixValueCalculator* newCalculator = new MatrixValueCalculatorDefault();
    delete calculatorMagnitude;
    calculatorMagnitude = newCalculator;

    updateMagnitudeMatrix();

    ui->useDefaultMagnitudeCalculator->hide();
    recolorTable(ui->magnitudeMatrixWidget);
    recolorTable(ui->phaseMatrixWidget);
}


void MainWindow::on_useDefaultPhaseCalculator_clicked()
{
    IMatrixValueCalculator* newCalculator = new MatrixValueCalculatorDefault();
    delete calculatorPhase;
    calculatorPhase = newCalculator;

    updatePhaseMatrix();

    ui->useDefaultPhaseCalculator->hide();
    recolorTable(ui->magnitudeMatrixWidget);
    recolorTable(ui->phaseMatrixWidget);
}

void MainWindow::updateMagnitudeMatrix()
{
    auto list = ui->graphViewWidget->getList();

    for(size_t i = 0; i < list->count(); i++)
    {
        auto seriesPairI = ui->graphViewWidget->getChartsAt(i);
        for(size_t j = i; j < list->count(); j++)
        {
            auto seriesPairJ = ui->graphViewWidget->getChartsAt(j);
            try
            {
                auto value = calculatorMagnitude->getValue(seriesPairI.first, seriesPairJ.first);
                ui->magnitudeMatrixWidget->setItem(i, j, new QTableWidgetItem(QString::number(value)));
                ui->magnitudeMatrixWidget->setItem(j, i, new QTableWidgetItem(QString::number(value)));
            }
            catch(std::runtime_error e)
            {
                QMessageBox::information(this, "Script runtime erorr", e.what());
                return;
            }
        }
    }
}

void MainWindow::updatePhaseMatrix()
{
    auto list = ui->graphViewWidget->getList();

    for(size_t i = 0; i < list->count(); i++)
    {
        auto seriesPairI = ui->graphViewWidget->getChartsAt(i);
        for(size_t j = i; j < list->count(); j++)
        {
            auto seriesPairJ = ui->graphViewWidget->getChartsAt(j);
            try
            {
                auto value = calculatorPhase->getValue(seriesPairI.second, seriesPairJ.second);
                ui->phaseMatrixWidget->setItem(i, j, new QTableWidgetItem(QString::number(value)));
                ui->phaseMatrixWidget->setItem(j, i, new QTableWidgetItem(QString::number(value)));
            }
            catch(std::runtime_error e)
            {
                QMessageBox::information(this, "Script runtime erorr", e.what());
                return;
            }
        }
    }
}

void MainWindow::recolorTable(QTableWidget* table)
{
    double minVal = findMin(table);
    if (minVal == 0) minVal = 0.000000001; // Avoid division by zero

    for (int row = 0; row < table->rowCount(); ++row) {
        for (int col = row + 1; col < table->columnCount(); ++col) {
            if (QTableWidgetItem* item = table->item(row, col)) {
                bool ok = false;
                double value = item->text().toDouble(&ok);
                if (ok) {
                    // Normalize value between 0 and 1
                    double normalized = minVal / value;

                    // Calculate gradient color (green to red)
                    int red = static_cast<int>(255 * (1 - normalized));
                    int green = static_cast<int>(255 * normalized);
                    int blue = 0;

                    // Set background color
                    item->setBackground(QColor(red, green, blue));

                    // Set text color for better contrast
                    //item->setForeground(normalized > 0.5 ? Qt::white : Qt::black);

                    QTableWidgetItem* itemOp = table->item(col, row);
                    itemOp->setBackground(QColor(red, green, blue));
                    //itemOp->setForeground(normalized > 0.5 ? Qt::white : Qt::black);
                }
            }
        }
    }
}

double MainWindow::findMin(QTableWidget* table)
{
    double minVal = std::numeric_limits<double>::max();
    for (int row = 0; row < table->rowCount(); ++row) {
        for (int col = row + 1; col < table->columnCount(); ++col) {
            if (QTableWidgetItem* item = table->item(row, col)) {
                bool ok;
                double value = item->text().toDouble(&ok);
                if (ok && value < minVal) {
                    minVal = value;
                }
            }
        }
    }
    return minVal;
}

