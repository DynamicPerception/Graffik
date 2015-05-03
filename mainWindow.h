#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QTimer>

#include "joystickModule.h"
#include "motorsModule.h"
#include "models/comPortItem.h"

class mainWindow : public QObject
{
    Q_OBJECT

public:
    mainWindow(QObject *parent = 0);

private:
    listModel *m_pPortsModel;
    QTimer m_movementTimer;
    QTimer m_progressTimer;
    joystickModule *m_pJoystickModule;
    motorsModule *m_pMotorsModule;
    QObject *m_pRootItem;
    bool m_motorsMoving;
    bool m_isOnStartPosition;
    int m_controllerAddress;

private slots:
    void initController();
    void programProgressRequest();
    void controllerConnected();
    void startProgramClicked();
    void stopProgramClicked();
    void pauseProgramClicked();
    void goToProgramStartClicked();
    void allStopClicked();
    void portsRescanClicked();
    void checkMotorAttachmentClicked();
    void setCameraEnabled(bool enable);
    void motorsStatusFinished(const QByteArray &data);
    void movementCheckRequest();
    void movementCheckFinished(const QByteArray &data);
    void connectToPortClicked(const QString &portName, const QString &address);
    void programProgressFinished(const QByteArray &data);
    void powerSaveStatusFinished(const QByteArray &data);
    void assignAddressRequest(const QString &portName, int address);
    void closePort();
};

#endif // MAINWINDOW_H
