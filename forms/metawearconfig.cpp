#include "metawearconfig.h"
#include "ui_metawearconfig.h"

MetawearConfig::MetawearConfig(QWidget *parent)
    : QDialog(parent), ui(new Ui::MetawearConfig) {
  this->ui->setupUi(this);

  connect(
      this->ui->acceleromterSlider, &QSlider::sliderMoved, [this](int value) {
        QDoubleSpinBox *spinBox = this->ui->sampleRateAccelerometer;
        QSlider *slider = this->ui->acceleromterSlider;

        double percent = ((double)value) / ((double)slider->maximum());
        double result = ((spinBox->maximum() - spinBox->minimum()) * percent) +
                        spinBox->minimum();
        spinBox->setValue(result);
      });

  connect(this->ui->enableAccelerometer, &QCheckBox::stateChanged,
          [this](int state) {
            this->ui->sampleRateAccelerometer->setEnabled(
                (state == Qt::Checked));
          });
}

MetawearConfig::~MetawearConfig() { delete this->ui; }

double MetawearConfig::getAcceleromterSampleRate() {
  return this->ui->sampleRateAccelerometer->value();
}

bool MetawearConfig::isMagnetometerActive() {
  return this->ui->enableMagnetometer->isChecked();
}

bool MetawearConfig::isGyroscopeActive() {
  return this->ui->enableGyroscope->isChecked();
}

bool MetawearConfig::isAcceleromterActive() {
  return this->ui->enableAccelerometer->isChecked();
}

bool MetawearConfig::isAmbientLightActive() {
  return this->ui->enableAmbientLight->isChecked();
}

bool MetawearConfig::isPressureActive() {
  return this->ui->enablePressure->isChecked();
}

void MetawearConfig::setAcceleromterSampleRate(double value) {
  QDoubleSpinBox *spintBox = this->ui->sampleRateAccelerometer;
  QSlider *slider = this->ui->acceleromterSlider;

  double factor = (spintBox->value() - spintBox->minimum()) /
                  (spintBox->maximum() - spintBox->minimum());
  int pos = (int)(((double)slider->maximum()) * factor);

  spintBox->setValue(value);
  slider->setValue(pos);
}
