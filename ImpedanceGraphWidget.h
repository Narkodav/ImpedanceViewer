#ifndef IMPEDANCEGRAPHWIDGET_H
#define IMPEDANCEGRAPHWIDGET_H

#include "SelectableChartView.h"
#include "ImpedanceReading.h"

#include <QListWidget>
#include <QtWidgets/QApplication>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QFormLayout>

QT_USE_NAMESPACE

    class ImpedanceGraphWidget : public QWidget
{
    Q_OBJECT

    using RowChartPair = std::pair<SelectableChartView*, SelectableChartView*>;
    using RowSeriesPair = std::pair<QLineSeries*, QLineSeries*>;

private:
    QVBoxLayout *mainLayout;
    QListWidget *listWidget;
    QPushButton *addButton;

    QVector<RowSeriesPair> charts;

public:
    ImpedanceGraphWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        // Main layout
        mainLayout = new QVBoxLayout(this);

        // Create list widget with drag and drop enabled
        listWidget = new QListWidget();
        listWidget->setDragDropMode(QAbstractItemView::DragDropMode::InternalMove);
        listWidget->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        listWidget->setStyleSheet("QListWidget { border: none; }");
        listWidget->setSpacing(10);
        mainLayout->addWidget(listWidget);
    }

    const QListWidget* getList() const { return listWidget; };

    void addReading(RowChartPair&& pair, const QString &title1, const QString &title2)
    {
        for(QAbstractSeries* series1 : pair.first->chart()->series())
            for(QAbstractSeries* series2 : pair.second->chart()->series())
                charts.push_back(std::make_pair(
                    qobject_cast<QLineSeries*>(series1),
                    qobject_cast<QLineSeries*>(series2)));

        QWidget *container = new QWidget();
        QHBoxLayout *hLayout = new QHBoxLayout(container);
        hLayout->setContentsMargins(0, 0, 0, 0);
        hLayout->setSpacing(10);

        hLayout->addWidget(pair.first);
        hLayout->addWidget(pair.second);

        QPushButton *removeButton = new QPushButton("Remove");
        hLayout->addWidget(removeButton);

        QListWidgetItem *item = new QListWidgetItem();
        item->setSizeHint(container->sizeHint());
        listWidget->addItem(item);
        listWidget->setItemWidget(item, container);

        connect(pair.first, &SelectableChartView::chartDoubleClicked,
                [this](QChart *clickedChart) { showChartPopup(clickedChart); });

        connect(pair.second, &SelectableChartView::chartDoubleClicked,
                [this](QChart *clickedChart) { showChartPopup(clickedChart); });

        connect(removeButton, &QPushButton::clicked,
                [this, item]() {
                    size_t row = listWidget->row(item);
                    charts.removeAt(row);
                    delete listWidget->takeItem(row);
                    emit chartRemoved(row);
                });
    }

    void addReading(ImpedanceReading&& reading, const QString &title1, const QString &title2)
    {
        auto viewOne = makeChartView(reading.getMagnitudeSeries(), title1);
        auto viewTwo = makeChartView(reading.getPhaseSeries(), title2);
        auto pair = std::make_pair(viewOne, viewTwo);
        addReading(std::move(pair), "1", "2");
    }

    const RowSeriesPair& getChartsAt(size_t row)
    {
        return charts[row];
    }

private:
    void showChartPopup(QChart *clickedChart)
    {
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
    }

    SelectableChartView* makeChartView(QLineSeries* series, const QString &title)
    {
        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle(title);
        chart->createDefaultAxes();
        chart->legend()->setVisible(false);
        chart->setAnimationOptions(QChart::NoAnimation);

        SelectableChartView *chartView = new SelectableChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setMinimumSize(100, 300);
        return chartView;
    }

protected:
signals:
    void chartRemoved(size_t deletedRow);
};

#endif // IMPEDANCEGRAPHWIDGET_H
