#ifndef MATRIXVALUECALCULATORS_H
#define MATRIXVALUECALCULATORS_H

#include "IMatrixValueCalculator.h"

#include <QFile>
#include <QTextStream>
#include <QJSEngine>
#include <QJSValue>
#include <QDebug>

#include <string>

//assumes deltas of both readings are the same
class MatrixValueCalculatorDefault : public IMatrixValueCalculator
{
public:
    MatrixValueCalculatorDefault() : IMatrixValueCalculator() {};

    double getValue(QLineSeries* reading1, QLineSeries* reading2) override {
        if(reading1->count() < reading2->count())
            return getValue(reading2, reading1);

        double K = 0;

        for(size_t i = 0; i < reading2->count(); i++)
            K += abs(reading1->at(i).y() - reading2->at(i).y());
        return K;
    };
};

//uses an external script
class MatrixValueCalculatorExternalScript : public IMatrixValueCalculator
{
    QString m_filepath;
    QJSEngine m_engine;
    QJSValue m_scriptFunction;

public:
    MatrixValueCalculatorExternalScript(const QString& filepath)
        : m_filepath(filepath), IMatrixValueCalculator()
    {
        // Load and compile the script
        QFile scriptFile(m_filepath);
        if (!scriptFile.open(QIODevice::ReadOnly)) {
            throw std::runtime_error(std::string("Failed to open script file: ") + m_filepath.toStdString());
        }

        QTextStream stream(&scriptFile);
        QString scriptContents = stream.readAll();
        scriptFile.close();

        // Evaluate the script
        QJSValue evalResult = m_engine.evaluate(scriptContents, m_filepath);
        if (evalResult.isError()) {
            throw std::runtime_error(std::string("Script evaluation error:") + evalResult.toString().toStdString());
        }

        // Get the calculate function (assuming it exists)
        m_scriptFunction = m_engine.globalObject().property("calculate");
        if (!m_scriptFunction.isCallable()) {
            throw std::runtime_error("Script must implement a calculate() function");
        }
    }

    double getValue(QLineSeries* reading1, QLineSeries* reading2) override
    {
        if (!m_scriptFunction.isCallable()) {
            throw std::runtime_error("Script function not callable");
        }

        // Convert series data to JS arrays
        QJSValue jsReading1 = convertSeriesToJS(reading1);
        QJSValue jsReading2 = convertSeriesToJS(reading2);

        // Verify the conversion worked
        if (jsReading1.isError() || jsReading2.isError()) {
            throw std::runtime_error("Failed to convert series data to JS");
        }

        // Call the script function
        QJSValue result = m_scriptFunction.call(QJSValueList() << jsReading1 << jsReading2);

        if (result.isError()) {
            throw std::runtime_error(std::string("Script evaluation error:") + result.toString().toStdString());
        }

        if (!result.isNumber()) {
            throw std::runtime_error("Script function not callable");
        }

        return result.toNumber();
    }

private:
    QJSValue convertSeriesToJS(QLineSeries* series)
    {
        const auto& points = series->points();
        QVariantList jsPoints;

        for (const QPointF& point : points) {
            QVariantMap jsPoint;
            jsPoint["x"] = point.x();
            jsPoint["y"] = point.y();
            jsPoints.append(jsPoint);
        }

        return m_engine.toScriptValue(jsPoints);
    }
};

#endif // MATRIXVALUECALCULATORS_H
