#include "motorsModule.h"
#include "motionController.h"
#include <QTime>
#include <QDebug>

motorsModule::motorsModule(QObject *root, QObject *parent) :
    QObject(parent)
{
    m_pRootItem = root;

    connect(&controller, SIGNAL(validateMotorFinished(QByteArray)), this, SLOT(validateMotorFinished(QByteArray)));
    connect(&controller, SIGNAL(settingStartPointFinished(QByteArray)), this, SLOT(validateProgram()));
    connect(&controller, SIGNAL(settingEndPointFinished(QByteArray)), this, SLOT(validateProgram()));
    connect(m_pRootItem, SIGNAL(validateRequest(int)), this, SLOT(validateProgram(int)));
    connect(m_pRootItem, SIGNAL(motionChanged(int,int,qreal,qreal,qreal,qreal)), this,
            SLOT(motionChanged(int,int,qreal,qreal,qreal,qreal)));
}

void motorsModule::validateProgram(int address, int motor) {
    address = address != -1 ? address : m_pRootItem->property("currentControllerAddress").toInt();
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
        motion m = m_motions[address][motor - 1];

        unsigned leadIn = unsigned(qRound(length * m.leadIn));
        unsigned leadOut = unsigned(qRound(length * m.leadOut));
        unsigned accel = unsigned(qRound(length * m.acceleration));
        unsigned decel = unsigned(qRound(length * m.deceleration));

        // This ensures enforces travel will never be off by a frame in SMS due to rounding
        unsigned travel = length - (leadIn + leadOut);

        controller.setLeadInShots((unsigned char)motor, leadIn);
        controller.setLeadOutShots((unsigned char)motor, leadOut);
        controller.setProgramAcceleration((unsigned char)motor, accel);
        controller.setProgramDeceleration((unsigned char)motor, decel);
        controller.setTravelTime((unsigned char)motor, travel);

        m_validatingMotor = (unsigned char)motor;
        controller.validateMotor((unsigned char)motor);
    }
    // Changing input other than graph points
    else {
        for(int i = 1; i <= 3; ++i) {
            motion m = m_motions[address][i - 1];

            unsigned leadIn = unsigned(qRound(length * m.leadIn));
            unsigned leadOut = unsigned(qRound(length * m.leadOut));
            unsigned accel = unsigned(qRound(length * m.acceleration));
            unsigned decel = unsigned(qRound(length * m.deceleration));

            // This ensures enforces travel will never be off by a frame in SMS due to rounding
            unsigned travel = length - (leadIn + leadOut);

            controller.setLeadInShots((unsigned char)i, leadIn);
            controller.setLeadOutShots((unsigned char)i, leadOut);
            controller.setProgramAcceleration((unsigned char)i, accel);
            controller.setProgramDeceleration((unsigned char)i, decel);
            controller.setTravelTime((unsigned char)i, travel);
        }

        controller.validateMotors();
    }
}

void motorsModule::motionChanged(int address, int motor, qreal p1, qreal p2, qreal p3, qreal p4) {
    // If the point is very close to 1, assume it is 1
    float point[4];
    point[0] = p1; point[1] = p2;
    point[2] = p3; point[3] = p4;

    for(int i = 0; i < 4; i++){
        if(point[i] > 0.99)
            point[i] = 1;
    }

    qDebug()<< "p1: " << point[0];
    qDebug()<< "p2: " << point[1];
    qDebug()<< "p3: " << point[2];
    qDebug()<< "p4: " << point[3];

    m_motions[address][(unsigned char)(motor - 1)].leadIn = point[0];
    m_motions[address][(unsigned char)(motor - 1)].leadOut = 1 - point[3];
    m_motions[address][(unsigned char)(motor - 1)].acceleration = point[1] - point[0];
    m_motions[address][(unsigned char)(motor - 1)].deceleration = point[3] - point[2];
    m_motions[address][(unsigned char)(motor - 1)].travelTime = point[3] - point[0];

    validateProgram(address, motor);
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
