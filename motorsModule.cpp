#include "motorsModule.h"
#include "motionController.h"
#include <QTime>
#include <QDebug>

motorsModule::motorsModule(QObject *root, QObject *parent) :
    QObject(parent)
{
    m_pRootItem = root;
    m_motions<<motion()<<motion()<<motion();

    connect(&controller, SIGNAL(validateMotorFinished(QByteArray)), this, SLOT(validateMotorFinished(QByteArray)));
    connect(&controller, SIGNAL(validateMotorsFinished(QByteArray)), this, SLOT(validateMotorsFinished(QByteArray)));
    connect(&controller, SIGNAL(settingStartPointFinished(QByteArray)), this, SLOT(validateProgram()));
    connect(&controller, SIGNAL(settingEndPointFinished(QByteArray)), this, SLOT(validateProgram()));
    connect(m_pRootItem, SIGNAL(validateRequest()), this, SLOT(validateProgram()));
    connect(m_pRootItem, SIGNAL(backlash(int,int)), this, SLOT(backlash(int,int)));
    connect(m_pRootItem, SIGNAL(powerSaveMode(int,bool)), this, SLOT(powerSaveMode(int,bool)));
    connect(m_pRootItem, SIGNAL(invertDirection(int,bool)), this, SLOT(invertDirection(int,bool)));
    connect(m_pRootItem, SIGNAL(stepResolution(int,int)), this, SLOT(stepResolution(int,int)));
    connect(m_pRootItem, SIGNAL(maxStepRate(int,int)), this, SLOT(maxStepRate(int,int)));
    connect(m_pRootItem, SIGNAL(easingType(int,int)), this, SLOT(easingType(int,int)));
    connect(m_pRootItem, SIGNAL(clearClicked(int)), this, SLOT(clearClicked(int)));
    connect(m_pRootItem, SIGNAL(motionChanged(int,qreal,qreal,qreal,qreal)), this,
            SLOT(motionChanged(int,qreal,qreal,qreal,qreal)));
}

void motorsModule::validateProgram(int motor) {
    unsigned programMode = m_pRootItem->property("programMode").toInt();
    unsigned interval = m_pRootItem->property("interval").toReal() * 1000.0;
    unsigned focusTime = m_pRootItem->property("focusTime").toReal() * 1000.0;
    unsigned triggerTime = m_pRootItem->property("triggerTime").toReal() * 1000.0;
    unsigned exposureDelay = m_pRootItem->property("exposureDelay").toReal() * 1000.0;
    float length;

    controller.setProgramMode((unsigned char)programMode);
    if(programMode != 2) { //not video mode
        controller.setShotsInterval(interval);
        controller.setFocusTime(focusTime);
        controller.setExposureTime(triggerTime);
        controller.setExposureDelay(exposureDelay);

        // SMS mode
        if(programMode == 0) {
            length = m_pRootItem->property("videoFrames").toInt();
        }
        // Continuous TL mode
        else {
            int shootingHours = m_pRootItem->property("shootingHours").toInt();
            int shootingMinutes = m_pRootItem->property("shootingMinutes").toInt();
            int shootingSecs = m_pRootItem->property("shootingSecs").toInt();
            length = shootingHours * 3600 + shootingMinutes * 60 + shootingSecs;
            length *= 1000;
        }
    }
    // Continuous video mode
    else {
        float videoLengthSeconds = m_pRootItem->property("videoLengthSeconds").toFloat();
        int videoLengthMinutes = m_pRootItem->property("videoLengthMinutes").toInt();

        length = videoLengthMinutes * 60 + (int)videoLengthSeconds;
        length += videoLengthSeconds - (int)videoLengthSeconds;

        length *= 1000; //to msecs
    }

    // Changing graph points
    if(motor != -1) {
        motion m = m_motions[motor - 1];
        controller.setLeadInShots((unsigned char)motor, unsigned(qRound(length * m.leadIn)));
        controller.setLeadOutShots((unsigned char)motor, unsigned(qRound(length * m.leadOut)));
        controller.setProgramAcceleration((unsigned char)motor, unsigned(qRound(length * m.acceleration)));
        controller.setProgramDeceleration((unsigned char)motor, unsigned(qRound(length * m.deceleration)));
        controller.setTravelTime((unsigned char)motor, unsigned(qRound(length * m.travelTime)));

        m_validatingMotor = (unsigned char)motor;
        controller.validateMotor((unsigned char)motor);
    }
    // Changing input other than graph points
    else {
        for(int i = 1; i <= 3; ++i) {
            motion m = m_motions[i - 1];            

            controller.setLeadInShots((unsigned char)i, unsigned(qRound(length * m.leadIn)));
            controller.setLeadOutShots((unsigned char)i, unsigned(qRound(length * m.leadOut)));
            controller.setProgramAcceleration((unsigned char)i, unsigned(qRound(length * m.acceleration)));
            controller.setProgramDeceleration((unsigned char)i, unsigned(qRound(length * m.deceleration)));
            controller.setTravelTime((unsigned char)i, unsigned(qRound(length * m.travelTime)));
        }

        controller.validateMotors();
    }
}

