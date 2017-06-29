#-------------------------------------------------
#
# Project created by QtCreator 2012-10-22T12:41:44
#
#-------------------------------------------------

include(qtsingleapplication/qtsingleapplication.pri)
system(prebuild.cmd)

QT       += widgets sql network printsupport

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
    statsmodel.cpp \
    confermadlg.cpp \
    dbmanager.cpp \
    storicoordini.cpp \
    storicoordinimodel.cpp \
    operazionidlg.cpp \
    storicoarticoliordinimodel.cpp \
    basemsgbox.cpp \
    parametriavanzati.cpp \
    buonidlg.cpp \
    buoniquerymodel.cpp \
    aboutdlg.cpp \
    dbparamdlg.cpp \
    backgroundcontroller.cpp \
    messaggiodlg.cpp \
    singleprocess.cpp

HEADERS  += mainwindow.h \
    ordine.h \
    repartobtnwidget.h \
    QPictureButton.h \
    dettaglireparto.h \
    commons.h \
    dettagliarticolo.h \
    articolobtnwidget.h \
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
    statsmodel.h \
    confermadlg.h \
    dbmanager.h \
    storicoordini.h \
    storicoordinimodel.h \
    operazionidlg.h \
    storicoarticoliordinimodel.h \
    basemsgbox.h \
    parametriavanzati.h \
    buonidlg.h \
    buoniquerymodel.h \
    aboutdlg.h \
    dbparamdlg.h \
    backgroundcontroller.h \
    messaggiodlg.h \
    singleprocess.h

FORMS    += mainwindow.ui \
    ordine.ui \
    dettaglireparto.ui \
    dettagliarticolo.ui \
    controlliordine.ui \
    restodlg.ui \
    configurazionedlg.ui \
    destinazionidlg.ui \
    reportform.ui \
    infowidget.ui \
    statsform.ui \
    confermadlg.ui \
    storicoordini.ui \
    operazionidlg.ui \
    parametriavanzati.ui \
    buonidlg.ui \
    aboutdlg.ui \
    dbparamdlg.ui \
    messaggiodlg.ui

#include(version.txt)

QMAKE_TARGET_COMPANY =
QMAKE_TARGET_PRODUCT = GestCassa
QMAKE_TARGET_DESCRIPTION = Gestione cassa
QMAKE_TARGET_COPYRIGHT = copyright MT 2013 - 2014

RESOURCES += \
    risorse.qrc
RC_FILE = gestcassa.rc
OTHER_FILES += \
    gestcassa.rc version.rc2.tmpl

LIBS += -L"$$PWD/vld"

LIBS +=user32.lib
