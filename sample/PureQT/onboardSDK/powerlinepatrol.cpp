#include "powerlinepatrol.h"
using namespace DJI::onboardSDK;

PowerLinePatrol::PowerLinePatrol()
{
    Missionclicked=false;
    abortMission=false;
    isRunning=false;
    isObtainControl=false;
    isLoadWayPoint=false;
    isTurnOff=false;
    isLanding=true;
    isUsingGPRSData=false;
}
void PowerLinePatrol::init(WayPointInitData *Info)
{
    if(Info)
        setInfo(*Info);
}
void PowerLinePatrol::setInfo(const WayPointInitData &value)
{
    //! @todo set information for way point
    info = value;
    for (int i = 0; i < 16; ++i) info.reserved[i] = 0;
    if (index != 0)
        delete index;
    index = 0;
}

WayPointInitData PowerLinePatrol::getInfo() const
{
    return info;
}

void PowerLinePatrol::setIndex(WayPointData *value, size_t pos)
{
  if (index == 0)
  {
    index = new WayPointData[info.indexNumber];
    if (index == NULL)
    {
      //API_LOG(api->getDriver(), ERROR_LOG, "Lack of memory\n");
      return;
    }
  }
  index[pos] = *value;
  for (int i = 0; i < 8; ++i) index[pos].reserved[i] = 0;
}
bool PowerLinePatrol::uploadIndexData(WayPointData *data)
{
    setIndex(data, data->index);
    return uploadIndexData(data->index);
}
bool PowerLinePatrol::uploadIndexData(uint8_t pos)
{
    WayPointData send;
    if (pos < info.indexNumber)
      send = index[pos];
    else
      return false; //! @note range error

    return true;
}
void PowerLinePatrol::startMission()
{
    posindex=0;
}

PositionData PowerLinePatrol::nextPosition()
{
    PositionData pos;
    if(posindex >= info.indexNumber)
    {
        pos.health=0;
    }
    else
    {
        pos.latitude=index[posindex].latitude;
        pos.longitude=index[posindex].longitude;
        pos.height=index[posindex].altitude;
        pos.health=posindex+1;
        posindex++;
    }
    return pos;
}