void motorsModule::powerSaveMode(int motor, bool enable) {
    controller.setMotorSleep((unsigned char)motor, enable);
}

void motorsModule::invertDirection(int motor, bool enable) {
    qDebug()<<"invert direction:"<<motor<<enable;
    controller.invertMotor((unsigned char)motor, enable);
}

void motorsModule::stepResolution(int motor, int resolution) {
    static unsigned char resolutions[3] = {4, 8, 16};
    controller.setMicroStepValue((unsigned char)motor, resolutions[resolution]);
}

void motorsModule::maxStepRate(int motor, int rate) {
    controller.setMaxStepSpeed((unsigned char)motor, (unsigned short)rate);
}

void motorsModule::easingType(int motor, int easing) {
    controller.setEasingMode((unsigned char)motor, (unsigned char)easing);
}

void motorsModule::backlash(int motor, int value) {
    controller.setBacklash((unsigned char)motor, (unsigned short)value);
}

void motorsModule::clearClicked(int motor) {

}

void motorsModule::motionChanged(int motor, qreal p1, qreal p2, qreal p3, qreal p4) {

    // If the point is very close to 1, assume it is 1
    float point[4] = {p1, p2, p3, p4};
    for(int i = 0; i < 4; i++){
        if(point[i] > 0.99)
            point[i] = 1;
    }
    qDebug()<< "p1: " << point[0];
    qDebug()<< "p2: " << point[1];
    qDebug()<< "p3: " << point[2];
    qDebug()<< "p4: " << point[3];
    m_motions[(unsigned char)(motor - 1)].leadIn = point[0];
    m_motions[(unsigned char)(motor - 1)].leadOut = 1 - point[3];
    m_motions[(unsigned char)(motor - 1)].acceleration = point[1] - point[0];
    m_motions[(unsigned char)(motor - 1)].deceleration = point[3] - point[2];
    m_motions[(unsigned char)(motor - 1)].travelTime = point[3] - point[0];

    validateProgram(motor);
}

void motorsModule::validateMotorFinished(const QByteArray &data) {
    unsigned char ret = data[1];
    qDebug()<<ret;
    switch(m_validatingMotor) {
    case 1: m_pRootItem->setProperty("motor1Valid", ret != 0); break;
    case 2: m_pRootItem->setProperty("motor2Valid", ret != 0); break;
    case 3: m_pRootItem->setProperty("motor3Valid", ret != 0); break;
    default: qDebug()<<"something went wrong when checking if motor has valid settings";
    }
}

void motorsModule::validateMotorsFinished(const QByteArray &data) {
    unsigned char ret = data[1];
    qDebug()<<"validate motors response:"<<data.toHex();
    m_pRootItem->setProperty("motor1Valid", (ret & 1) != 0);
    m_pRootItem->setProperty("motor2Valid", (ret & 2) != 0);
    m_pRootItem->setProperty("motor3Valid", (ret & 4) != 0);
}
