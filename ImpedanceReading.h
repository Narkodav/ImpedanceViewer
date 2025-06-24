#ifndef IMPEDANCEREADING_H
#define IMPEDANCEREADING_H

#include <QVector>
#include <QtCharts/QtCharts>
#include <QtCharts/QLineSeries>

class ImpedanceReading
{
    QLineSeries* magnitudes;
    QLineSeries* phases;
public:

    ImpedanceReading() : magnitudes(new QLineSeries()), phases(new QLineSeries()) {
        magnitudes->setName("Magnitude");
        phases->setName("Phase");
    };

    QLineSeries* getMagnitudeSeries() const { return magnitudes; }
    QLineSeries* getPhaseSeries() const { return phases; }

    static bool getReading(ImpedanceReading& reading, QString filePath) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);

            // Regular expression to split on spaces, tabs, or commas
            QRegularExpression re("(\\s+|,(?=\\s)|\\t)");

            //skip first line if necessary
            if (!stream.atEnd())
            {
                QString line = stream.readLine().trimmed();
                if (!line.contains("freq", Qt::CaseInsensitive) &&
                    !line.contains("magnitude", Qt::CaseInsensitive) &&
                    !line.contains("phase", Qt::CaseInsensitive))
                    parseLine(line, re, reading);
            }

            while (!stream.atEnd()) {
                QString line = stream.readLine().trimmed();
                parseLine(line, re, reading);
            }
            file.close();
        }
        if(reading.getMagnitudeSeries()->count() == 0 || reading.getPhaseSeries()->count() == 0 ||
            reading.getMagnitudeSeries()->count() != reading.getPhaseSeries()->count())
            return false;
        return true;
    }

private:

    static inline void parseLine(QString& line, QRegularExpression& re, ImpedanceReading& reading)
    {
        // Skip empty lines and comment lines (starting with #)
        if (line.isEmpty() || line.startsWith("#")) {
            return;
        }

        // Split line into parts
        QStringList parts = line.split(re, Qt::SkipEmptyParts);

        // We need exactly 3 columns: frequency, magnitude, phase
        if (parts.size() >= 3) {
            bool ok1, ok2, ok3;
            parts[0].replace(',', '.');
            double frequency = parts[0].toDouble(&ok1);
            parts[1].replace(',', '.');
            double magnitude = parts[1].toDouble(&ok2);
            parts[2].replace(',', '.');
            double phase = parts[2].toDouble(&ok3);

            if (ok1 && ok2 && ok3) {
                reading.magnitudes->append(frequency, magnitude);
                reading.phases->append(frequency, phase);
            }
        }
    }
};

#endif // IMPEDANCEREADING_H
