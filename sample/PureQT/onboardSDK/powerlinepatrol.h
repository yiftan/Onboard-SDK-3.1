#ifndef POWERLINEPATROL_H
#define POWERLINEPATROL_H

#include "QonboardSDK.h"

namespace DJI
{
namespace onboardSDK
{

class PowerLinePatrol //: public QThread
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
private:
    WayPointInitData info;
    WayPointData *index;
    int posindex;
/*protected:
    void run();*/

};

} //namespace onboardSDK
} //namespace DJI

#endif // POWERLINEPATROL_H
