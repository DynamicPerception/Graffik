#include "mainWindow.h"
#include "motionController.h"

#include <QSerialPortInfo>
#include <QDateTime>
#include <QDebug>

mainWindow::mainWindow(QObject *parent)
    : QObject(parent)
{
    m_pRootItem = parent;
    m_pPortsModel = new listModel(new comPortItem, this);
    m_pMotorsModule = new motorsModule(m_pRootItem, this);
    m_pJoystickModule = new joystickModule(m_pRootItem, this);
    m_motorsMoving = false;
    m_isOnStartPosition = false;

    m_pRootItem->setProperty("availablePorts", QVariant::fromValue<listModel*>(m_pPortsModel));

    connect(&controller, SIGNAL(powerSaveStatusFinished(QByteArray)), this, SLOT(powerSaveStatusFinished(QByteArray)));
    connect(&controller, SIGNAL(firmwareVersionFinished(QByteArray)), this, SLOT(controllerConnected()));
    connect(&controller, SIGNAL(motorStatusFinished(QByteArray)), this, SLOT(motorsStatusFinished(QByteArray)));
    connect(&controller, SIGNAL(motorsRunningFinished(QByteArray)), this, SLOT(movementCheckFinished(QByteArray)));
    connect(&controller, SIGNAL(programProgressFinished(QByteArray)), this, SLOT(programProgressFinished(QByteArray)));
    connect(m_pRootItem, SIGNAL(assignAddressRequest(QString,int)), this, SLOT(assignAddressRequest(QString,int)));
    connect(m_pRootItem, SIGNAL(setCameraEnabled(bool)), this, SLOT(setCameraEnabled(bool)));
    connect(m_pRootItem, SIGNAL(goToProgramStartClicked()), this, SLOT(goToProgramStartClicked()));
    connect(m_pRootItem, SIGNAL(allStopClicked()), this, SLOT(allStopClicked()));
    connect(m_pRootItem, SIGNAL(startProgramClicked()), this, SLOT(startProgramClicked()));
    connect(m_pRootItem, SIGNAL(stopProgramClicked()), this, SLOT(stopProgramClicked()));
    connect(m_pRootItem, SIGNAL(pauseProgramClicked()), this, SLOT(pauseProgramClicked()));
    connect(m_pRootItem, SIGNAL(portsRescanClicked()), this, SLOT(portsRescanClicked()));
    connect(m_pRootItem, SIGNAL(checkMotorAttachmentClicked()), this, SLOT(checkMotorAttachmentClicked()));
    connect(m_pRootItem, SIGNAL(connectToPortClicked(QString,QString)), this, SLOT(connectToPortClicked(QString,QString)));
    connect(m_pRootItem, SIGNAL(closePort()), this, SLOT(closePort()));

    connect(&m_progressTimer, SIGNAL(timeout()), this, SLOT(programProgressRequest()));
    connect(&m_movementTimer, SIGNAL(timeout()), this, SLOT(movementCheckRequest()));

    m_progressTimer.setInterval(1000);
    m_movementTimer.setInterval(1000);
    portsRescanClicked();
}

void mainWindow::initController() {
    controller.setGraffikModeEnable(true);
    controller.setJoystickMode(false);
    controller.setWatchdogEnable(false);
    controller.setCameraEnable(true);

    controller.setMotorEnable(1, true);
    controller.setMotorEnable(2, true);
    controller.setMotorEnable(3, true);

    controller.setMicroStepValue(1, 4);
    controller.setMicroStepValue(2, 4);
    controller.setMicroStepValue(3, 4);

    controller.setEasingMode(1, 2);
    controller.setEasingMode(2, 2);
    controller.setEasingMode(3, 2);

    controller.setProgramMode(1);
    controller.setFocusWithShutter(true);
    controller.powerSaveStatus();
    for(int i = 1; i <= 3; ++i)
        controller.setMotorAcceleration((unsigned char)i, (float)25000);
}

void mainWindow::programProgressRequest() {
    controller.programProgress();
}

void mainWindow::controllerConnected() {
    qDebug()<<"controller connected";
    //m_pRootItem->setProperty("connected", true);
    controller.assignAddress((unsigned char)m_controllerAddress);
}

/*
    programStatus === 0 - stopped
    programStatus === 1 - running
    programStatus === 2 - paused
    programStatus === 3 - going to start position
*/

void mainWindow::startProgramClicked() {
    m_pRootItem->setProperty("progressFormVisible", true);
    m_pRootItem->setProperty("programStatus", 1);

    unsigned interval = m_pRootItem->property("interval").toReal() * 1000;
    unsigned focusTime = m_pRootItem->property("focusTime").toReal() * 1000;
    unsigned triggerTime = m_pRootItem->property("triggerTime").toReal() * 1000;
    unsigned exposureDelay = m_pRootItem->property("exposureDelay").toReal() * 1000;
    unsigned programMode = m_pRootItem->property("programMode").toInt();
    bool pingPongFlag = m_pRootItem->property("pingPong").toBool();

    float length;
    controller.setProgramMode((unsigned char)programMode);
    controller.setPingPongFlag(pingPongFlag);

    // Determine the overall program length and set general parameters
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
        length += (int)(videoLengthSeconds - (int)videoLengthSeconds);
        length *= 1000; //to msecs
    }

    // Assign motor parameters
    for(unsigned char i = 1; i <= 3; ++i) {
        motion m = m_pMotorsModule->motorMotion(i);

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

    controller.startPlannedMove();
    m_progressTimer.start();
}

