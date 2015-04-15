#ifndef GLOBAL_H
#define GLOBAL_H

#include <QObject>
#include <QVector>
#include "motionController.h"

class core : public QObject
{
    Q_OBJECT

public:
    explicit core(QObject *parent = 0);

private:
    float m_exposure;
    float m_interval;
    unsigned m_buffer;
    unsigned m_shootingDuration;
    unsigned m_frames;
    unsigned m_videoLength;
    unsigned m_fps;
};

core &coreInstance();
#define global coreInstance()

#endif // GLOBAL_H
