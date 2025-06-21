#ifndef GRAPHWINDOW_H
#define GRAPHWINDOW_H

#include "InteractiveChartView.h"

#include <QMainWindow>
#include <QtWidgets/QApplication>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollBar>
#include <QGroupBox>
#include <QFormLayout>

QT_USE_NAMESPACE

class GraphWindow : public QWidget
{
    using RowChartPair = std::pair<InteractiveChartView*, InteractiveChartView*>;
private:
    QFormLayout *graphLayout;
    QVBoxLayout *mainLayout;
    QScrollArea *scrollArea;
    QWidget *graphContainer;
    QPushButton *addButton;
    QPushButton *removeButton;
    QScrollBar *scrollBar;

public:

    GraphWindow(QWidget *parent = nullptr) : QWidget(parent) {
        // Main layout
        mainLayout = new QVBoxLayout(this);

        // Create scroll area
        scrollArea = new QScrollArea();
        scrollArea->setWidgetResizable(true); // Important for proper scrolling
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // Create container widget for the scroll area
        graphContainer = new QWidget();
        graphLayout = new QFormLayout(graphContainer);
        graphLayout->setAlignment(Qt::AlignTop); // Align graphs at the top
        graphLayout->setSpacing(20); // Space between rows

        // Set up the scroll area
        scrollArea->setWidget(graphContainer);
        mainLayout->addWidget(scrollArea);

        // Add button (outside scroll area for better UX)
        addButton = new QPushButton("Add New Graph");
        removeButton = new QPushButton("Remove Last Graph");
        connect(addButton, &QPushButton::clicked, this, &GraphWindow::generateGraphPair);
        connect(removeButton, &QPushButton::clicked, this, &GraphWindow::popBack);

        mainLayout->addWidget(addButton);
        mainLayout->addWidget(removeButton);
        // Add initial graphs
        //addGraphPairS("Magnitude (dB)");
        //addGraphPairS("Impedance (Ω)");
    }

public slots:
    void generateGraphPair() {
        generateGraphPairS(QString("Graph %1").arg(graphLayout->count() + 1));
    }

    void generateGraphPairS(const QString &title) {
        auto viewOne = makeChartView("1");
        auto viewTwo = makeChartView("2");
        auto pair = std::make_pair(viewOne, viewTwo);

        pushBack(std::move(pair), "1", "2");
    }

    void pushBack(RowChartPair&& pair, const QString &title1, const QString &title2)
    {
        auto lastRawLayout = new QHBoxLayout();
        graphLayout->addRow(lastRawLayout);

        connect(pair.first, &InteractiveChartView::chartDoubleClicked,
                [this](QChart *clickedChart) {
                    QChart* chartCopy = new QChart();
                    QLineSeries *seriesCopy = new QLineSeries();

                    for(QAbstractSeries *series : clickedChart->series())
                    {
                        QLineSeries *original = qobject_cast<QLineSeries*>(series);
                        for(int i = 0; i < original->count(); i++)
                            seriesCopy->append(original->at(i).x(), original->at(i).y());
                    }
                    chartCopy->addSeries(seriesCopy);
                    chartCopy->setTitle(clickedChart->title());
                    chartCopy->createDefaultAxes();
                    chartCopy->legend()->setVisible(false);
                    ChartPopup *popup = new ChartPopup(chartCopy, this);
                    popup->setAttribute(Qt::WA_DeleteOnClose);
                    popup->exec();
                });

        connect(pair.second, &InteractiveChartView::chartDoubleClicked,
                [this](QChart *clickedChart) {
                    QChart* chartCopy = new QChart();
                    QLineSeries *seriesCopy = new QLineSeries();

                    for(QAbstractSeries *series : clickedChart->series())
                    {
                        QLineSeries *original = qobject_cast<QLineSeries*>(series);
                        for(int i = 0; i < original->count(); i++)
                            seriesCopy->append(original->at(i).x(), original->at(i).y());
                    }
                    chartCopy->addSeries(seriesCopy);
                    chartCopy->setTitle(clickedChart->title());
                    chartCopy->createDefaultAxes();
                    chartCopy->legend()->setVisible(false);
                    ChartPopup *popup = new ChartPopup(chartCopy, this);
                    popup->setAttribute(Qt::WA_DeleteOnClose);
                    popup->exec();
                });

        lastRawLayout->addWidget(pair.first);
        lastRawLayout->addWidget(pair.second);
        //lastRawLayout->setAlignment(Qt::Alignment::enum_type::AlignHCenter);
    }

    void popBack()
    {
        if (graphLayout->count() == 0) return;

        // Remove the row's layout
        graphLayout->removeRow(graphLayout->count() - 1);
    }

private:
    InteractiveChartView* makeChartView(const QString &title) {
        QLineSeries *series = new QLineSeries();
        // Generate random data for demo
        for (int i = 0; i < 5; ++i)
            series->append(i, rand() % 10);

        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle(title);
        chart->createDefaultAxes();
        chart->legend()->setVisible(false);
        chart->setAnimationOptions(QChart::NoAnimation);

        InteractiveChartView *chartView = new InteractiveChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setMinimumSize(100, 300);
        //chartView->setRubberBand(QChartView::RectangleRubberBand);
        return chartView;
    }
};

#endif // GRAPHWINDOW_H
