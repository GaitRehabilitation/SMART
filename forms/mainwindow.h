#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QBluetoothAddress>
#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

QT_FORWARD_DECLARE_CLASS(DeviceWizard)
QT_FORWARD_DECLARE_CLASS(QFileDialog)

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();
public slots:
    void registerDevice(QBluetoothAddress address);
private slots:
    void deviceAddWizard();    

private:
    Ui::MainWindow* ui;
};

#endif // MAINWINDOW_H
