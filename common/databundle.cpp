#include "databundle.h"

#include "datasink.h"

#include <QTemporaryDir>

DataBundle::DataBundle(QObject *parent):
    QObject(parent),m_title(""),m_dir(""),m_dataSink()
{

}

DataBundle::~DataBundle()
{

}

void DataBundle::pushSink(DataSink *sink)
{
    m_dataSink.push_back(sink);
    sink->setParent(this);
    connect(this,SIGNAL(onDirSet(QString)),sink,SLOT(setDir(QString)));
    connect(this,SIGNAL(onTitleSet(QString)),sink,SLOT(setTitle(QString)));
    connect(this,SIGNAL(onStartCapture()),sink,SLOT(startCapture()));
    connect(this,SIGNAL(onStopCapture()),sink,SLOT(stopCapture()));
    setDir(m_dir);
    setTitle(m_title);
}

QString DataBundle::getDir()
{
    return m_dir;
}

QString DataBundle::getTitle()
{
    return m_title;
}

void DataBundle::setDir(QString dir)
{
    m_dir = dir;
    emit onDirSet(dir);
}

void DataBundle::setTitle(QString entry)
{
    m_title = entry;
    emit onTitleSet(entry);
}

void DataBundle::startCapture()
{
    emit onStartCapture();
}

void DataBundle::stopCapture()
{
    emit onStopCapture();
}
