#include "joystickModule.h"
#include "motionController.h"
#include <cmath>
#include <QDebug>

joystickModule::joystickModule(QObject *root, QObject *parent) :
    QObject(parent)
{
    m_pRootItem = root;
    m_maxMotorSpeed = 1;
    connect(m_pRootItem, SIGNAL(joystickMoved(int,int)), this, SLOT(joystickMoved(int,int)));
    connect(m_pRootItem, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));
    connect(m_pRootItem, SIGNAL(setStartClicked()), this, SLOT(setStartClicked()));
    connect(m_pRootItem, SIGNAL(setEndClicked()), this, SLOT(setEndClicked()));
    connect(m_pRootItem, SIGNAL(dampingChanged(qreal)), this, SLOT(dampingChanged(qreal)));
    connect(m_pRootItem, SIGNAL(maxJoystickSpeedChanged(qreal)), this, SLOT(maxJoystickSpeedChanged(qreal)));
}

void joystickModule::joystickMoved(int x, int y) {
    qDebug()<<"joystick moved:"<<x<<y;
    const float joystickRange = 64;
    unsigned maxSpeed = 4250 * m_maxMotorSpeed + 750;

    float speedX = (x / joystickRange) * maxSpeed;
    float speedY = (y / joystickRange) * maxSpeed;

    speedX = std::min(speedX, (float)maxSpeed);
    speedY = std::min(speedY, (float)maxSpeed);

    if(!controller.joystickMode()) {
        controller.setJoystickMode(true, true);
        controller.setWatchdogEnable(true, false);
    }

    controller.setContinuousSpeed(2, speedX, false);
    controller.setContinuousSpeed(3, speedY, false);

    if(qAbs(speedX) == 0 && qAbs(speedY) == 0)
        controller.setJoystickMode(false);
}

void joystickModule::sliderMoved(int x) {
    const float sliderRange = 128;
    unsigned maxSpeed = 4250 * m_maxMotorSpeed + 750;

    float speedX = (x / sliderRange) * maxSpeed;
    speedX = std::min(speedX, (float)maxSpeed);

    if(!controller.joystickMode()) {
        controller.setJoystickMode(true, false);
        controller.setWatchdogEnable(true, false);
    }

    controller.setContinuousSpeed(1, speedX);

    if(qAbs(speedX) < 3)
        controller.setJoystickMode(false, false);
}

void joystickModule::setStartClicked() {
    controller.setProgramStartPoint();
}

void joystickModule::setEndClicked() {
    controller.setProgramStopPoint();
}

void joystickModule::maxJoystickSpeedChanged(qreal pos) {
    m_maxMotorSpeed = pos;
}

void joystickModule::dampingChanged(qreal pos) {
    // Use a square relationship between accel and
    // position for better responsiveness of damping slider
    unsigned value = 24000.0 * (pos * pos) + 1000;
    for(int i = 1; i <= 3; ++i)
        controller.setMotorAcceleration((unsigned char)i, (float)value);
}
