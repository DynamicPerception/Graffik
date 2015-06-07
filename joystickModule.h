#ifndef JOYSTICKMODULE_H
#define JOYSTICKMODULE_H

#include <QObject>

class joystickModule : public QObject
{
    Q_OBJECT
public:
    explicit joystickModule(QObject *root, QObject *parent = 0);
    qreal maxMotorSpeed() const { return m_maxMotorSpeed; }

private:
    QObject *m_pRootItem;
    qreal m_maxMotorSpeed;

private slots:
    void joystickMoved(int x, int y);
    void sliderMoved(int x);
    void maxJoystickSpeedChanged(qreal pos);
    void dampingChanged(qreal pos);
};

#endif // JOYSTICKMODULE_H
