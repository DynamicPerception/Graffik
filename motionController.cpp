#include "motionController.h"
#include <QDebug>

motionController &motionControllerInstance() {
    static motionController mc;
    return mc;
}

motionController::motionController(QObject *parent) :
    QObject(parent)
{
    m_deviceAddress = 3;
    m_motorsInfo.resize(3);
    m_processing = false;
    m_joystickMode = false;
    m_timer.setInterval(40);
    m_timer.start();

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
    connect(&m_serialPort, SIGNAL(readyRead()), this, SLOT(serialPortReadyRead()));
    connect(&m_serialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(serialPortError(QSerialPort::SerialPortError)));
}

bool motionController::openPort(const QString &portName) {
    m_serialPort.setPortName(portName);
    m_portName = portName;
    return m_serialPort.open(QSerialPort::ReadWrite);
}

void motionController::closePort() {
    m_serialPort.close();
}

void motionController::setGraffikModeEnable(bool enable) {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 0;
    cr.command = 50;
    cr.dataLength = 1;
    cr.data[0] = enable ? 1 : 0;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setMotorEnable(unsigned char motor, bool enable) {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = motor;
    cr.command = 3;
    cr.dataLength = 1;
    cr.data[0] = enable ? 1 : 0;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
        m_motorsInfo[motor - 1].enable = enable;
        if(!enable) m_motorsInfo[motor - 1].moving = false;
    } else qDebug()<<"serial port is not opened";
}

