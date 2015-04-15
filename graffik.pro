TEMPLATE = app

QT += qml quick widgets serialport

SOURCES += main.cpp \
    motionController.cpp \
    mainWindow.cpp \
    joystickModule.cpp \
    motorsModule.cpp \
    models/listModel.cpp \
    models/comPortItem.cpp \
    filter.cpp

RESOURCES += qml.qrc

macx {
    ICON = graffikIcon.icns
} win32 {
    RC_FILE = graffikIcon.rc
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    motionController.h \
    mainWindow.h \
    joystickModule.h \
    motorsModule.h \
    models/listModel.h \
    models/comPortItem.h \
    filter.h
