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

namespace DJI
{
namespace onboardSDK
{

class PowerLinePatrol : public QThread
{
public:
    PowerLinePatrol();


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
    bool isRunning;
    bool isStart;
    bool isObtainControl;
    bool isLoadWayPoint;
    bool isTurnOff;
    bool isLanding;
    bool isUsingGPRSData;
    bool isGoHome;
    bool isFinished;
    bool stopped;
    int plpstatus;
    float setSpeed;
    void plpMission(CoreAPI *api, Flight *flight);
    void localOffsetFromGpsOffset(DJI::Vector3dData& deltaNed, PositionData* target, PositionData* origin);
    int moveByPositionOffset(float32_t xOffsetDesired, float32_t yOffsetDesired, float32_t zOffsetDesired, float32_t yawDesired,
                             int timeoutInMs=60000, float yawThresholdInDeg=0.5, float posThresholdInCm=30.0);
    int moveByPositionBodyFrame(PositionData* targetPosition,int timeoutInMs=60000, float yawThresholdInDeg=0.5, float posThresholdInCm=30.0);
    int moveByYawRate(float32_t yawDesired, float32_t zDesired,
                      int timeoutInMs=6000, float yawThresholdIndeg=0.5, float posDesiredInCm=30);
    int moveBySpeedBodyFrame(PositionData* targetPosition,
                             int timeoutInMs=60000, float yawThresholdInDeg=0.5, float posThresholdInCm=30);
    void sleepmSec(int mSec);
private:
    WayPointInitData info;
    WayPointData *index;
    int posindex;
    PositionData curpos;
    CoreAPI *api;
    Flight *flight;
protected:
    void run();

};

} //namespace onboardSDK
} //namespace DJI

#endif // POWERLINEPATROL_H
