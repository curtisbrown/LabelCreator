#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "control.h"

#include "cameraproperty.h"
#include "videostreaming.h"
#include "uvccamera.h"
#include "seecam_81.h"
#include "common.h"
#include "common_enums.h"

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    qmlRegisterType<Control>("com.ctdi.labelcreator.control", 1, 0, "Control");

    qmlRegisterType<Cameraproperty>("econ.camera.property",1,0,"Camproperty");
    qmlRegisterType<Videostreaming>("econ.camera.stream", 1, 0, "Videostreaming");
    qmlRegisterType<See3CAM_81>("econ.camera.see3cam81", 1, 0, "See3Cam81");
    qmlRegisterType<See3CAM_GPIOControl>("econ.camera.see3camGpioControl", 1, 0, "See3CamGpio");
    qmlRegisterType<CommonEnums>("cameraenum", 1, 0, "CommonEnums");

    Control control;
    Cameraproperty camProperty;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("control", &control);
    engine.rootContext()->setContextProperty("camModels", &camProperty.modelCam);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
