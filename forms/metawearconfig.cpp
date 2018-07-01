#include "metawearconfig.h"
#include "ui_metawearconfig.h"


MetawearConfig::MetawearConfig(QWidget *parent)
    :QDialog(parent),
     ui(new Ui::MetawearConfig)
{
    this->ui->setupUi(this);

    connect(this->ui->acceleromterSlider,&QSlider::sliderMoved,[this](int value){
        QDoubleSpinBox* spinBox =  this->ui->sampleRateAccelerometer;
        QSlider* slider = this->ui->acceleromterSlider;

        double percent = ((double)value)/((double)slider->maximum());
        double result =  ((spinBox->maximum() - spinBox->minimum()) * percent) + spinBox->minimum();
        spinBox->setValue(result);
    });
    connect(this->ui->gyroscopeSlider,&QSlider::sliderMoved,[this](int value){
        QDoubleSpinBox* spinBox =  this->ui->sampleRateGyroscope;
        QSlider* slider = this->ui->gyroscopeSlider;

        double percent = ((double)value)/((double)slider->maximum());
        double result =  ((spinBox->maximum() - spinBox->minimum()) * percent) + spinBox->minimum();
        spinBox->setValue(result);
    });
    connect(this->ui->ambientLightSlider,&QSlider::sliderMoved,[this](int value){
        QDoubleSpinBox* spinBox =  this->ui->sampleeRateAmbientLight;
        QSlider* slider = this->ui->ambientLightSlider;

        double percent = ((double)value)/((double)slider->maximum());
        double result =  ((spinBox->maximum() - spinBox->minimum()) * percent) + spinBox->minimum();
        spinBox->setValue(result);
    });
    connect(this->ui->magnetometerSlider,&QSlider::sliderMoved,[this](int value){
        QDoubleSpinBox* spinBox =  this->ui->sampleRateMagnetometer;
        QSlider* slider = this->ui->magnetometerSlider;

        double percent = ((double)value)/((double)slider->maximum());
        double result =  ((spinBox->maximum() - spinBox->minimum()) * percent) + spinBox->minimum();
        spinBox->setValue(result);
    });
    connect(this->ui->pressureSlider,&QSlider::sliderMoved,[this](int value){
        QDoubleSpinBox* spinBox =  this->ui->sampleRatePressure;
        QSlider* slider = this->ui->pressureSlider;

        double percent = ((double)value)/((double)slider->maximum());
        double result =  ((spinBox->maximum() - spinBox->minimum()) * percent) + spinBox->minimum();
        spinBox->setValue(result);
    });

    connect(this->ui->enableAccelerometer,&QCheckBox::stateChanged,[this](int state){
            this->ui->pressureSlider->setEnabled((state == Qt::Checked));
            this->ui->sampleRateAccelerometer->setEnabled((state == Qt::Checked));
    });

    connect(this->ui->enableGyroscope,&QCheckBox::stateChanged,[this](int state){
            this->ui->gyroscopeSlider->setEnabled((state == Qt::Checked));
            this->ui->sampleRateGyroscope->setEnabled((state == Qt::Checked));
    });

    connect(this->ui->enableAmbientLight,&QCheckBox::stateChanged,[this](int state){
            this->ui->ambientLightSlider->setEnabled((state == Qt::Checked));
            this->ui->sampleeRateAmbientLight->setEnabled((state == Qt::Checked));
    });

    connect(this->ui->enableMagnetometer,&QCheckBox::stateChanged,[this](int state){
            this->ui->sampleRateMagnetometer->setEnabled((state == Qt::Checked));
            this->ui->magnetometerSlider->setEnabled((state == Qt::Checked));
    });

    connect(this->ui->enablePressure,&QCheckBox::stateChanged,[this](int state){
            this->ui->pressureSlider->setEnabled((state == Qt::Checked));
            this->ui->sampleRateAccelerometer->setEnabled((state == Qt::Checked));
    });
}

MetawearConfig::~MetawearConfig()
{
    delete this->ui;
}

double MetawearConfig::getMagnetometereSampleRate()
{
    return this->ui->sampleRateMagnetometer->value();
}

double MetawearConfig::getGyroscopeSampleRate()
{
    return this->ui->sampleRateGyroscope->value();
}

double MetawearConfig::getAcceleromterSampleRate()
{
    return this->ui->sampleRateAccelerometer->value();
}

double MetawearConfig::getAmbientLightSampleRate()
{
    return this->ui->sampleeRateAmbientLight->value();
}

double MetawearConfig::getPressureSampleRate()
{
    return this->ui->sampleRatePressure->value();
}

bool MetawearConfig::isMagnetometerActive()
{
    return this->ui->enableMagnetometer;
}

bool MetawearConfig::isGyroscopeActive()
{
    return this->ui->enableGyroscope;
}

bool MetawearConfig::isAcceleromterActive()
{
    return this->ui->enableAccelerometer;
}

bool MetawearConfig::isAmbientLightActive()
{
    return this->ui->enableAmbientLight;
}

bool MetawearConfig::isPressureActive()
{
    return this->ui->enablePressure;
}

void MetawearConfig::setMagnetometereSampleRate(double value)
{
    QDoubleSpinBox* spintBox = this->ui->sampleRateMagnetometer;
    QSlider* slider = this->ui->magnetometerSlider;

    double factor = (spintBox->value() -  spintBox->minimum())/(spintBox->maximum() - spintBox->minimum());
    int pos = (int)(((double)slider->maximum()) * factor);
    spintBox->setValue(value);
    slider->setValue(pos);
}

void MetawearConfig::setGyroscopeSampleRate(double value)
{
    QDoubleSpinBox* spintBox = this->ui->sampleRateGyroscope;
    QSlider* slider =  this->ui->gyroscopeSlider;

    double factor = (spintBox->value() -  spintBox->minimum())/(spintBox->maximum() - spintBox->minimum());
    int pos = (int)(((double)slider->maximum()) * factor);
    spintBox->setValue(value);
    slider->setValue(pos);
}

void MetawearConfig::setAcceleromterSampleRate(double value)
{
    QDoubleSpinBox* spintBox = this->ui->sampleRateAccelerometer;
    QSlider* slider =  this->ui->acceleromterSlider;

    double factor = (spintBox->value() -  spintBox->minimum())/(spintBox->maximum() - spintBox->minimum());
    int pos = (int)(((double)slider->maximum()) * factor);

    spintBox->setValue(value);
    slider->setValue(pos);

}

void MetawearConfig::setAmbientLightSampleRate(double value)
{
    QDoubleSpinBox* spintBox = this->ui->sampleeRateAmbientLight;
    QSlider* slider =  this->ui->ambientLightSlider;

    double factor = (spintBox->value() -  spintBox->minimum())/(spintBox->maximum() - spintBox->minimum());
    int pos = (int)(((double)slider->maximum()) * factor);
    spintBox->setValue(value);
    slider->setValue(pos);
}

void MetawearConfig::setPressureSampleRate(double value)
{
    QDoubleSpinBox* spintBox = this->ui->sampleRateMagnetometer;
    QSlider* slider =  this->ui->magnetometerSlider;

    double factor = (spintBox->value() -  spintBox->minimum())/(spintBox->maximum() - spintBox->minimum());
    int pos = (int)(((double)slider->maximum()) * factor);
    spintBox->setValue(value);
    slider->setValue(pos);
}

