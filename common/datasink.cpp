#include "common/datasink.h"

#include <QVariant>
#include <QStandardPaths>
#include <QTextStream>


DataSink::DataSink(const QString sinkTitle, const QStringList &headers, QObject *parent)
    :QObject(parent),m_sinkTitle(sinkTitle),m_columns(headers)
{

}

void DataSink::pushEntry(QList<QVariant>& data)
{
    if(this->canWrite()){
        QStringList result;
        for(int x = 0; x < data.length(); ++x){
            result.append(data.at(x).toString());
        }
        QString output = result.join(', ');
        QString path =  QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        path.append(QString("/%1_%2.csv").arg(m_title,m_sinkTitle));
        QFile file(path);
        bool exist = file.exists();
        if(file.open(QIODevice::WriteOnly | QIODevice::Append)){
            QTextStream outStream(&file);
            if(!exist){
                outStream << m_columns.join(', ') << '\n';
            }
            outStream << output << '\n';
        }
        file.close();
    }

}

bool DataSink::canWrite()
{
    return this->isWriting && !this->m_dir.isEmpty();
}

void DataSink::setDir(QString directory)
{
    m_dir = directory;
}

void DataSink::setTitle(QString title)
{
    m_title = title;
}


void DataSink::startCapture()
{
    if(!this->m_dir.isEmpty()){
        this->isWriting = true;
    }
}

void DataSink::stopCapture()
{
    this->isWriting = false;
    setDir("");
}
