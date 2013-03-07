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
    arealavoro.cpp \
    QPictureButton.cpp \
    dettaglireparto.cpp \
    dettagliarticolo.cpp \
    articolobtnwidget.cpp \
    dbdialog.cpp \
    elementobtnwidget.cpp \
    dettaglioelementodlg.cpp

HEADERS  += mainwindow.h \
    form01.h \
    ordine.h \
    dettagliorepartodlg.h \
    comandibox.h \
    repartobtnwidget.h \
    arealavoro.h \
    QPictureButton.h \
    dettaglireparto.h \
    commons.h \
    dettagliarticolo.h \
    articolobtnwidget.h \
    dbdialog.h \
    elementobtnwidget.h \
    dettaglioelementodlg.h

FORMS    += mainwindow.ui \
    form01.ui \
    ordine.ui \
    dettagliorepartodlg.ui \
    comandibox.ui \
    arealavoro.ui \
    dettaglireparto.ui \
    dettagliarticolo.ui \
    dbdialog.ui \
    dettaglioelementodlg.ui

include(version.txt)

QMAKE_TARGET_COMPANY =
QMAKE_TARGET_PRODUCT = GestCassa
QMAKE_TARGET_DESCRIPTION = Gestione cassa
QMAKE_TARGET_COPYRIGHT = copyright MT 2012

RESOURCES += \
    risorse.qrc
RC_FILE = gestcassa.rc
OTHER_FILES += \
    gestcassa.rc