void mainWindow::stopProgramClicked() {
    qDebug()<<"stop planned move";
    m_progressTimer.stop();
    m_movementTimer.stop();
    m_pRootItem->setProperty("programStatus", 0);
    m_pRootItem->setProperty("progressFormVisible", false);
    m_pRootItem->setProperty("motorsOnStartPositions", false);
    controller.stopPlannedMove();

    for(unsigned char i = 1; i <= 3; ++i)
        controller.setMicroStepValue(i, controller.motorMicrostep(i));
}

void mainWindow::pauseProgramClicked() {
    qDebug()<<"pause planned move";
    m_progressTimer.stop();
    m_pRootItem->setProperty("programStatus", 2);
    controller.pausePlannedMove();
}

void mainWindow::goToProgramStartClicked() {
    controller.setJoystickMode(false);
    controller.sendMotorToStartPoint(1);
    controller.sendMotorToStartPoint(2);
    controller.sendMotorToStartPoint(3);

    m_pRootItem->setProperty("progressFormVisible", true);
    m_pRootItem->setProperty("programStatus", 3);
    m_movementTimer.start();
}

void mainWindow::allStopClicked() {
    m_progressTimer.stop();
    m_movementTimer.stop();
    m_pRootItem->setProperty("programStatus", 0);
    m_pRootItem->setProperty("progressFormVisible", false);
    m_pRootItem->setProperty("motorsOnStartPositions", false);

    for(unsigned char i = 1; i <= 3; ++i) {
        controller.stopMotor(i);
        controller.setMicroStepValue(i, controller.motorMicrostep(i));
    }
}

void mainWindow::portsRescanClicked() {
    m_pPortsModel->clear();
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    foreach(QSerialPortInfo port, ports) {
        QString portName = port.portName();
        m_pPortsModel->appendRow(new comPortItem(portName, this));
    }
}

void mainWindow::checkMotorAttachmentClicked() {
    qDebug()<<"check motor attachment clicked";
    controller.motorsStatus();
}

void mainWindow::setCameraEnabled(bool enable) {
    controller.setCameraEnable(enable);
}

void mainWindow::motorsStatusFinished(const QByteArray &data) {
    if(!data.size()) {
        qDebug()<<"motor attachment data is empty";
        return;
    }

    unsigned char ret = data[1];
    //ret = 0xff;
    m_pRootItem->setProperty("motor1Available", (ret & 1) != 0);
    m_pRootItem->setProperty("motor2Available", (ret & 2) != 0);
    m_pRootItem->setProperty("motor3Available", (ret & 4) != 0);
}

void mainWindow::movementCheckRequest() {
    controller.motorsRunning();
}

void mainWindow::movementCheckFinished(const QByteArray &data) {
    if(!data.size()) {
        qDebug()<<"running state data is empty";
        return;
    }

    unsigned char ret = data[1];
    m_pRootItem->setProperty("motorsOnStartPositions", ret == 0);
    if(ret == 0 && m_pRootItem->property("programStatus").toInt() != 0) {
        m_pRootItem->setProperty("progressFormVisible", false);
        m_pRootItem->setProperty("programStatus", 0);
        m_movementTimer.stop();
    }
}

void mainWindow::connectToPortClicked(const QString &portName, const QString &address) {
    if(controller.portName() == portName)
        controller.closePort();

    controller.setDeviceAddress((unsigned char)address.toInt());
    if(controller.openPort(portName))
        controller.firmwareVersion();
}

void mainWindow::programProgressFinished(const QByteArray &data) {
    if(!data.size()) {
        qDebug()<<"program progress data is empty";
        return;
    }

    unsigned char progress = data[1];
    qDebug()<<"progress:"<<progress;

    static unsigned char prev = 0;
    if(progress == 100 || prev > progress) {
        qDebug()<<"program finished";
        m_progressTimer.stop();
        m_movementTimer.stop();
        m_pRootItem->setProperty("motorsOnStartPositions", false);
        m_pRootItem->setProperty("progressFormVisible", false);
        m_pRootItem->setProperty("programStatus", 0);
        prev = 0;

        for(unsigned char i = 1; i <= 3; ++i)
            controller.setMicroStepValue(i, controller.motorMicrostep(i));
    } else {
        prev = progress;
    }

    qDebug()<<"progress:"<<progress;
    m_pRootItem->setProperty("programProgress", progress);
}

void mainWindow::powerSaveStatusFinished(const QByteArray &data) {
    if(!data.size()) {
        qDebug()<<"power status data is empty";
        return;
    }

    unsigned char ret = data[1];
    m_pRootItem->setProperty("motor1PowerSave", (ret & 1) != 0);
    m_pRootItem->setProperty("motor2PowerSave", (ret & 2) != 0);
    m_pRootItem->setProperty("motor3PowerSave", (ret & 4) != 0);

    controller.motorsStatus();
}

void mainWindow::assignAddressRequest(const QString &portName, int address) {
    m_controllerAddress = address;
    if(controller.portName() == portName)
        controller.closePort();

    controller.setDeviceAddress((unsigned char)address);
    if(controller.openPort(portName))
        controller.firmwareVersion();
}

void mainWindow::closePort() {
    controller.closePort();
}
