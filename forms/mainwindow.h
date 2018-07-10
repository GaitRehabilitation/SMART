#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QBluetoothDeviceInfo>
#include <QMainWindow>
#include <QTemporaryDir>
#include <QTimer>

namespace Ui {
class MainWindow;
}
class SensorPanel;
QT_FORWARD_DECLARE_CLASS(QFileDialog)

class MainWindow : public QMainWindow {
  Q_OBJECT
private:
    QTemporaryDir* m_temporaryData;
    QTimer m_triggerSingleShot;
    QTimer m_updateTriggerTimer;
    float m_triggerTime;
public:
  MainWindow(QWidget *parent = 0);
  virtual ~MainWindow();
public slots:
  void registerDevice(const QBluetoothDeviceInfo &info);
  void startCapture();
  void stopCapture();
private slots:
  void deviceAddWizard();

private:
  Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
