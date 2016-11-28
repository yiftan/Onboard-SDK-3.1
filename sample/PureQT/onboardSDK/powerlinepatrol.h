#ifndef POWERLINEPATROL_H
#define POWERLINEPATROL_H

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
#include <QCoreApplication>

namespace DJI
{
namespace onboardSDK
{

class PowerLinePatrol : public QThread
{
    Q_OBJECT
public:
    PowerLinePatrol(CoreAPI *api, Flight *flight);

    void init(WayPointInitData *Info);
    void setInfo(const WayPointInitData &value);
    void setIndex(WayPointData *value, size_t pos);
    bool uploadIndexData(uint8_t pos);
    bool uploadIndexData(WayPointData *data);
    void startMission();
    void stop();
    PositionData nextPosition();
    WayPointInitData getInfo() const;
    bool Missionclicked;
    bool abortMission;
    volatile bool isRunning;
    bool isStart;
    bool isObtainControl;
    bool isLoadWayPoint;
    bool isTurnOff;
    bool isLanding;
    bool isUsingGPRSData;
    bool isGoHome;
    bool isFinished;
    bool stopped;
    volatile int plpstatus;
    float setSpeed;
    float setheight;
    float goHomeSpeed;
    PositionData goHome;
    QMutex *statusMutex;
    void plpMission();
    void goHomeMission();
    void sleepmSec(int mSec);
    void localOffsetFromGpsOffset(DJI::Vector3dData& deltaNed, PositionData* target, PositionData* origin);
    int moveByPositionOffset(float32_t xOffsetDesired, float32_t yOffsetDesired, float32_t zOffsetDesired, float32_t yawDesired,
                             int timeoutInMs=60000, float yawThresholdInDeg=1, float posThresholdInCm=30.0);
    int moveByPositionBodyFrame(PositionData* targetPosition,int timeoutInMs=60000, float yawThresholdInDeg=0.5, float posThresholdInCm=30.0);
    int moveByYawRate(float32_t yawDesired, float32_t zDesired,
                      int timeoutInMs=6000, float yawThresholdIndeg=1, float posDesiredInCm=30);
    int moveBySpeedBodyFrame(PositionData* targetPosition,
                             int timeoutInMs=60000, float yawThresholdInDeg=1, float posThresholdInCm=30);
public slots:
    void abortSignalSlot(const QString &abortMission);
private:
    WayPointInitData info;
    WayPointData *index;
    int posindex;
    CoreAPI *api;
    Flight *flight;
    QString log;
protected:
    void run();
signals:
    void emitLog(const QString &log);

};

} //namespace onboardSDK
} //namespace DJI

#endif // POWERLINEPATROL_H
