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
    repartibox.cpp \
    arealavoro.cpp \
    repartobutton.cpp \
    dettagliorepartodlg.cpp \
    comandibox.cpp

HEADERS  += mainwindow.h \
    form01.h \
    ordine.h \
    repartibox.h \
    arealavoro.h \
    repartobutton.h \
    dettagliorepartodlg.h \
    comandibox.h

FORMS    += mainwindow.ui \
    form01.ui \
    ordine.ui \
    repartibox.ui \
    arealavoro.ui \
    repartobutton.ui \
    dettagliorepartodlg.ui \
    comandibox.ui

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
