#ifndef XYSERIESSTREAM_H
#define XYSERIESSTREAM_H

#include <QObject>
#include <QtCore/QIODevice>
#include <QtCore/QPointF>
#include <QtCore/QVector>
#include <QtCharts/QChartGlobal>

QT_CHARTS_BEGIN_NAMESPACE
class QSplineSeries;
class QValueAxis;
class QXYSeries;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class XYSeriesStream : public QObject
{
    Q_OBJECT
public:
    static const int sampleCount = 2000;

    XYSeriesStream(QXYSeries* series, QObject* parent = nullptr);
    void appendPoint(qint64 x, float y);
    void setOffset(qint64 offset);
public slots:
    void clear();
    QVector<QPointF>* getPayload();


private:
    QXYSeries *m_series;
    QVector<QPointF> m_visualBuffer;
    QVector<QPointF> m_data;
    int64_t m_offset;

};

#endif // XYSERIESSTREAM_H
