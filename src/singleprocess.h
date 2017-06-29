#ifndef SINGLEPROCESS_H
#define SINGLEPROCESS_H
#include <QProcess>

class SingleProcess : public QProcess
{
public:
    SingleProcess(QObject* parent);
    void esegue(QString comando);
    ~SingleProcess();
};

#endif // SINGLEPROCESS_H
