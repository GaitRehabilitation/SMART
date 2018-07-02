#ifndef METAWEARCONFIG_H
#define METAWEARCONFIG_H

#include <QDialog>

namespace Ui {
class MetawearConfig;
}

class MetawearConfig : public QDialog {
  Q_OBJECT
public:
  explicit MetawearConfig(QWidget *parent = 0);
  ~MetawearConfig();

  double getAcceleromterSampleRate();

  bool isMagnetometerActive();
  bool isGyroscopeActive();
  bool isAcceleromterActive();
  bool isAmbientLightActive();
  bool isPressureActive();

public slots:
  void setAcceleromterSampleRate(double);
signals:

private:
  Ui::MetawearConfig *ui;
};

#endif // METAWEARCONFIG_H
