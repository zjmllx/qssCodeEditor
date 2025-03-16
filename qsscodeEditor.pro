QT       += core gui
RC_ICONS = app.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    config.cpp \
    customDialog/myColorDialog/huesaturationselector.cpp \
    customDialog/myColorDialog/mycolordialog.cpp \
    customDialog/myColorDialog/mylineedit.cpp \
    customDialog/myColorDialog/myvslider.cpp \
    customDialog/myCustomStyle/mycustomstyle.cpp \
    interfaceUi/aboutus.cpp \
    interfaceUi/buttongroup.cpp \
    interfaceUi/inputgroup.cpp \
    interfaceUi/texteditgroup.cpp \
    interfaceUi/useguide.cpp \
    interfaceUi/viewgroup.cpp \
    interfaceUi/app.cpp \
    myCustomControl/myCustomBtn/jsonparse.cpp \
    myCustomControl/myCustomBtn/mypushbutton.cpp \
    myEditor/codeeditor.cpp \
    myEditor/completer.cpp \
    myEditor/highlighter.cpp \
    myEditor/linenumberarea.cpp \
    myMenuBar/findwidget.cpp \
    myMenuBar/mytolbtn.cpp \
    myMenuBar/topbar.cpp \
    main.cpp \

HEADERS += \
    config.h \
    customDialog/myColorDialog/huesaturationselector.h \
    customDialog/myColorDialog/mycolordialog.h \
    customDialog/myColorDialog/mylineedit.h \
    customDialog/myColorDialog/myvslider.h \
    customDialog/myCustomStyle/mycustomstyle.h \
    interfaceUi/aboutus.h \
    interfaceUi/buttongroup.h \
    interfaceUi/inputgroup.h \
    interfaceUi/texteditgroup.h \
    interfaceUi/useguide.h \
    interfaceUi/viewgroup.h \
    interfaceUi/app.h \
    myCustomControl/myCustomBtn/jsonparse.h \
    myCustomControl/myCustomBtn/mypushbutton.h \
    myEditor/codeeditor.h \
    myEditor/completer.h \
    myEditor/highlighter.h \
    myEditor/linenumberarea.h \
    myMenuBar/findwidget.h \
    myMenuBar/mytolbtn.h \
    myMenuBar/topbar.h \

FORMS += \
    customDialog/myColorDialog/mycolordialog.ui \
    customDialog/myCustomStyle/mycustomstyle.ui \
    interfaceUi/aboutus.ui \
    interfaceUi/buttongroup.ui \
    interfaceUi/inputgroup.ui \
    interfaceUi/texteditgroup.ui \
    interfaceUi/useguide.ui \
    interfaceUi/viewgroup.ui \


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    config.qrc \
    images.qrc \
    systemStyle.qrc


