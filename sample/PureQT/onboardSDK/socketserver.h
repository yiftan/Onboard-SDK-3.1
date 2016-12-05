#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include "powerlinepatrol.h"

class socketServer : public QThread
{
    Q_OBJECT
public:
    socketServer();

    bool stopped;
    void stop();
signals:
    void emitMalfunction(const QString &mal);
protected:
    void run();
};

#endif // SOCKETSERVER_H
