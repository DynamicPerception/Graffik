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
    QString m_portName;
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
    void portsRescanClicked();
    void connectToDevices();
    void movementCheckRequest();
    void testControllerFinished(const QByteArray &data);
    void movementCheckFinished(const QByteArray &data);
    void connectToPortClicked(const QString &portName, const QString &address);
    void programProgressFinished(const QByteArray &data);
    void powerSaveStatusFinished(const QByteArray &data);
    void assignAddressRequest(const QString &portName, int address);
};

#endif // MAINWINDOW_H
