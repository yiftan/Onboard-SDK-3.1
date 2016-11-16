#ifndef POWERLINEPATROL_H
#define POWERLINEPATROL_H

#include "QonboardSDK.h"

namespace DJI
{
namespace onboardSDK
{

class PowerLinePatrol
{
public:
    PowerLinePatrol();

    void init(WayPointInitData *Info);
    void setInfo(const WayPointInitData &value);
    void setIndex(WayPointData *value, size_t pos);
    bool uploadIndexData(uint8_t pos);
    bool uploadIndexData(WayPointData *data);
    void startMission();
    PositionData nextPosition();
    WayPointInitData getInfo() const;
    bool Missionclicked;
    bool abortMission;
    bool isRunning;
private:
    WayPointInitData info;
    WayPointData *index;
    int posindex;


};

} //namespace onboardSDK
} //namespace DJI

#endif // POWERLINEPATROL_H
