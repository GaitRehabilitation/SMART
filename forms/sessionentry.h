#ifndef SESSIONENTRY_H
#define SESSIONENTRY_H

#include <QWidget>

namespace Ui {
class SessionEntry;
}

class SessionEntry : public QWidget
{
    Q_OBJECT

public:
    explicit SessionEntry(QWidget *parent = 0);
    ~SessionEntry();

private:
    Ui::SessionEntry *ui;
};

#endif // SESSIONENTRY_H
