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
    ordine.cpp \
    repartobtnwidget.cpp \
    QPictureButton.cpp \
    dettaglireparto.cpp \
    dettagliarticolo.cpp \
    articolobtnwidget.cpp \
    dbdialog.cpp \
    ordinemodel.cpp \
    controlliordine.cpp \
    restodlg.cpp \
    configurazionedlg.cpp \
    destinazionidlg.cpp \
    simplecrypt.cpp \
    reportform.cpp \
    textprinter.cpp \
    infowidget.cpp \
    QDigitalClock.cpp \
    qcustomplot.cpp \
    statsform.cpp \
    statsmodel.cpp

HEADERS  += mainwindow.h \
    ordine.h \
    repartobtnwidget.h \
    QPictureButton.h \
    dettaglireparto.h \
    commons.h \
    dettagliarticolo.h \
    articolobtnwidget.h \
    dbdialog.h \
    ordinemodel.h \
    controlliordine.h \
    restodlg.h \
    configurazionedlg.h \
    destinazionidlg.h \
    simplecrypt.h \
    reportform.h \
    textprinter.h \
    infowidget.h \
    QDigitalClock.h \
    qcustomplot.h \
    statsform.h \
    statsmodel.h

FORMS    += mainwindow.ui \
    ordine.ui \
    dettaglireparto.ui \
    dettagliarticolo.ui \
    dbdialog.ui \
    controlliordine.ui \
    restodlg.ui \
    configurazionedlg.ui \
    destinazionidlg.ui \
    reportform.ui \
    infowidget.ui \
    statsform.ui

include(version.txt)

QMAKE_TARGET_COMPANY =
QMAKE_TARGET_PRODUCT = GestCassa
QMAKE_TARGET_DESCRIPTION = Gestione cassa
QMAKE_TARGET_COPYRIGHT = copyright MT 2013

RESOURCES += \
    risorse.qrc
RC_FILE = gestcassa.rc
OTHER_FILES += \
    gestcassa.rc version.rc2.tmpl
