#ifndef GPRS_H
#define GPRS_H

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
#include <QThread>

namespace Ui
{
class DJIonboardSDK;
}
namespace DJI
{
namespace onboardSDK
{

class GPRSSendMessage:public QThread
{
    Q_OBJECT
public:
    GPRSSendMessage(QtOnboardsdkPortDriver *GPRSdriver,QSerialPort *GPRSport,QTextBrowser *tb_GPRSDisplay);
    void GPRSDataSend();

    void stop();

    bool stopped;
protected:
    void run();
private:
    QtOnboardsdkPortDriver *GPRSdriver;
    QSerialPort *GPRSport;
    QString Message;
    QStringList GPRSSendBufData;
    QStringList GPRSSendBufHD;
    Ui::DJIonboardSDK *ui;
    QTextBrowser *text;
public slots:
    void GPRSSignalRev(const QString &s);
};

}
}
#endif // GPRS_H
