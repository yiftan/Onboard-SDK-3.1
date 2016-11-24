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
#include <Windows.h>
#include <WinBase.h>

namespace DJI
{
namespace onboardSDK
{

class PowerLinePatrol : public QThread
{
public:
    PowerLinePatrol(CoreAPI *api, Flight *flight);
	~PowerLinePatrol();

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
	void enter();
    void leave();
    int set_event();
    int wait_event();
    bool stopped;
    int plpstatus;
    float setSpeed;
    float setheight;
    void plpMission();
    void localOffsetFromGpsOffset(DJI::Vector3dData& deltaNed, PositionData* target, PositionData* origin);
    int moveByPositionOffset(float32_t xOffsetDesired, float32_t yOffsetDesired, float32_t zOffsetDesired, float32_t yawDesired,
                             int timeoutInMs=60000, float yawThresholdInDeg=0.5, float posThresholdInCm=30.0);
    int moveByPositionBodyFrame(PositionData* targetPosition,int timeoutInMs=60000, float yawThresholdInDeg=0.5, float posThresholdInCm=30.0);
    int moveByYawRate(float32_t yawDesired, float32_t zDesired,
                      int timeoutInMs=6000, float yawThresholdIndeg=0.5, float posDesiredInCm=30);
    int moveBySpeedBodyFrame(PositionData* targetPosition,
                             int timeoutInMs=60000, float yawThresholdInDeg=0.5, float posThresholdInCm=30);
private:
    WayPointInitData info;
    WayPointData *index;
    int posindex;
    PositionData curpos;
    CoreAPI *api;
    Flight *flight;
	CRITICAL_SECTION  m_critical_section;
    HANDLE      m_pipe_read;
    HANDLE      m_pipe_write;
protected:
    void run();

};

} //namespace onboardSDK
} //namespace DJI

#endif // POWERLINEPATROL_H
