#ifndef INTERACTIVECHARTVIEW_H
#define INTERACTIVECHARTVIEW_H

#include <QMainWindow>
#include <QtWidgets/QApplication>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QDialog>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>

QT_USE_NAMESPACE

class InteractiveChartView : public QChartView
{
    Q_OBJECT
private:
    QGraphicsDropShadowEffect *shadowEffect;
    QPropertyAnimation *hoverAnimation;

public:
    InteractiveChartView(QChart *chart, QWidget *parent = nullptr)
        : QChartView(chart, parent)
    {
        // Add a subtle shadow effect that we'll animate on hover
        shadowEffect = new QGraphicsDropShadowEffect(this);
        shadowEffect->setBlurRadius(0);  // Start with no shadow
        shadowEffect->setColor(Qt::cyan);
        shadowEffect->setOffset(0);
        this->setGraphicsEffect(shadowEffect);

        // Set up hover animation
        hoverAnimation = new QPropertyAnimation(shadowEffect, "blurRadius");
        hoverAnimation->setDuration(150); // milliseconds
    }

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton) {
            emit chartDoubleClicked(chart());
        }
        QChartView::mouseDoubleClickEvent(event);
    }

    void enterEvent(QEnterEvent *event) override
    {
        // Animate glow effect on hover
        hoverAnimation->stop();
        hoverAnimation->setStartValue(shadowEffect->blurRadius());
        hoverAnimation->setEndValue(15); // Glow intensity
        hoverAnimation->start();

        setCursor(Qt::PointingHandCursor); // Change cursor to indicate interactivity
        QChartView::enterEvent(event);
    }

    void leaveEvent(QEvent *event) override
    {
        // Animate glow effect removal
        hoverAnimation->stop();
        hoverAnimation->setStartValue(shadowEffect->blurRadius());
        hoverAnimation->setEndValue(0);
        hoverAnimation->start();

        unsetCursor(); // Revert cursor
        QChartView::leaveEvent(event);
    }

signals:
    void chartDoubleClicked(QChart *chart);
};

class ChartPopup : public QDialog
{
public:
    ChartPopup(QChart *chart, QWidget *parent = nullptr)
        : QDialog(parent)
    {
        setWindowTitle("Enlarged Chart View");
        setMinimumSize(800, 600);

        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setRubberBand(QChartView::RectangleRubberBand);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(chartView);

        // Add close button
        QPushButton *closeButton = new QPushButton("Close", this);
        connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
        layout->addWidget(closeButton);
    }
};

#endif // INTERACTIVECHARTVIEW_H
