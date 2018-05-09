QT += quick multimedia widgets concurrent
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    control.cpp \
    utilities.cpp \
    cameracapture.cpp \
    labelprinter.cpp \
    cameraproperty.cpp \
    videostreaming.cpp \
    uvccamera.cpp \
    v4l2-api.cpp \
    videoencoder.cpp \
    h264decoder.cpp \
    seecam_81.cpp \
    imageprocessing.cpp

HEADERS += \
    control.h \
    utilities.h \
    cameracapture.h \
    labelprinter.h \
    cameraproperty.h \
    videostreaming.h \
    uvccamera.h \
    v4l2-api.h \
    videoencoder.h \
    h264decoder.h \
    seecam_81.h \
    common_enums.h \
    common.h \
    imageprocessing.h


RESOURCES += qml.qrc \
    images.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


INCLUDEPATH +=  $$PWD/v4l2headers/include \
                /usr/include \
                /usr/include/libusb-1.0



LIBS += -lv4l2 -lv4lconvert \
        -lavutil \
        -lavcodec \
        -lavformat \
        -lswscale \
        -ludev \
        -lusb-1.0 \
        -L/usr/lib/ -lturbojpeg
