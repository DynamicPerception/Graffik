#ifndef MOTORSMODULE_H
#define MOTORSMODULE_H

#include <QObject>

struct motion {
    qreal leadIn;
    qreal leadOut;
    qreal acceleration;
    qreal deceleration;
    qreal travelTime;

    qreal prevLeadIn;
    qreal prevAcceleration;
    qreal prevDeceleration;
    qreal prevTravelTime;
    quint8 prevMicrostep;

    // Set default values
    motion() {
        leadIn = 0;
        acceleration = 0.333333;
        deceleration = 0.333333;
        travelTime = 1;
        leadOut = 0;

        prevLeadIn = -1;
        prevAcceleration = -1;
        prevDeceleration = -1;
        prevTravelTime = -1;
        prevMicrostep = 0xFF;
    }
};

class motorsModule : public QObject
{
    Q_OBJECT
public:
    explicit motorsModule(QObject *root, QObject *parent = 0);
    motion motorMotion(unsigned char address, unsigned char motor) const {
      return m_motions[address][motor - 1];
    }

private:
    QObject *m_pRootItem;
    motion m_motions[255][3];
    unsigned char m_validatingMotor;

private slots:
    void validateProgram(int address = -1, int motor = -1);
    void motionChanged(int address, int motor, qreal p1, qreal p2, qreal p3, qreal p4);
    void validateMotorFinished(const QByteArray &data);
};

#endif // MOTORSMODULE_H
