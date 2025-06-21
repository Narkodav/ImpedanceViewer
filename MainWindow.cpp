#include "MainWindow.h"
#include "./ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->scrollArea->setWidgetResizable(true);

    // Create a line series (data points)
    QLineSeries *series = new QLineSeries();
    series->append(0, 6);  // (x, y)
    series->append(1, 3);
    series->append(2, 7);
    series->append(3, 8);
    series->append(4, 5);

    // Customize the line (optional)
    series->setName("Sample Data");
    series->setColor(Qt::blue);

    // Create a chart and add the series
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Simple Qt Chart Example");
    chart->createDefaultAxes();  // Auto-scales axes based on data

    // Enable animations (optional)
    chart->setAnimationOptions(QChart::AllAnimations);

    // Create a chart view to display the chart
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);  // Improves visual quality

    QWidget *graphContainer = new QWidget;
    ui->scrollArea->setWidget(graphContainer);
}

MainWindow::~MainWindow()
{
    delete ui;
}
