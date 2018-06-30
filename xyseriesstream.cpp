#include "xyseriesstream.h"

#include <QObject>
#include <QDebug>
#include <QtCharts/QXYSeries>
#include <QtCharts/QValueAxis>

XYSeriesStream::XYSeriesStream(QXYSeries *series, QObject *parent)
    : QObject(parent), m_data(QVector<QPointF>()),
      m_visualBuffer(QVector<QPointF>()), m_series(series), m_offset(0) {
}

void XYSeriesStream::setOffset(qint64 offset) { this->m_offset = offset; }

void XYSeriesStream::clear() {
  m_visualBuffer.clear();
  m_data.clear();
}

QVector<QPointF> *XYSeriesStream::getPayload() { return &m_data; }

void XYSeriesStream::appendPoint(qint64 x, float y) {
  static const int resolution = 4;

  m_data.append(QPointF(x, y));

  if (m_visualBuffer.isEmpty()) {
    m_visualBuffer.reserve(sampleCount);
    for (int i = 0; i < sampleCount; ++i) {
      m_visualBuffer.append(QPointF(this->m_offset, 1.0));
    }
  }

  qDebug() << x << "," << y;

  m_visualBuffer.push_front(QPointF(x - this->m_offset, y));
  m_visualBuffer.removeLast();
  m_series->replace(m_visualBuffer);
}
