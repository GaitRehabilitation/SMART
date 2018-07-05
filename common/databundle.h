#ifndef DATABUNDLE_H
#define DATABUNDLE_H

#include <QObject>
#include "datasink.h"



class DataBundle : public QObject
{
private:
    QList<DataSink> m_dataSink;
public:
    DataBundle(QObject* parent = 0);

    void pushSink();
public slots:
    void setTitle(QString entry);
    void setPath(QString path);
    void startCapture();
    void stopCapture();
signals:
    void onPathSet(QString);
    void onStartCapture();
    void onStopCapture();
};

#endif // DATABUNDLE_H
