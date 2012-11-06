#-------------------------------------------------
#
# Project created by QtCreator 2012-10-22T12:41:44
#
#-------------------------------------------------

system(prebuild.cmd)

QT       += core gui sql

TARGET = GestCassa
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    form01.cpp \
    ordine.cpp \
    dettagliorepartodlg.cpp \
    comandibox.cpp \
    repartobtnwidget.cpp \
    arealavoro.cpp

HEADERS  += mainwindow.h \
    form01.h \
    ordine.h \
    dettagliorepartodlg.h \
    comandibox.h \
    repartobtnwidget.h \
    arealavoro.h

FORMS    += mainwindow.ui \
    form01.ui \
    ordine.ui \
    dettagliorepartodlg.ui \
    comandibox.ui \
    arealavoro.ui

include(version.txt)

QMAKE_TARGET_COMPANY =
QMAKE_TARGET_PRODUCT = GestCassa
QMAKE_TARGET_DESCRIPTION = Gestione cassa
QMAKE_TARGET_COPYRIGHT = copyright MT 2012

RESOURCES += \
    risorse.qrc
#RC_FILE = gestcassa.rc
OTHER_FILES += \
    gestcassa.rc
