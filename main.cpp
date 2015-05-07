#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "mainWindow.h"
#include "filter.h"
#include "popupWindow.h"
#include "motionController.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<popupWindow>("Graffik.Components", 1, 0, "PopupWindow");

    filter *eventFilter = new filter;
    app.installEventFilter(eventFilter);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("controller", &motionControllerInstance());
    engine.load(QUrl(QStringLiteral("qrc:/ui/main.qml")));

    QObject *root = engine.rootObjects().at(0);
    mainWindow mw(root);

    QObject::connect(eventFilter, SIGNAL(mousePressed(QVariant,QVariant,QVariant)), root, SLOT(mousePressed(QVariant,QVariant,QVariant)));
    QObject::connect(eventFilter, SIGNAL(mouseReleased(QVariant,QVariant,QVariant)), root, SLOT(mouseReleased(QVariant,QVariant,QVariant)));

    return app.exec();
}
