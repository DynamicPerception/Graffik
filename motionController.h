#ifndef MOTIONCONTROLLER_H
#define MOTIONCONTROLLER_H

#include <QObject>
#include <QVector>
#include <QQueue>
#include <QTimer>
#include <QSerialPort>
#include <algorithm>

struct commandRequest {
    unsigned char blocking;
    unsigned char breakSequence[6];
    unsigned char address;
    unsigned char subAddress;
    unsigned char command;
    unsigned char dataLength;
    unsigned char data[256];

    unsigned size() const { return 10 + dataLength; }
    commandRequest() {
        memset(breakSequence, 0, 5);
        breakSequence[5] = 0xFF;
        dataLength = 0;
        blocking = 0;
    }
};

struct programInfo {
    unsigned char programMode;
    bool pingPongFlag;
    unsigned interval;
    unsigned exposure;
    unsigned short exposureDelay;
    unsigned short focusTime;
    unsigned backlash;

    programInfo() {
        backlash = 0;
        programMode = 0xFF;
        exposure = 0xFFFFFFFF;
        interval = 0xFFFFFFFF;
        focusTime = 0xFFFF;
        exposureDelay = 0xFFFF;
        pingPongFlag = false;
    }
};

struct motorInfo {
    unsigned short maxStepSpeed;
    unsigned char port;
    unsigned char stepResolution;
    unsigned char direction;
    unsigned char microstep;
    unsigned travelTime;
    unsigned acceleration;
    unsigned deceleration;
    unsigned leadInShots;
    unsigned leadOutShots;
    bool enable;
    bool moving;
    bool powerSave;
    bool invertDirection;

    motorInfo() {
        invertDirection = false;
        microstep = 0xFF;
        stepResolution = 0xFF;
        direction = 0xFF;
        maxStepSpeed = 0xFFFF;

        leadInShots = 0xFFFFFFFF;
        leadOutShots = 0xFFFFFFFF;
        deceleration = 0xFFFFFFFF;
        acceleration = 0xFFFFFFFF;
        travelTime = 0xFFFFFFFF;

        enable = false;
        moving = false;
        powerSave = false;
    }
};

class motionController : public QObject
{
    Q_OBJECT
    friend motionController &motionControllerInstance();

public:
    bool openPort(const QString &portName);
    void clearQueue();
    QString portName() const { return m_portName; }
    void requestClosePort();

    void testController(bool blocking = true);
    void assignAddress(unsigned char address, bool blocking = false);
    void setGraffikModeEnable(bool enable, bool blocking = true);
    void setMotorEnable(unsigned char motor, bool enable, bool blocking = true);
    void setFocusWithShutter(bool enable, bool blocking = true);
    void setWatchdogEnable(bool enable, bool blocking = true);
    void setShotsInterval(unsigned interval, bool blocking = true);
    void setMotorAcceleration(unsigned char motor, float value, bool blocking = true);
    void setLeadInShots(unsigned char motor, unsigned shots, bool blocking = true);
    void setLeadOutShots(unsigned char motor, unsigned shots, bool blocking = true);
    void setExposureDelay(unsigned short delay, bool blocking = true);
    void setMaxShots(unsigned short shots, bool blocking = true);
    void setExposureTime(unsigned time, bool blocking = true);
    void setFocusTime(unsigned short time, bool blocking = true);
    void expose(unsigned time, bool blocking = true);
    void setContinuousSpeed(unsigned char motor, float stepsPerSecs, bool blocking = true);
    void setProgramAcceleration(unsigned char motor, unsigned value, bool blocking = true);
    void setProgramDeceleration(unsigned char motor, unsigned value, bool blocking = true);
    void setTravelTime(unsigned char motor, unsigned time, bool blocking = true);
    void setProgramMode(unsigned char mode, bool blocking = true);
    void setMaxStepSpeed(unsigned char motor, unsigned short speed, bool blocking = true);
    void setDirection(unsigned char motor, unsigned char direction, bool blocking = true);
    void moveMotor(unsigned char motor, unsigned char direction, unsigned steps = 0, bool blocking = true);
    void motorPosition(unsigned char motor, bool blocking = true);
    void stopMotor(unsigned char motor, bool blocking = true);
    void setProgramStartPoint(unsigned char motor, unsigned stepPosition, bool blocking = true);
    void setProgramStartPoint(bool blocking = true);
    void setProgramStopPoint(unsigned char motor, unsigned stepPosition, bool blocking = true);
    void setProgramStopPoint(bool blocking = true);
    void sendMotorToStartPoint(unsigned char motor, bool blocking = true);
    void sendMotorToStopPoint(unsigned char motor, bool blocking = true);
    void setPingPongFlag(bool enable, bool blocking = true);
    void setJoystickMode(bool enable, bool blocking = true);
    void startPlannedMove(bool blocking = true);
    void stopPlannedMove(bool blocking = true);
    void pausePlannedMove(bool blocking = true);
    void firmwareVersion(bool blocking = true);
    void shotsInterval(bool blocking = true);
    void motorsStatus(bool blocking = true);
    void motorRunning(unsigned char motor, bool blocking = true);
    void motorsRunning(bool blocking = true);
    void validateMotor(unsigned char motor, bool blocking = true);
    void powerSaveStatus(bool blocking = true);
    void validateMotors(bool blocking = true);
    void programProgress(bool blocking = true);

