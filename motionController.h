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
    QString portName() const { return m_portName; }

    void testController();
    void assignAddress(unsigned char address);
    void setGraffikModeEnable(bool enable);
    void setMotorEnable(unsigned char motor, bool enable);
    void setCameraEnable(bool enable);
    void setFocusWithShutter(bool enable);
    void setWatchdogEnable(bool enable);
    void setShotsInterval(unsigned interval);
    void setMotorAcceleration(unsigned char motor, float value);
    void setLeadInShots(unsigned char motor, unsigned shots);
    void setLeadOutShots(unsigned char motor, unsigned shots);
    void setExposureDelay(unsigned short delay);
    void setMaxShots(unsigned short shots);
    void setExposureTime(unsigned time);
    void setBacklash(unsigned char motor, unsigned short value);
    void setFocusTime(unsigned short time);
    void expose(unsigned time);
    void setContinuousSpeed(unsigned char motor, float stepsPerSecs);
    void setMicroStepValue(unsigned char motor, unsigned char value);
    void setProgramAcceleration(unsigned char motor, unsigned value);
    void setProgramDeceleration(unsigned char motor, unsigned value);
    void setTravelTime(unsigned char motor, unsigned time);
    void setProgramMode(unsigned char mode);
    void setMaxStepSpeed(unsigned char motor, unsigned short speed);
    void setDirection(unsigned char motor, unsigned char direction);
    void setMotorSleep(unsigned char motor, bool sleep);
    void moveMotor(unsigned char motor, unsigned char direction, unsigned steps = 0);
    void motorPosition(unsigned char motor);
    void stopMotor(unsigned char motor);
    void setEasingMode(unsigned char motor, unsigned char mode);
    void setProgramStartPoint(unsigned char motor, unsigned stepPosition);
    void setProgramStartPoint();
    void setProgramStopPoint(unsigned char motor, unsigned stepPosition);
    void setProgramStopPoint();
    void sendMotorToStartPoint(unsigned char motor);
    void sendMotorToStopPoint(unsigned char motor);
    void setPingPongFlag(bool enable);
    void setJoystickMode(bool enable);
    void startPlannedMove();
    void stopPlannedMove();
    void pausePlannedMove();
    void firmwareVersion();
    void shotsInterval();
    void motorsStatus();
    void motorRunning(unsigned char motor);
    void motorsRunning();
    void setDeviceAddress(unsigned char address);
    void validateMotor(unsigned char motor);
    void powerSaveStatus();
    void validateMotors();
    void programProgress();
    void invertMotor(unsigned char motor, bool yes);

    unsigned char motorDirection(unsigned char motor) const { return m_motorsInfo[motor - 1].direction; }
    unsigned char motorMicrostep(unsigned char motor) const { return m_motorsInfo[motor - 1].microstep; }
    bool isMotorEnabled(unsigned char motor) const { return m_motorsInfo[motor - 1].enable; }
    bool isMotorMoving(unsigned char motor) const { return m_motorsInfo[motor - 1].moving; }
    bool joystickMode() const { return m_joystickMode; }

    template<typename T>
    static T parseResult(const QByteArray &src) {
        T dst;
        std::reverse_copy((char*)src.data() + 1, (char*)src.data() + 1 + sizeof(T), (char*)&dst);
        return dst;
    }

private:
    explicit motionController(QObject *parent = 0);
    QSerialPort m_serialPort;
    QString m_portName;
    QByteArray m_repliesBuffer;
    QTimer m_timer;
    bool m_blocked;
    bool m_joystickMode;
    unsigned char m_deviceAddress;

    QVector<motorInfo> m_motorsInfo;
    programInfo m_programInfo;
    QQueue<commandRequest> m_requestsQueue;
    void processCommands();
    void replyEmiter(unsigned char address, unsigned char subAddress,
                     unsigned char command, const QByteArray &data);

private slots:
    void serialPortReadyRead();
    void serialPortError(QSerialPort::SerialPortError err);
    void timerTimeout();

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
};

motionController &motionControllerInstance();

#define controller motionControllerInstance()

#endif // MOTIONCONTROLLER_H
