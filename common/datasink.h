#ifndef DATASINK_H
#define DATASINK_H

#include <QFile>
#include <QObject>


class DataSink : public QObject
{
private:
    QString m_path;
    int m_numberOfColumns;
    QStringList m_columns;
    QFile file;
    bool isWriting;
public:
    DataSink(const QStringList& headers,QObject* parent = 0);
    void pushEntry(QList<QVariant>&);
    bool canWrite();
public slots:
    void setPath(QString& path);
    void startCapture();
    void stopCapture();

};

#endif // DATASINK_H