    Q_INVOKABLE void setAction(const QString &id);
    Q_INVOKABLE void setDeviceAddress(unsigned char address);
    Q_INVOKABLE void setCameraEnable(unsigned char address, bool enable, bool blocking = true);
    Q_INVOKABLE void setCameraEnable(bool enable, bool blocking = true);
    Q_INVOKABLE void setEasingMode(unsigned char address, unsigned char motor, unsigned char mode, bool blocking = true);
    Q_INVOKABLE void setEasingMode(unsigned char motor, unsigned char mode, bool blocking = true);
    Q_INVOKABLE void setMicroStepValue(unsigned char address, unsigned char motor, unsigned char value, bool blocking = true);
    Q_INVOKABLE void setMicroStepValue(unsigned char motor, unsigned char value, bool blocking = true);
    Q_INVOKABLE void setBacklash(unsigned char address, unsigned char motor, unsigned short value, bool blocking = true);
    Q_INVOKABLE void setBacklash(unsigned char motor, unsigned short value, bool blocking = true);
    Q_INVOKABLE void setMotorSleep(unsigned char address, unsigned char motor, bool sleep, bool blocking = true);
    Q_INVOKABLE void invertMotorDirection(unsigned char address, unsigned char motor, bool yes);

    unsigned char motorDirection(unsigned char address, unsigned char motor) const { return m_motorsInfo[address][motor - 1].direction; }
    unsigned char motorMicrostep(unsigned char address, unsigned char motor) const { return m_motorsInfo[address][motor - 1].microstep; }
    unsigned char motorDirection(unsigned char motor) { return m_motorsInfo[m_deviceAddress][motor - 1].direction; }
    unsigned char motorMicrostep(unsigned char motor) { return m_motorsInfo[m_deviceAddress][motor - 1].microstep; }
    bool isMotorEnabled(unsigned char address, unsigned char motor) const { return m_motorsInfo[address][motor - 1].enable; }
    bool isMotorMoving(unsigned char address, unsigned char motor) const { return m_motorsInfo[address][motor - 1].moving; }
    bool isMotorEnabled(unsigned char motor) const { return m_motorsInfo[m_deviceAddress][motor - 1].enable; }
    bool isMotorMoving(unsigned char motor) const { return m_motorsInfo[m_deviceAddress][motor - 1].moving; }

    bool joystickMode() const { return m_joystickMode; }
    unsigned char currentDeviceAddress() const { return m_deviceAddress; }

private:
    explicit motionController(QObject *parent = 0);
    QSerialPort m_serialPort;
    QString m_portName;
    QByteArray m_repliesBuffer;
    QTimer m_timer;
    QTimer m_connectionTimer;
    bool m_blocked;
    bool m_joystickMode;
    unsigned char m_deviceAddress;

    motorInfo m_motorsInfo[255][3];
    programInfo m_programInfo;
    QQueue<commandRequest> m_requestsQueue;
    void processCommands();
    void replyEmiter(unsigned char address, unsigned char subAddress,
                     unsigned char command, const QByteArray &data);

private slots:
    void serialPortReadyRead();
    void serialPortError(QSerialPort::SerialPortError err);
    void timerTimeout();
    void connectionError();

public slots:
    void clearPort();
    void closePort();

signals:
    void firmwareVersionFinished(const QByteArray &data);
    void motorStatusFinished(const QByteArray &data);
    void motorPositionFinished(const QByteArray &data);
    void motorRunningFinished(const QByteArray &data);
    void validateMotorFinished(const QByteArray &data);
    void validateMotorsFinished(const QByteArray &data);
    void programProgressFinished(const QByteArray &data);
    void powerSaveStatusFinished(const QByteArray &data);
    void motorsRunningFinished(const QByteArray &data);
    void settingStartPointFinished(const QByteArray &data);
    void settingEndPointFinished(const QByteArray &data);
    void assignAddressFinished(const QByteArray &data);
    void testControllerFinished(const QByteArray &data);
    void actionFinished(const QVariant &data);
    void disconnected();
};

motionController &motionControllerInstance();

#define controller motionControllerInstance()

#endif // MOTIONCONTROLLER_H
