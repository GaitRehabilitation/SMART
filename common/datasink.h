#ifndef DATASINK_H
#define DATASINK_H

#include <QFile>
#include <QObject>


class DataSink : public QObject
{
    Q_OBJECT
private:
    QString m_title;
    QString m_sinkTitle;
    QString m_dir;
    int m_numberOfColumns;
    QStringList m_columns;
    QFile file;
    bool isWriting;
    QFile m_outputFile;
public:
    DataSink(const QString sinkTitle,const QStringList& headers,QObject* parent = 0);
    void pushEntry(QList<QVariant>&);
    bool canWrite();
public slots:
    void setDir(QString);
    void setTitle(QString);
    void startCapture();
    void stopCapture();

};

#endif // DATASINK_H
