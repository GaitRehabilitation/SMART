#include "datasink.h"

#include <QVariant>

DataSink::DataSink(const QStringList& headers, QObject* parent ):
    QObject(parent),m_columns(headers),m_path("")
{

}

void DataSink::pushEntry(QList<QVariant>& data)
{
    if(this->canWrite()){
        QStringList result;
        for(int x = 0; x < data.length(); ++x){
            result.append(data.at(x).toString());
        }
        QString final = result.join(', ');

    }
}

bool DataSink::canWrite()
{
    return this->isWriting && !this->m_path.isEmpty();
}

void DataSink::setPath(QString& path)
{
    this->m_path = QString(path);
}

void DataSink::startCapture()
{
    if(!this->m_path.isEmpty()){
        this->isWriting = true;
    }
}

void DataSink::stopCapture()
{
    this->isWriting = false;
    this->m_path = QString("");
}