void motionController::setCameraEnable(bool enable) {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 4;
    cr.command = 2;
    cr.dataLength = 1;
    cr.data[0] = enable ? 1 : 0;
    qDebug()<<"set camera enable:"<<enable;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setFocusWithShutter(bool enable) {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 4;
    cr.command = 8;
    cr.dataLength = 1;
    cr.data[0] = enable ? 1 : 0;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setWatchdogEnable(bool enable) {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 0;
    cr.command = 14;
    cr.dataLength = 1;
    cr.data[0] = enable ? 1 : 0;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setShotsInterval(unsigned interval) {
    if(m_programInfo.interval == interval)
        return;

    qDebug()<<"setting interval:"<<interval;
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 4;
    cr.command = 10;
    cr.dataLength = 4;
    m_programInfo.interval = interval;

    //reorder bytes
    std::reverse_copy((char*)&interval, (char*)&interval + 4, cr.data);

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setMotorAcceleration(unsigned char motor, float value) {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = motor;
    cr.command = 14;
    cr.dataLength = 4;

    //reorder bytes
    std::reverse_copy((char*)&value, (char*)&value + 4, cr.data);

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setLeadInShots(unsigned char motor, unsigned shots) {
    if(m_motorsInfo[motor - 1].leadInShoots == shots)
        return;

    qDebug()<<"setting lead in shots, motor:"<<motor<<"value:"<<shots;
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = motor;
    cr.command = 19;
    cr.dataLength = 4;
    m_motorsInfo[motor - 1].leadInShoots = shots;

    //reorder bytes
    std::reverse_copy((char*)&shots, (char*)&shots + 4, cr.data);

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setExposureDelay(unsigned short delay) {
    if(m_programInfo.exposureDelay == delay)
        return;

    qDebug()<<"setting exposure delay:"<<delay;
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 4;
    cr.command = 7;
    cr.dataLength = 2;
    m_programInfo.exposureDelay = delay;

    //reorder bytes
    std::reverse_copy((char*)&delay, (char*)&delay + 2, cr.data);

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setMaxShots(unsigned short shots) {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 4;
    cr.command = 6;
    cr.dataLength = 2;

    //reorder bytes
    std::reverse_copy((char*)&shots, (char*)&shots + 2, cr.data);

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setExposureTime(unsigned time) {
    if(m_programInfo.exposure == time)
        return;

    qDebug()<<"setting exposure time:"<<time;
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 4;
    cr.command = 4;
    cr.dataLength = 4;
    m_programInfo.exposure = time;

    //reorder bytes
    std::reverse_copy((char*)&time, (char*)&time + 4, cr.data);

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setBacklash(unsigned char motor, unsigned short value) {
    if(m_programInfo.backlash == value)
        return;

    qDebug()<<"setting backlash value:"<<value;
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = motor;
    cr.command = 5;
    cr.dataLength = 2;
    m_programInfo.backlash = value;

    //reorder bytes
    std::reverse_copy((char*)&value, (char*)&value + 2, cr.data);

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setFocusTime(unsigned short time) {
    if(m_programInfo.focusTime == time)
        return;

    qDebug()<<"setting focus time:"<<time;
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 4;
    cr.command = 5;
    cr.dataLength = 2;
    m_programInfo.focusTime = time;

    //reorder bytes
    std::reverse_copy((char*)&time, (char*)&time + 2, cr.data);

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::expose(unsigned time) {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 4;
    cr.command = 3;
    cr.dataLength = 4;

    //reorder bytes
    std::reverse_copy((char*)&time, (char*)&time + 4, cr.data);

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setContinuousSpeed(unsigned char motor, float stepsPerSecs) {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = motor;
    cr.command = 13;
    cr.dataLength = 4;

    //reorder bytes
    stepsPerSecs *= m_motorsInfo[motor - 1].invertDirection ? -1 : 1;
    std::reverse_copy((char*)&stepsPerSecs, (char*)&stepsPerSecs + 4, cr.data);

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setMicroStepValue(unsigned char motor, unsigned char value) {
//    if(m_motorsInfo[motor - 1].microstep == value)
//        return;

    qDebug()<<"setting microstep value, motor:"<<motor<<"value:"<<value;
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = motor;
    cr.command = 6;
    cr.dataLength = 1;
    cr.data[0] = value;
    m_motorsInfo[motor - 1].microstep = value;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setProgramAcceleration(unsigned char motor, unsigned value) {
    if(m_motorsInfo[motor - 1].acceleration == value)
        return;

    qDebug()<<"setting acceleration, motor:"<<motor<<"value:"<<value;
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = motor;
    cr.command = 21;
    cr.dataLength = 4;
    m_motorsInfo[motor - 1].acceleration = value;

    //reorder bytes
    std::reverse_copy((char*)&value, (char*)&value + 4, cr.data);

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setProgramDeceleration(unsigned char motor, unsigned value) {
    if(m_motorsInfo[motor - 1].deceleration == value)
        return;

    qDebug()<<"setting deceleration, motor:"<<motor<<"value:"<<value;
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = motor;
    cr.command = 22;
    cr.dataLength = 4;
    m_motorsInfo[motor - 1].deceleration = value;

    //reorder bytes
    std::reverse_copy((char*)&value, (char*)&value + 4, cr.data);

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setTravelTime(unsigned char motor, unsigned time) {
    if(m_motorsInfo[motor - 1].travelTime == time)
        return;

    qDebug()<<"setting travel time, motor:"<<motor<<"value:"<<time;
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = motor;
    cr.command = 20;
    cr.dataLength = 4;
    m_motorsInfo[motor - 1].travelTime = time;

    //reorder bytes
    std::reverse_copy((char*)&time, (char*)&time + 4, cr.data);

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setProgramMode(unsigned char mode) {
    if(m_programInfo.programMode == mode)
        return;

    qDebug()<<"setting program mode:"<<mode;
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 0;
    cr.command = 22;
    cr.dataLength = 1;
    cr.data[0] = mode;
    m_programInfo.programMode = mode;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setMaxStepSpeed(unsigned char motor, unsigned short speed) {
    if(m_motorsInfo[motor - 1].maxStepSpeed == speed)
        return;

    qDebug()<<"setting max step speed, motor:"<<motor<<"value:"<<speed;
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = motor;
    cr.command = 7;
    cr.dataLength = 2;
    m_motorsInfo[motor - 1].maxStepSpeed = speed;

    //reorder bytes
    std::reverse_copy((char*)&speed, (char*)&speed + 2, cr.data);

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setDirection(unsigned char motor, unsigned char direction) {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = motor;
    cr.command = 8;
    cr.dataLength = 1;
    cr.data[0] = direction;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
        m_motorsInfo[motor - 1].direction = direction;
    } else qDebug()<<"serial port is not opened";
}

void motionController::setMotorSleep(unsigned char motor, bool sleep) {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = motor;
    cr.command = 2;
    cr.dataLength = 1;
    cr.data[0] = sleep ? 1 : 0;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::moveMotor(unsigned char motor, unsigned char direction, unsigned steps) {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = motor;
    cr.command = 15;
    cr.dataLength = 5;
    cr.data[0] = direction;

    std::reverse_copy((char*)&steps, (char*)&steps + 4, &cr.data[1]);

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
        m_motorsInfo[motor - 1].direction = direction;
        m_motorsInfo[motor - 1].moving = true;
    } else qDebug()<<"serial port is not opened";
}

void motionController::motorPosition(unsigned char motor) {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = motor;
    cr.command = 106;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
        m_motorsInfo[motor - 1].moving = false;
    } else qDebug()<<"serial port is not opened";
}

void motionController::stopMotor(unsigned char motor) {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = motor;
    cr.command = 4;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
        m_motorsInfo[motor - 1].moving = false;
    } else qDebug()<<"serial port is not opened";
}

void motionController::setEasingMode(unsigned char motor, unsigned char mode) {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = motor;
    cr.command = 18;
    cr.dataLength = 1;
    cr.data[0] = mode;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setProgramStartPoint(unsigned char motor, unsigned stepPosition) {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = motor;
    cr.command = 16;
    cr.dataLength = 4;

    std::reverse_copy((char*)&stepPosition, (char*)&stepPosition + 4, cr.data);

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setProgramStartPoint() {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 0;
    cr.command = 26;
    cr.dataLength = 0;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setProgramStopPoint(unsigned char motor, unsigned stepPosition) {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = motor;
    cr.command = 17;
    cr.dataLength = 4;

    std::reverse_copy((char*)&stepPosition, (char*)&stepPosition + 4, cr.data);

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setProgramStopPoint() {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 0;
    cr.command = 27;
    cr.dataLength = 0;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::sendMotorToStartPoint(unsigned char motor) {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = motor;
    cr.command = 23;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::sendMotorToStopPoint(unsigned char motor) {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = motor;
    cr.command = 24;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setPingPongFlag(bool enable) {
    if(m_programInfo.pingPongFlag == enable)
        return;

    qDebug()<<"setting Ping-Pong flag:"<<enable;
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 0;
    cr.command = 24;
    cr.dataLength = 1;
    cr.data[0] = enable ? 1 : 0;
    m_programInfo.pingPongFlag = enable;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setJoystickMode(bool enable) {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 0;
    cr.command = 23;
    cr.dataLength = 1;
    cr.data[0] = enable ? 1 : 0;
    m_joystickMode = enable;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::startPlannedMove() {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 0;
    cr.command = 2;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::stopPlannedMove() {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 0;
    cr.command = 4;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::pausePlannedMove() {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 0;
    cr.command = 3;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::firmwareVersion() {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 0;
    cr.command = 100;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::shotsInterval() {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 4;
    cr.command = 108;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::motorsStatus() {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 0;
    cr.command = 124;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::motorRunning(unsigned char motor) {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = motor;
    cr.command = 107;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::motorsRunning() {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 0;
    cr.command = 128;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::setDeviceAddress(unsigned char address) {
    m_deviceAddress = address;
}

void motionController::validateMotor(unsigned char motor) {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = motor;
    cr.command = 118;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::powerSaveStatus() {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 0;
    cr.command = 130;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::validateMotors() {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 0;
    cr.command = 129;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::programProgress() {
    commandRequest cr;
    cr.address = m_deviceAddress;
    cr.subAddress = 0;
    cr.command = 123;

    if(m_serialPort.isOpen()) {
        m_requestsQueue.enqueue(cr);
    } else qDebug()<<"serial port is not opened";
}

void motionController::invertMotor(unsigned char motor, bool yes) {
    m_motorsInfo[motor - 1].invertDirection = yes;
}

void motionController::processCommands() {
    if(!m_requestsQueue.empty()) {
        commandRequest cr = m_requestsQueue.dequeue();
        m_serialPort.setProperty("command", cr.command);
        m_serialPort.setProperty("subAddress", cr.subAddress);

        qDebug()<<"command processing:"<<QByteArray((const char*)&cr, cr.size()).toHex();

        m_serialPort.write((const char*)&cr, cr.size());
        m_serialPort.flush();
        m_processing = true;
    } else {
        m_processing = false;
    }
}

void motionController::replyEmiter(unsigned char subAddress, unsigned char command, const QByteArray &data) {
    if(subAddress == 0) {
        switch(command) {
        case 100: emit firmwareVersionFinished(data); break;
        case 124: emit motorStatusFinished(data); break;
        case 123: emit programProgressFinished(data); break;
        case 129: emit validateMotorsFinished(data); break;
        case 130: emit powerSaveStatusFinished(data); break;
        case 128: emit motorsRunningFinished(data); break;
        case 26: emit settingStartPointFinished(data); break;
        case 27: emit settingEndPointFinished(data); break;
        }
    } else if(subAddress == 4) {
        switch(command) {
        case 108: qDebug()<<data.toHex(); break;
        }
    } else if(subAddress >= 1 && subAddress <= 3) {
        switch(command) {
        case 106: emit motorPositionFinished(data); break;
        case 107: emit motorRunningFinished(data); break;
        case 118: emit validateMotorFinished(data); break;
        }
    }
}

void motionController::serialPortReadyRead() {
    m_repliesBuffer.append(m_serialPort.readAll());

    if(m_repliesBuffer.size() >= 10) {
        int replyPos = m_repliesBuffer.indexOf(QByteArray("\x00\x00\x00\x00\x00\xFF"));
        if(replyPos > 0) {
            m_repliesBuffer.remove(0, replyPos);
            qDebug()<<"shifted reply";
        } else if(replyPos == -1) {
            m_repliesBuffer.clear();
            m_processing = false;
            qDebug()<<"bad reply";
            return;
        }

        int dataSize = m_repliesBuffer[9]; //byte 10 cointains data size of reply
        if(dataSize + 10 <= m_repliesBuffer.size()) {
            QByteArray reply = m_repliesBuffer.mid(10, dataSize);

            unsigned char command = m_serialPort.property("command").toUInt();
            unsigned char subAddress = m_serialPort.property("subAddress").toUInt();
            qDebug()<<"command processed:"<<m_repliesBuffer.left(10 + dataSize).toHex();
            replyEmiter(subAddress, command, reply);

            m_repliesBuffer.remove(0, 10 + dataSize);
            //we can process next command
            //processCommands();
            m_timer.start();
        }
    }
}

void motionController::serialPortError(QSerialPort::SerialPortError err) {
    if(err != QSerialPort::NoError)
        qDebug()<<"serial port error:"<<err;
}

void motionController::timerTimeout() {
    //qDebug()<<m_repliesBuffer.toHex();
    //if(!m_processing)
        processCommands();
}


