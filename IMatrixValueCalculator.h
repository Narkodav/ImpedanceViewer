#ifndef IMATRIXVALUECALCULATOR_H
#define IMATRIXVALUECALCULATOR_H

#include <vector>
#include <stdlib.h>
#include <QtCharts/QLineSeries>>

struct MatrixValues
{
private:
    std::vector<double> data;
    size_t rowAmount, colAmount;
public:

    void resize(size_t newRowAmount, size_t newColAmount) {
        rowAmount = newRowAmount;
        colAmount = newColAmount;
        data.resize(rowAmount * colAmount);
    }

    double& operator()(size_t row, size_t col) { return data[row * colAmount + col]; };
    const double& operator()(size_t row, size_t col) const { return data[row * colAmount + col]; };
};

class IMatrixValueCalculator
{
public:
    IMatrixValueCalculator() {};
    virtual ~IMatrixValueCalculator() {};

    virtual double getValue(QLineSeries* reading1, QLineSeries* reading2) = 0;
};

#endif // IMATRIXVALUECALCULATOR_H
