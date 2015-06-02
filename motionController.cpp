#include "motionController.h"
#include <QDebug>

QDebug operator << (QDebug d, const commandRequest &r) {
  d<<"processing| address:"<<r.address<<"subAddress:"<<r.subAddress<<"command:"<<r.command<<"data:"<<QByteArray((const char*)r.data, r.dataLength).toHex();
  return d;
}

motionController &motionControllerInstance() {
  static motionController mc;
  return mc;
}

motionController::motionController(QObject *parent) :
  QObject(parent)
{
  m_serialPort.setSettingsRestoredOnClose(false);
  m_deviceAddress = 3;
  m_joystickMode = false;
  m_blocked = false;
  m_connectionTimer.setInterval(5000);
  m_timer.setInterval(40);
  m_timer.start();
}

bool motionController::openPort(const QString &portName) {
  qDebug()<<"opening port";
#ifdef Q_OS_MAC
  m_serialPort.setPortName(QLatin1String("/dev/tty.") + portName);
#else
  m_serialPort.setPortName(portName);
#endif
  m_portName = portName;
  if(m_serialPort.open(QSerialPort::ReadWrite)) {
      connect(&m_timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
      connect(&m_connectionTimer, SIGNAL(timeout()), this, SLOT(connectionError()));
      connect(&m_serialPort, SIGNAL(readyRead()), this, SLOT(serialPortReadyRead()));
      connect(&m_serialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(serialPortError(QSerialPort::SerialPortError)));
      return true;
    }

  return false;
}

void motionController::clearQueue() {
  m_requestsQueue.clear();
}

void motionController::requestClosePort() {
  commandRequest cr;
  cr.address = 0;
  cr.subAddress = 0;
  cr.command = 0;

  QByteArray buff = "__close__";
  cr.dataLength = buff.size();
  qstrcpy((char*)cr.data, (char*)buff.data());

  m_requestsQueue.enqueue(cr);
}

void motionController::setAction(const QString &id) {
  commandRequest cr;
  cr.address = 0;
  cr.subAddress = 0;
  cr.command = 0;
  cr.dataLength = id.size();
  qstrcpy((char*)cr.data, (char*)id.toLatin1().data());

  m_requestsQueue.enqueue(cr);
}

void motionController::clearPort() {
  m_serialPort.clear();
}

void motionController::closePort() {
  m_serialPort.close();
}

void motionController::testController(bool blocking) {
  commandRequest cr;
  cr.address = 1;
  cr.subAddress = 0;
  cr.command = 5;
  cr.dataLength = 0;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::assignAddress(unsigned char address, bool blocking) {
  commandRequest cr;
  cr.address = 1;
  cr.subAddress = 0;
  cr.command = 4;
  cr.dataLength = 1;
  cr.data[0] = address;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::setGraffikModeEnable(bool enable, bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 0;
  cr.command = 50;
  cr.dataLength = 1;
  cr.data[0] = enable ? 1 : 0;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::setMotorEnable(unsigned char motor, bool enable, bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = motor;
  cr.command = 3;
  cr.dataLength = 1;
  cr.data[0] = enable ? 1 : 0;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
  m_motorsInfo[m_deviceAddress][motor - 1].enable = enable;
  if(!enable) m_motorsInfo[m_deviceAddress][motor - 1].moving = false;
}

void motionController::setCameraEnable(unsigned char address, bool enable, bool blocking) {
  commandRequest cr;
  cr.address = address;
  cr.subAddress = 4;
  cr.command = 2;
  cr.dataLength = 1;
  cr.data[0] = enable ? 1 : 0;
  cr.blocking = blocking;
  qDebug()<<"set camera enable:"<<enable;

  m_requestsQueue.enqueue(cr);
}

void motionController::setCameraEnable(bool enable, bool blocking) {
  setCameraEnable(m_deviceAddress, enable, blocking);
}

void motionController::setFocusWithShutter(bool enable, bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 4;
  cr.command = 8;
  cr.dataLength = 1;
  cr.data[0] = enable ? 1 : 0;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::setWatchdogEnable(bool enable, bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 0;
  cr.command = 14;
  cr.dataLength = 1;
  cr.data[0] = enable ? 1 : 0;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::setShotsInterval(unsigned interval, bool blocking) {
  if(m_programInfo.interval == interval)
    return;

  qDebug()<<"setting interval:"<<interval;
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 4;
  cr.command = 10;
  cr.dataLength = 4;
  cr.blocking = blocking;
  m_programInfo.interval = interval;

  //reorder bytes
  std::reverse_copy((char*)&interval, (char*)&interval + 4, cr.data);

  m_requestsQueue.enqueue(cr);
}

void motionController::setMotorAcceleration(unsigned char motor, float value, bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = motor;
  cr.command = 14;
  cr.dataLength = 4;
  cr.blocking = blocking;

  //reorder bytes
  std::reverse_copy((char*)&value, (char*)&value + 4, cr.data);

  m_requestsQueue.enqueue(cr);
}

void motionController::setLeadInShots(unsigned char motor, unsigned shots, bool blocking) {
  if(m_motorsInfo[m_deviceAddress][motor - 1].leadInShots == shots)
    return;

  qDebug()<<"setting lead in shots, motor:"<<motor<<"value:"<<shots;
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = motor;
  cr.command = 19;
  cr.dataLength = 4;
  cr.blocking = blocking;
  m_motorsInfo[m_deviceAddress][motor - 1].leadInShots = shots;

  //reorder bytes
  std::reverse_copy((char*)&shots, (char*)&shots + 4, cr.data);

  m_requestsQueue.enqueue(cr);
}

void motionController::setLeadOutShots(unsigned char motor, unsigned shots, bool blocking) {
  if(m_motorsInfo[m_deviceAddress][motor - 1].leadOutShots == shots)
    return;

  qDebug()<<"setting lead out shots, motor:"<<motor<<"value:"<<shots;
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = motor;
  cr.command = 25;
  cr.dataLength = 4;
  cr.blocking = blocking;
  m_motorsInfo[m_deviceAddress][motor - 1].leadOutShots = shots;

  //reorder bytes
  std::reverse_copy((char*)&shots, (char*)&shots + 4, cr.data);

  m_requestsQueue.enqueue(cr);
}

void motionController::setExposureDelay(unsigned short delay, bool blocking) {
  if(m_programInfo.exposureDelay == delay)
    return;

  qDebug()<<"setting exposure delay:"<<delay;
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 4;
  cr.command = 7;
  cr.dataLength = 2;
  cr.blocking = blocking;
  m_programInfo.exposureDelay = delay;

  //reorder bytes
  std::reverse_copy((char*)&delay, (char*)&delay + 2, cr.data);

  m_requestsQueue.enqueue(cr);
}

void motionController::setMaxShots(unsigned short shots, bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 4;
  cr.command = 6;
  cr.dataLength = 2;
  cr.blocking = blocking;

  //reorder bytes
  std::reverse_copy((char*)&shots, (char*)&shots + 2, cr.data);

  m_requestsQueue.enqueue(cr);
}

void motionController::setExposureTime(unsigned time, bool blocking) {
  if(m_programInfo.exposure == time)
    return;

  qDebug()<<"setting exposure time:"<<time;
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 4;
  cr.command = 4;
  cr.dataLength = 4;
  cr.blocking = blocking;
  m_programInfo.exposure = time;

  //reorder bytes
  std::reverse_copy((char*)&time, (char*)&time + 4, cr.data);

  m_requestsQueue.enqueue(cr);
}

void motionController::setBacklash(unsigned char address, unsigned char motor, unsigned short value, bool blocking) {
  if(m_programInfo.backlash == value)
    return;

  qDebug()<<"setting backlash value:"<<value;
  commandRequest cr;
  cr.address = address;
  cr.subAddress = motor;
  cr.command = 5;
  cr.dataLength = 2;
  cr.blocking = blocking;
  m_programInfo.backlash = value;

  //reorder bytes
  std::reverse_copy((char*)&value, (char*)&value + 2, cr.data);

  m_requestsQueue.enqueue(cr);
}

void motionController::setBacklash(unsigned char motor, unsigned short value, bool blocking) {
  setBacklash(m_deviceAddress, motor, value, blocking);
}

void motionController::setFocusTime(unsigned short time, bool blocking) {
  if(m_programInfo.focusTime == time)
    return;

  qDebug()<<"setting focus time:"<<time;
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 4;
  cr.command = 5;
  cr.dataLength = 2;
  cr.blocking = blocking;
  m_programInfo.focusTime = time;

  //reorder bytes
  std::reverse_copy((char*)&time, (char*)&time + 2, cr.data);

  m_requestsQueue.enqueue(cr);
}

void motionController::expose(unsigned time, bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 4;
  cr.command = 3;
  cr.dataLength = 4;
  cr.blocking = blocking;

  //reorder bytes
  std::reverse_copy((char*)&time, (char*)&time + 4, cr.data);

  m_requestsQueue.enqueue(cr);
}

void motionController::setContinuousSpeed(unsigned char motor, float stepsPerSecs, bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = motor;
  cr.command = 13;
  cr.dataLength = 4;
  cr.blocking = blocking;

  //reorder bytes
  stepsPerSecs *= m_motorsInfo[m_deviceAddress][motor - 1].invertDirection ? -1 : 1;
  std::reverse_copy((char*)&stepsPerSecs, (char*)&stepsPerSecs + 4, cr.data);

  m_requestsQueue.enqueue(cr);
}

void motionController::setMicroStepValue(unsigned char address, unsigned char motor,
                                         unsigned char value, bool blocking)
{
  qDebug()<<"setting microstep value, motor:"<<motor<<"value:"<<value;

  commandRequest cr;
  cr.address = address;
  cr.subAddress = motor;
  cr.command = 6;
  cr.dataLength = 1;
  cr.data[0] = value;
  cr.blocking = blocking;
  m_motorsInfo[address][motor - 1].microstep = value;

  m_requestsQueue.enqueue(cr);
}

void motionController::setMicroStepValue(unsigned char motor, unsigned char value, bool blocking) {
  setMicroStepValue(m_deviceAddress, motor, value, blocking);
}

void motionController::setProgramAcceleration(unsigned char motor, unsigned value, bool blocking) {
  if(m_motorsInfo[m_deviceAddress][motor - 1].acceleration == value)
    return;

  qDebug()<<"setting acceleration, motor:"<<motor<<"value:"<<value;
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = motor;
  cr.command = 21;
  cr.dataLength = 4;
  cr.blocking = blocking;
  m_motorsInfo[m_deviceAddress][motor - 1].acceleration = value;

  //reorder bytes
  std::reverse_copy((char*)&value, (char*)&value + 4, cr.data);

  m_requestsQueue.enqueue(cr);
}

void motionController::setProgramDeceleration(unsigned char motor, unsigned value, bool blocking) {
  if(m_motorsInfo[m_deviceAddress][motor - 1].deceleration == value)
    return;

  qDebug()<<"setting deceleration, motor:"<<motor<<"value:"<<value;
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = motor;
  cr.command = 22;
  cr.dataLength = 4;
  cr.blocking = blocking;
  m_motorsInfo[m_deviceAddress][motor - 1].deceleration = value;

  //reorder bytes
  std::reverse_copy((char*)&value, (char*)&value + 4, cr.data);

  m_requestsQueue.enqueue(cr);
}

void motionController::setTravelTime(unsigned char motor, unsigned time, bool blocking) {
  if(m_motorsInfo[m_deviceAddress][motor - 1].travelTime == time)
    return;

  qDebug()<<"setting travel time, motor:"<<motor<<"value:"<<time;
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = motor;
  cr.command = 20;
  cr.dataLength = 4;
  cr.blocking = blocking;
  m_motorsInfo[m_deviceAddress][motor - 1].travelTime = time;

  //reorder bytes
  std::reverse_copy((char*)&time, (char*)&time + 4, cr.data);

  m_requestsQueue.enqueue(cr);
}

void motionController::setProgramMode(unsigned char mode, bool blocking) {
  if(m_programInfo.programMode == mode)
    return;

  qDebug()<<"setting program mode:"<<mode;
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 0;
  cr.command = 22;
  cr.dataLength = 1;
  cr.data[0] = mode;
  cr.blocking = blocking;
  m_programInfo.programMode = mode;

  m_requestsQueue.enqueue(cr);
}

void motionController::setMaxStepSpeed(unsigned char motor, unsigned short speed, bool blocking) {
  if(m_motorsInfo[m_deviceAddress][motor - 1].maxStepSpeed == speed)
    return;

  qDebug()<<"setting max step speed, motor:"<<motor<<"value:"<<speed;
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = motor;
  cr.command = 7;
  cr.dataLength = 2;
  cr.blocking = blocking;
  m_motorsInfo[m_deviceAddress][motor - 1].maxStepSpeed = speed;

  //reorder bytes
  std::reverse_copy((char*)&speed, (char*)&speed + 2, cr.data);

  m_requestsQueue.enqueue(cr);
}

void motionController::setDirection(unsigned char motor, unsigned char direction, bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = motor;
  cr.command = 8;
  cr.dataLength = 1;
  cr.data[0] = direction;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
  m_motorsInfo[m_deviceAddress][motor - 1].direction = direction;
}

void motionController::setMotorSleep(unsigned char address, unsigned char motor, bool sleep, bool blocking) {
  commandRequest cr;
  cr.address = address;
  cr.subAddress = motor;
  cr.command = 2;
  cr.dataLength = 1;
  cr.data[0] = sleep ? 1 : 0;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::moveMotor(unsigned char motor, unsigned char direction, unsigned steps, bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = motor;
  cr.command = 15;
  cr.dataLength = 5;
  cr.data[0] = direction;
  cr.blocking = blocking;

  std::reverse_copy((char*)&steps, (char*)&steps + 4, &cr.data[1]);

  m_requestsQueue.enqueue(cr);
  m_motorsInfo[m_deviceAddress][motor - 1].direction = direction;
  m_motorsInfo[m_deviceAddress][motor - 1].moving = true;
}

void motionController::motorPosition(unsigned char motor, bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = motor;
  cr.command = 106;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
  m_motorsInfo[m_deviceAddress][motor - 1].moving = false;
}

void motionController::stopMotor(unsigned char motor, bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = motor;
  cr.command = 4;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
  m_motorsInfo[m_deviceAddress][motor - 1].moving = false;
}

void motionController::setEasingMode(unsigned char address, unsigned char motor, unsigned char mode, bool blocking) {
  commandRequest cr;
  cr.address = address;
  cr.subAddress = motor;
  cr.command = 18;
  cr.dataLength = 1;
  cr.data[0] = mode;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::setEasingMode(unsigned char motor, unsigned char mode, bool blocking) {
  setEasingMode(m_deviceAddress, motor, mode, blocking);
}

void motionController::setProgramStartPoint(unsigned char motor, unsigned stepPosition, bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = motor;
  cr.command = 16;
  cr.dataLength = 4;
  cr.blocking = blocking;

  std::reverse_copy((char*)&stepPosition, (char*)&stepPosition + 4, cr.data);

  m_requestsQueue.enqueue(cr);
}

void motionController::setProgramStartPoint(bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 0;
  cr.command = 26;
  cr.dataLength = 0;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::setProgramStopPoint(unsigned char motor, unsigned stepPosition, bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = motor;
  cr.command = 17;
  cr.dataLength = 4;
  cr.blocking = blocking;

  std::reverse_copy((char*)&stepPosition, (char*)&stepPosition + 4, cr.data);

  m_requestsQueue.enqueue(cr);
}

void motionController::setProgramStopPoint(bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 0;
  cr.command = 27;
  cr.dataLength = 0;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::sendMotorToStartPoint(unsigned char motor, bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = motor;
  cr.command = 23;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::sendMotorToStopPoint(unsigned char motor, bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = motor;
  cr.command = 24;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::setPingPongFlag(bool enable, bool blocking) {
  if(m_programInfo.pingPongFlag == enable)
    return;

  qDebug()<<"setting Ping-Pong flag:"<<enable;
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 0;
  cr.command = 24;
  cr.dataLength = 1;
  cr.data[0] = enable ? 1 : 0;
  cr.blocking = blocking;
  m_programInfo.pingPongFlag = enable;

  m_requestsQueue.enqueue(cr);
}

void motionController::setJoystickMode(bool enable, bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 0;
  cr.command = 23;
  cr.dataLength = 1;
  cr.data[0] = enable ? 1 : 0;
  cr.blocking = blocking;
  m_joystickMode = enable;

  m_requestsQueue.enqueue(cr);
}

void motionController::startPlannedMove(bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 0;
  cr.command = 2;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::stopPlannedMove(bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 0;
  cr.command = 4;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::pausePlannedMove(bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 0;
  cr.command = 3;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::firmwareVersion(bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 0;
  cr.command = 100;
  cr.blocking = blocking;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::shotsInterval(bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 4;
  cr.command = 108;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::motorsStatus(unsigned char address, bool blocking) {
  commandRequest cr;
  cr.address = address;
  cr.subAddress = 0;
  cr.command = 124;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::motorsStatus(bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 0;
  cr.command = 124;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::motorRunning(unsigned char motor, bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = motor;
  cr.command = 107;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::motorsRunning(bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 0;
  cr.command = 128;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::setDeviceAddress(unsigned char address) {
  m_deviceAddress = address;
  qDebug()<<"set current controller address:"<<address;
}

void motionController::validateMotor(unsigned char motor, bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = motor;
  cr.command = 118;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::powerSaveStatus(bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 0;
  cr.command = 130;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::validateMotors(bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 0;
  cr.command = 129;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::programProgress(bool blocking) {
  commandRequest cr;
  cr.address = m_deviceAddress;
  cr.subAddress = 0;
  cr.command = 123;
  cr.blocking = blocking;

  m_requestsQueue.enqueue(cr);
}

void motionController::invertMotorDirection(unsigned char address, unsigned char motor, bool yes) {
  m_motorsInfo[address][motor - 1].invertDirection = yes;
}

void motionController::processCommands() {
  if(!m_requestsQueue.empty()) {
      commandRequest cr = m_requestsQueue.dequeue();
      if(cr.address == 0 && cr.subAddress == 0 &&
         cr.command == 0)
        {
          QByteArray buff((char*)cr.data, cr.dataLength);
          if(buff == "__close__") {
              m_blocked = false;
              m_repliesBuffer.clear();
              m_serialPort.close();
              //maybe lets not do that?
              m_requestsQueue.clear();
            } else {
              emit actionFinished(QString(buff));
            }

          return;
        }

      m_serialPort.setProperty("command", cr.command);
      m_serialPort.setProperty("subAddress", cr.subAddress);
      m_serialPort.setProperty("address", cr.address);
      m_blocked = cr.blocking;

      qDebug()<<cr;

      const char *pRequest = (const char*)&cr;
      m_serialPort.write(++pRequest, cr.size());
      m_serialPort.flush();
    }
}

void motionController::replyEmiter(unsigned char address, unsigned char subAddress,
                                   unsigned char command, const QByteArray &data)
{
  if(address == 1) {
      qDebug()<<"test controller finished";
      switch(command) {
        case 5: emit testControllerFinished(data); break;
      }
    } else if(subAddress == 0) {
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
          m_blocked = false;
          qDebug()<<"bad reply";
          return;
        }

      int dataSize = m_repliesBuffer[9]; //byte 10 cointains data size of reply
      if(dataSize + 10 <= m_repliesBuffer.size()) {
          QByteArray reply = m_repliesBuffer.mid(10, dataSize);

          unsigned char command = m_serialPort.property("command").toUInt();
          unsigned char subAddress = m_serialPort.property("subAddress").toUInt();
          unsigned char address = m_serialPort.property("address").toUInt();

          qDebug()<<"processed | address:"<<address<<"subAddress:"<<subAddress<<"command:"<<command<<"data:"<<m_repliesBuffer.left(10 + dataSize).toHex();
          replyEmiter(address, subAddress, command, reply);

          m_repliesBuffer.remove(0, 10 + dataSize);
          m_blocked = false;
          m_timer.start();
        }
    }
}

void motionController::serialPortError(QSerialPort::SerialPortError err) {
  if(err != QSerialPort::NoError) {
      qDebug()<<"serial port error:"<<err;
    }
}

void motionController::timerTimeout() {
  if(m_serialPort.isOpen() && !m_blocked)
    processCommands();
  //    else if(m_requestsQueue.count() > 0)
  //      qDebug()<<"shit:"<<m_requestsQueue.first();
}

void motionController::connectionError() {
  m_repliesBuffer.clear();
  m_serialPort.close();
  m_blocked = false;
  emit disconnected();
}


