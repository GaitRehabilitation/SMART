#include "metawearconfig.h"
#include "ui_metawearconfig.h"


MetawearConfig::MetawearConfig(QWidget *parent)
    :QDialog(parent),
     ui(new Ui::MetawearConfig)
{
    this->ui->setupUi(this);

    connect(this->ui->acceleromterSlider,&QSlider::sliderMoved,[this](int value){
       this->ui->sampleRateAccelerometer->setValue( this->getPosition(
                    this->ui->sampleRateAccelerometer->minimum(),
                    this->ui->sampleRateAccelerometer->maximum(),
                    value,
                    this->ui->acceleromterSlider->maximum()));
    });
    connect(this->ui->gyroscopeSlider,&QSlider::sliderMoved,[this](int value){
        this->ui->sampleRateGyroscope->setValue( this->getPosition(
                     this->ui->sampleRateGyroscope->minimum(),
                     this->ui->sampleRateGyroscope->maximum(),
                     value,
                     this->ui->gyroscopeSlider->maximum()));

    });
    connect(this->ui->ambientLightSlider,&QSlider::sliderMoved,[this](int value){
        this->ui->sampleeRateAmbientLight->setValue( this->getPosition(
                     this->ui->sampleeRateAmbientLight->minimum(),
                     this->ui->sampleeRateAmbientLight->maximum(),
                     value,
                     this->ui->ambientLightSlider->maximum()));
    });
    connect(this->ui->magnetometerSlider,&QSlider::sliderMoved,[this](int value){
        this->ui->sampleRateMagnetometer->setValue( this->getPosition(
                     this->ui->sampleRateMagnetometer->minimum(),
                     this->ui->sampleRateMagnetometer->maximum(),
                     value,
                     this->ui->magnetometerSlider->maximum()));
    });
    connect(this->ui->pressureSlider,&QSlider::sliderMoved,[this](int value){
        this->ui->sampleRatePressure->setValue( this->getPosition(
                     this->ui->sampleRatePressure->minimum(),
                     this->ui->sampleRatePressure->maximum(),
                     value,
                     this->ui->pressureSlider->maximum()));
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

double MetawearConfig::getPosition(double minimum, double maximum, int slidePosition, int max)
{
    double percent = ((double)slidePosition)/((double)max);
    return ((maximum - minimum) * percent) + minimum;
}

