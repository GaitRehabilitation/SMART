#include "sessionentry.h"
#include "ui_sessionentry.h"

SessionEntry::SessionEntry(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SessionEntry)
{
    ui->setupUi(this);
}

SessionEntry::~SessionEntry()
{
    delete ui;
}
