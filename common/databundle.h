#ifndef DATABUNDLE_H
#define DATABUNDLE_H

#include <QObject>
#include <QList>

class DataSink;

class DataBundle : public QObject
{
    Q_OBJECT
private:
    QList<DataSink*> m_dataSink;
    QString m_title;
    QString m_dir;
public:
    DataBundle(QObject* parent = 0);
    virtual ~DataBundle();
    void pushSink(DataSink* sink);
public slots:
    QString getDir();
    QString getTitle();
    void setDir(QString dir);
    void setTitle(QString entry);
    void startCapture();
    void stopCapture();
signals:
    void onDirSet(QString);
    void onTitleSet(QString);
    void onStartCapture();
    void onStopCapture();
};

#endif // DATABUNDLE_H
