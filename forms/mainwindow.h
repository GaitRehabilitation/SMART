#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

QT_FORWARD_DECLARE_CLASS(DeviceWizard)

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();
private slots:
    void deviceAddWizard();
private:
    Ui::MainWindow* ui;
    DeviceWizard* deviceWizard;
};

#endif // MAINWINDOW_H
