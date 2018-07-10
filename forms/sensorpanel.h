#ifndef SENSORPANEL_H
#define SENSORPANEL_H

#include "metawearconfig.h"
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QMutex>
#include <QTemporaryDir>
#include <QWidget>
#include <qtimer.h>

class MetawearWrapper;
class MblMwMetaWearBoard;
class QTimer;
class DataBundle;

namespace Ui {
class SensorPanel;
}

class SensorPanel : public QWidget {

  Q_OBJECT
public:
  explicit SensorPanel(const QBluetoothDeviceInfo &device, QWidget *parent = 0);
  virtual ~SensorPanel();

private:
  MetawearWrapper *m_wrapper;
  Ui::SensorPanel *ui;
  QBluetoothDeviceInfo m_currentDevice;
  MetawearConfig *m_metawearConfig;
  qint64 m_plotoffset;
  qint64 m_laststEpoch;
  QTimer *settingUpdateTimer;
  QTemporaryDir* m_temporaryDir;
  QMutex m_plotLock;
  QMutex m_directoryLock;

  void registerPlotHandlers();
  void registerDataHandlers();
private slots:
  void metawareInitialized();
public slots:
  void setName(QString);
  void setOffset(qint64 offset);
  qint64 getOffset();
  qint64 getLatestEpoch();
  void startCapture(QTemporaryDir* dir);
  void stopCapture();
  void clearPlots();
signals:
  void onConnected();
  void onDisconnect();
  void onBluetoothError(QLowEnergyController::Error e);

  double onBatteryPercentage(qint8);
  double onVoltage(quint16);

  void setDir();
  void onMagnetometer(qint64, float, float, float);
  void onGyro(qint64, float, float, float);
  void onAcceleration(qint64, float, float, float);
  void onAmbientLight(qint64, qint32);
  void onMetawearInitilized();
  void onLatestEpoch(qint64);

  void onEpoch(qint64);
};

#endif // SENSORPANEL_H
