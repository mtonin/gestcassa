#include "singleprocess.h"
#include <QMessageBox>
#include <QThread>
#include <windows.h>

BOOL CALLBACK EnumWindowsProc(HWND hWnd, long lParam) {

    DWORD pid;
    GetWindowThreadProcessId(hWnd,&pid);
    if(pid==lParam) {
        /*
        TCHAR titolo[255];
        GetWindowText(hWnd,titolo,255);
        QString titoloString=QString::fromWCharArray(titolo);
        if(titoloString.toUpper().startsWith("TTK")) {
            qDebug(titoloString.toLatin1());
            return TRUE;
        }
        */
        qDebug("ok");
        SetForegroundWindow(hWnd);
        //ShowWindow(hWnd,SW_RESTORE);
        //PostMessage(hWnd,WM_SHOWWINDOW,TRUE,NULL);
        return FALSE;
    }
    return TRUE;
}

SingleProcess::SingleProcess(QObject* parent):QProcess(parent) {
}
SingleProcess::~SingleProcess(){
    close();
}

void SingleProcess::esegue(QString comando) {

    if(QProcess::Running==state()) {
        kill();
        waitForFinished(1000);
    }
    QStringList tokens=comando.split(" ");
    QString cmd=tokens[0];
    tokens.removeFirst();
    start(cmd,tokens);
    /*

    if(QProcess::Running!=state()) {
        QStringList tokens=comando.split(" ");
        QString cmd=tokens[0];
        tokens.removeFirst();
        start(cmd,tokens);
        processo->waitForFinished();
        QProcess::ExitStatus stato=processo->exitStatus();
        //if(QProcess::ExitStatus::NormalExit!=stato) {
            QString errore=processo->errorString();
            QMessageBox::critical(this,"Errore",errore);
        //}
        processo=nullptr;
    } else {
        EnumWindows(&EnumWindowsProc,pid()->dwProcessId);
    }
        */

}
