#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H
#include "QonboardSDK.h"
#include <QMainWindow>
#include <QAbstractButton>
#include <QTimer>
#include <QTime>
#include <QTextStream>
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <QList>
#include <QFile>
#include <QComboBox>
#include <QEventLoop>
#include <fstream>
#include "QonboardSDK.h"
#include <qDebug>
#include <QCoreApplication>
#include <winsock2.h>

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
