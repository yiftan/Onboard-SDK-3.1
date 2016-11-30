#include "powerlinepatrol.h"
using namespace DJI::onboardSDK;

#include "DJI_utility.h"
#include "DJI_guidance.h"

#define   C_EARTH (double) 6378137.0
#define   DEG2RAD (double)0.01745329252
#define   RAD2DEG (double)57.29577951308

PowerLinePatrol::PowerLinePatrol(CoreAPI *api, Flight *flight)
{
    this->api=api;
    this->flight=flight;
    index=0;
    plpstatus=1;
    Missionclicked=false;
    abortMission=false;
    isRunning=false;
    isObtainControl=false;
    isLoadWayPoint=false;
    isTurnOff=false;
    isLanding=true;
    isUsingGPRSData=false;
    isGoHome=false;
    isFinished=false;
    stopped=false;
    isStart=false;
    isUsingGUID=false;
    setSpeed=2.0;
    setheight=2.0;
    avoidDistanceFront=2.0;
    avoidDistanceBottom=1.0;
    goHome.height=100;
    goHomeSpeed=15;
    goHome.latitude=30.265750643*DEG2RAD;
    goHome.longitude=120.11965452*DEG2RAD;
    statusMutex = new QMutex();
}

PowerLinePatrol::~PowerLinePatrol()
{

}

void PowerLinePatrol::stop()
{
    stopped=true;
}
void PowerLinePatrol::run()
{
    while(!stopped)
    {
        if(plpstatus==8)
        {
            isRunning=true;
            goHomeMission();
            isRunning=false;
        }
        else
        {
            //if(isRunning==false)
            {
                plpMission();
                if(plpstatus==7)
                {
                    log="4007";
                    emitLog(QString(log));
                    sprintf(DJI::onboardSDK::buffer, "%s","Auto go home after 20s...");
                    api->serialDevice->displayLog();
                    QTime dieTime = QTime::currentTime().addSecs(20);
                    while(QTime::currentTime() < dieTime)
                    {
                        if(abortMission)
                        {
                            break;
                        }
                    }
                    if(abortMission)
                    {
                        statusMutex->lock();
                        plpstatus=5;
                        statusMutex->unlock();
                        abortMission=false;
                        log="4008";
                        emitLog(log);
                        sprintf(DJI::onboardSDK::buffer, "%s","Auto go home aborted");
                        api->serialDevice->displayLog();
                    }
                    else
                    {
                        isRunning=true;
                        goHomeMission();
                        isRunning=false;
                    }
                }
                isRunning=false;
            }
        }
        stopped=true;
    }
    stopped=false;
    //qDebug()<<QString("PLP Thread stoped");
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
    {
        delete []index;
        index = NULL;
    }
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

void PowerLinePatrol::sleepmSec(int mSec)
{
    QTime dieTime = QTime::currentTime().addMSecs(mSec);
    while( QTime::currentTime() <dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
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

void PowerLinePatrol::goHomeMission()
{
    statusMutex->lock();
    plpstatus=8;
    statusMutex->unlock();
    bool disableGuid=isUsingGUID;
    isUsingGUID=false;
    moveBySpeedBodyFrame(&goHome,60000,0.5,15);
    isUsingGUID=disableGuid;
    if(abortMission)
    {
        statusMutex->lock();
        plpstatus=5;
        statusMutex->unlock();
        log="4009";
        emitLog(log);
        sprintf(DJI::onboardSDK::buffer, "%s","PLPMission, Go home aborted");
        api->serialDevice->displayLog();
        abortMission=false;
    }
    else
    {
        flight->task(Flight::TASK_LANDING);
        statusMutex->lock();
        plpstatus=6;
        statusMutex->unlock();
        log="2002";
        emitLog(log);
        sprintf(DJI::onboardSDK::buffer, "%s","PLPMission, Landing");
        api->serialDevice->displayLog();
    }
}

void PowerLinePatrol::plpMission()
{
    startMission();
    PositionData nextPos=nextPosition();
	
    while(nextPos.health)
    {
        int avoidance_flag = 0;
        log="300"+QString(QString::number(nextPos.health));
        emitLog(log);
        sprintf(DJI::onboardSDK::buffer, "%s%d","PLPMission, Fly to point ",nextPos.health);
        api->serialDevice->displayLog();
        //GPRSProtocolSend_6(QString("300"+QString(QString::number(nextPos.health))));
        //control yaw in ground frame, then control position in body frame, position offset calculated from gps.
        if(nextPos.health==getInfo().indexNumber)
        {
           // moveByPositionBodyFrame(&nextPos,60000,0.5,15);
			moveBySpeedBodyFrame(&nextPos, 60000, 0.5, 15);

			//如果有障碍，执行避障算法；
            if (distance_front < avoidDistanceFront&&isUsingGUID){
				avoidance_flag = 1;  
				PositionData curPosition = api->getBroadcastData().pos;//记录当前高度
				float32_t z = curPosition.height;
				do{
                    moveByPositionZOffset(1, 60000, 15);
					if (abortMission){
						break;
					}
		
                } while (distance_front < avoidDistanceFront&&isUsingGUID);
				do{
                    moveByPositionXOffset(2, 60000, 15);
					if (abortMission){
						break;
					}
                } while (distance_down < avoidDistanceBottom&&isUsingGUID);
                moveByPositionZDesired(z, 60000, 15);
				avoidance_flag = CalculateRadOffset(&nextPos);
			}
		}
				
        else
        {
			
            //moveByPositionBodyFrame(&nextPos);
			moveBySpeedBodyFrame(&nextPos);
            if (distance_front < avoidDistanceFront&&isUsingGUID){
				avoidance_flag = 1;
				PositionData curPosition = api->getBroadcastData().pos;//记录当前高度
				float32_t z = curPosition.height;
				do{
					moveByPositionZOffset(1, 60000, 30);
					if (abortMission){
						break;
					}

                } while (distance_front < avoidDistanceFront&&isUsingGUID);
				do{
					moveByPositionXOffset(2, 60000, 30);
					if (abortMission){
						break;
					}
                } while (distance_down < avoidDistanceBottom&&isUsingGUID);
				moveByPositionZDesired(z, 60000, 30);
				avoidance_flag = CalculateRadOffset(&nextPos);
			}
			
        }
        if(abortMission)
        {
            break;
        }
		
		if (avoidance_flag == 0 ){
			nextPos = nextPosition();
		}
    }
    if(abortMission)
    {
        sprintf(DJI::onboardSDK::buffer, "%s","PLPMission, Mission aborted");
        api->serialDevice->displayLog();
        abortMission=false;
        isRunning=false;
    }
    else
    {
        statusMutex->lock();
        plpstatus=7;
        statusMutex->unlock();
        log="4006";
        emitLog(log);
        sprintf(DJI::onboardSDK::buffer, "%s","PLPMission, Finished...");
        api->serialDevice->displayLog();
    }
}

void PowerLinePatrol::localOffsetFromGpsOffset(DJI::Vector3dData& deltaNed,
     PositionData* target, PositionData* origin)
{
    double deltaLon = target->longitude - origin->longitude;
    double deltaLat = target->latitude - origin->latitude;
    deltaNed.x = deltaLat * C_EARTH;
    deltaNed.y = deltaLon * C_EARTH * cos(target->latitude/2.0+origin->latitude/2.0);
    deltaNed.z = target->height - origin->height;
}
int PowerLinePatrol::moveByYawRate(float32_t yawDesired, float32_t zDesired, int timeoutInMs, float yawThresholdIndeg, float posDesiredInCm)
{
    uint8_t flag=0x99;
    double yawRateRad=40*DEG2RAD;
    double MinRateRad = 5*DEG2RAD;
    double yawDesiredRad=yawDesired*DEG2RAD;
    double yawThresholdInRad=yawThresholdIndeg*DEG2RAD;
    double curYawInRad=Flight::toEulerAngle(api->getBroadcastData().q).yaw;
    double yawRemaining=yawDesiredRad-curYawInRad;
    double zRemaining = zDesired - api->getBroadcastData().pos.height;
    double posDesiredInm = posDesiredInCm/100;

    int elapsedTime = 0;
    float yawCmd;
    if(yawDesiredRad>0&&curYawInRad<0&&(fabs(yawDesiredRad)+fabs(curYawInRad))>(180*DEG2RAD)){
        yawRemaining=curYawInRad-yawDesiredRad;
    }
    else
        yawRemaining=yawDesiredRad-curYawInRad;
    if(yawRemaining>0)
        yawCmd = yawRemaining > yawRateRad ? yawRateRad : yawRemaining;
    else if(yawRemaining<0)
        yawCmd = yawRemaining > -1*yawRateRad ? yawRemaining : -1*yawRateRad;

    //! Main closed-loop receding setpoint position control
    while(fabs(curYawInRad - yawDesiredRad) > yawThresholdInRad || fabs(zRemaining) > posDesiredInm)
    {
      // Check timeout
      if (elapsedTime >= timeoutInMs)
      {
          break;
      }
      if(abortMission)
      {
          break;
      }
      //MovementControl API call
      if(fabs(zRemaining) > posDesiredInm)
      {
          flight->setMovementControl(flag,0, 0, zDesired, 0);
      }
      else
      {
          flight->setMovementControl(flag,0, 0, zDesired, yawCmd*RAD2DEG);
      }
      /*QEventLoop eventloop;
      QTimer::singleShot(20, &eventloop, SLOT(quit()));
      eventloop.exec();*/
      msleep(20);

      elapsedTime += 20;

      //Get current position in required coordinates and units
      curYawInRad = Flight::toEulerAngle(api->getBroadcastData().q).yaw;
      zRemaining = zDesired - api->getBroadcastData().pos.height;

      //See how much farther we have to go
      //See if we need to modify the setpoint
      if(fabs(yawRemaining)<yawRateRad)
          yawRateRad *= 0.75;
      if(yawRateRad<MinRateRad)
          yawRateRad=MinRateRad;
      if((yawDesiredRad*curYawInRad)<0&&(fabs(yawDesiredRad)+fabs(curYawInRad))>(180*DEG2RAD)){
          yawRemaining=curYawInRad-yawDesiredRad;
      }
      else
          yawRemaining=yawDesiredRad-curYawInRad;
      if(yawRemaining>0)
          yawCmd = yawRemaining > yawRateRad ? yawRateRad : yawRemaining;
      else if(yawRemaining<0)
          yawCmd = yawRemaining > -1*yawRateRad ? yawRemaining : -1*yawRateRad;
    }
    return 1;
}

int PowerLinePatrol::moveBySpeedBodyFrame(PositionData* targetPosition, int timeoutInMs, float yawThresholdInDeg, float posThresholdInCm)
{
    uint8_t flag=0x52;//body frame, speed control
    // Get current poition
    PositionData curPosition = api->getBroadcastData().pos;
    DJI::Vector3dData curLocalOffset;
    DJI::EulerAngle curEuler = Flight::toEulerAngle(api->getBroadcastData().q);

    //Convert position offset from first position to local coordinates
    localOffsetFromGpsOffset(curLocalOffset, targetPosition, &curPosition);

    //Conversions
    double yawThresholdInRad = DEG2RAD*yawThresholdInDeg;
    float32_t posThresholdInM = posThresholdInCm/100;

    int elapsedTime = 0;
    float speedFactor = setSpeed;
    if(plpstatus==8)
    {
        speedFactor = goHomeSpeed;
    }
    float MinSpeed = 0.1;
    Angle radOffset=0;
    double xCmd=sqrt(curLocalOffset.x*curLocalOffset.x+curLocalOffset.y*curLocalOffset.y);
    double zCmd=targetPosition->height;
    radOffset=RAD2DEG*atan2(fabs(curLocalOffset.y),fabs(curLocalOffset.x));
    if(curLocalOffset.x<0&&curLocalOffset.y<0)
        radOffset=-180.0+radOffset;
    else if(curLocalOffset.x>0&&curLocalOffset.y<0)
        radOffset=-radOffset;
    else if(curLocalOffset.x<0&&curLocalOffset.y>0)
        radOffset=180-radOffset;
    if(std::abs(curEuler.yaw - radOffset) > yawThresholdInRad)
    {
        int cnt=40;
        //Run multiple times to fix the influence of the inertia
        while(cnt--)
        {
            moveByYawRate(radOffset,targetPosition->height);
        }
    }
    while(std::abs(curLocalOffset.x) > posThresholdInM || std::abs(curLocalOffset.y) > posThresholdInM || \
          std::abs(curLocalOffset.z) > posThresholdInM)
    {
      // Check timeout
      if (elapsedTime >= timeoutInMs)
      {
          break;
      }
      if(abortMission)
      {
          break;
      }

      //MovementControl API call

      if (distance_front < avoidDistanceFront&&isUsingGUID){

		  break;

	  }

      flight->setMovementControl(flag,xCmd, 0, zCmd,  radOffset);
      /*QEventLoop eventloop;
      QTimer::singleShot(20, &eventloop, SLOT(quit()));
      eventloop.exec();*/
      elapsedTime += 20;
      msleep(20);
      //Get current position in required coordinates and units
      curEuler = Flight::toEulerAngle(api->getBroadcastData().q);
      curPosition = api->getBroadcastData().pos;
      localOffsetFromGpsOffset(curLocalOffset, targetPosition, &curPosition);

      radOffset=RAD2DEG*atan2(fabs(curLocalOffset.y),fabs(curLocalOffset.x));
      if(curLocalOffset.x<0&&curLocalOffset.y<0)
          radOffset=-180.0+radOffset;
      else if(curLocalOffset.x>0&&curLocalOffset.y<0)
          radOffset=-radOffset;
      else if(curLocalOffset.x<0&&curLocalOffset.y>0)
          radOffset=180-radOffset;
      xCmd=sqrt(curLocalOffset.x*curLocalOffset.x+curLocalOffset.y*curLocalOffset.y);
      zCmd=targetPosition->height;
      if(xCmd<3*speedFactor)
      {
          speedFactor *= 0.75;
      }
      if(speedFactor<MinSpeed)
          speedFactor=MinSpeed;
      if (xCmd > speedFactor)
        xCmd = speedFactor;
    }
    return 1;
}

int PowerLinePatrol::moveByPositionOffset(float32_t xOffsetDesired, float32_t yOffsetDesired,
                                          float32_t zOffsetDesired, float32_t yawDesired,int timeoutInMs, float yawThresholdInDeg, float posThresholdInCm)
{
  uint8_t flag = 0x91; //Position Control

  // Get current poition
  PositionData curPosition = api->getBroadcastData().pos;
  PositionData originPosition = curPosition;
  DJI::Vector3dData curLocalOffset;
  DJI::EulerAngle curEuler = Flight::toEulerAngle(api->getBroadcastData().q);

  //Convert position offset from first position to local coordinates
  localOffsetFromGpsOffset(curLocalOffset, &curPosition, &originPosition);

  //See how much farther we have to go
  float32_t xOffsetRemaining = xOffsetDesired - curLocalOffset.x;
  float32_t yOffsetRemaining = yOffsetDesired - curLocalOffset.y;
  float32_t zOffsetRemaining = zOffsetDesired - curLocalOffset.z;

  //Conversions
  double yawDesiredRad = DEG2RAD*yawDesired;
  double yawThresholdInRad = DEG2RAD*yawThresholdInDeg;
  float32_t posThresholdInM = posThresholdInCm/100;

  int elapsedTime = 0;
  float speedFactor = 2;
  float xCmd, yCmd, zCmd;

  /*! Calculate the inputs to send the position controller. We implement basic
      receding setpoint position control and the setpoint is always 1 m away
      from the current position - until we get within a threshold of the goal.
      From that point on, we send the remaining distance as the setpoint.
  !*/
  if (xOffsetDesired > 0)
    xCmd = xOffsetDesired < speedFactor ? xOffsetDesired : speedFactor;
  else if (xOffsetDesired < 0)
    xCmd = xOffsetDesired > -1*speedFactor ? xOffsetDesired : -1*speedFactor;
  else
    xCmd = 0;

  if (yOffsetDesired > 0)
    yCmd = yOffsetDesired < speedFactor ? yOffsetDesired : speedFactor;
  else if (yOffsetDesired < 0)
    yCmd = yOffsetDesired > -1*speedFactor ? yOffsetDesired : -1*speedFactor;
  else
    yCmd = 0;

  zCmd = curPosition.height + zOffsetDesired;


  //! Main closed-loop receding setpoint position control
  while(std::abs(xOffsetRemaining) > posThresholdInM || std::abs(yOffsetRemaining) > posThresholdInM || \
        std::abs(zOffsetRemaining) > posThresholdInM || std::abs(curEuler.yaw - yawDesiredRad) > yawThresholdInRad)
  {
    // Check timeout
    if (elapsedTime >= timeoutInMs)
    {
        break;
    }
    if(abortMission)
    {
        break;
    }

    //MovementControl API call

    flight->setMovementControl(flag,xCmd, yCmd, zCmd, yawDesired);

    //sleep(20)
    /*QEventLoop eventloop;
    QTimer::singleShot(20, &eventloop, SLOT(quit()));
    eventloop.exec();*/
    //msleep(20);

    elapsedTime += 20;

    //Get current position in required coordinates and units
    curEuler = Flight::toEulerAngle(api->getBroadcastData().q);
    curPosition = api->getBroadcastData().pos;
    localOffsetFromGpsOffset(curLocalOffset, &curPosition, &originPosition);

    //See how much farther we have to go
    xOffsetRemaining = xOffsetDesired - curLocalOffset.x;
    yOffsetRemaining = yOffsetDesired - curLocalOffset.y;
    zOffsetRemaining = zOffsetDesired - curLocalOffset.z;
    //See if we need to modify the setpoint
    if(std::abs(xOffsetRemaining)<speedFactor*1.5&&std::abs(yOffsetRemaining)<speedFactor*1.5)
        speedFactor/=2.0;
    if (std::abs(xOffsetRemaining) < speedFactor)
      xCmd = xOffsetRemaining;
    if (std::abs(yOffsetRemaining) < speedFactor)
      yCmd = yOffsetRemaining;

  }
  return 1;
}

int  PowerLinePatrol::moveByPositionXOffset(float32_t xOffsetDesired, int timeoutInMs, float posThresholdInCm)
{
	uint8_t flag = 0x9b; //Position Control

	// Get current poition
	PositionData curPosition = api->getBroadcastData().pos;
	PositionData originPosition = curPosition;
	DJI::Vector3dData curLocalOffset;


	//Convert position offset from first position to local coordinates
	localOffsetFromGpsOffset(curLocalOffset, &curPosition, &originPosition);

	//See how much farther we have to go
	float32_t xOffsetRemaining = xOffsetDesired - sqrt(curLocalOffset.x*curLocalOffset.x + curLocalOffset.y*curLocalOffset.y);
	//Conversions
	float32_t posThresholdInM = posThresholdInCm / 100;
	int elapsedTime = 0;
	float speedFactor = 2;
	float xCmd;

	/*! Calculate the inputs to send the position controller. We implement basic
	receding setpoint position control and the setpoint is always 1 m away
	from the current position - until we get within a threshold of the goal.
	From that point on, we send the remaining distance as the setpoint.
	!*/
	if (xOffsetDesired > 0)
		xCmd = xOffsetDesired < speedFactor ? xOffsetDesired : speedFactor;
	else if (xOffsetDesired < 0)
		xCmd = xOffsetDesired > -1 * speedFactor ? xOffsetDesired : -1 * speedFactor;
	else
		xCmd = 0;


	//! Main closed-loop receding setpoint position control
	while (std::abs(xOffsetRemaining) > posThresholdInM)
	{
		// Check timeout
		if (elapsedTime >= timeoutInMs)
		{
			break;
		}
		if (abortMission)
		{
			break;
		}
		//MovementControl API call

		flight->setMovementControl(flag, xCmd, 0,curPosition.height , 0);

		//sleep(20)
		QEventLoop eventloop;
		QTimer::singleShot(20, &eventloop, SLOT(quit()));
		eventloop.exec();

		elapsedTime += 20;

		//Get current position in required coordinates and units
		curPosition = api->getBroadcastData().pos;
		localOffsetFromGpsOffset(curLocalOffset, &curPosition, &originPosition);

		//See how much farther we have to go
		xOffsetRemaining = xOffsetDesired - sqrt(curLocalOffset.x*curLocalOffset.x + curLocalOffset.y*curLocalOffset.y);
		//See if we need to modify the setpoint
		if (std::abs(xOffsetRemaining)<speedFactor*1.5)
			speedFactor /= 2.0;
		if (std::abs(xOffsetRemaining) < speedFactor)
			xCmd = xOffsetRemaining;

	}
	return 1;
}


int  PowerLinePatrol::moveByPositionZDesired(float32_t zDesired, int timeoutInMs, float posThresholdInCm)
{
	uint8_t flag = 0x9b; //Position Control

	// Get current poition
	PositionData curPosition = api->getBroadcastData().pos;
	//See how much farther we have to go
	float32_t zOffsetRemaining = zDesired - curPosition.height;

	//Conversions
	float32_t posThresholdInM = posThresholdInCm / 100;
	int elapsedTime = 0;

	float zCmd;

	/*! Calculate the inputs to send the position controller. We implement basic
	receding setpoint position control and the setpoint is always 1 m away
	from the current position - until we get within a threshold of the goal.
	From that point on, we send the remaining distance as the setpoint.
	!*/

	zCmd =  zDesired;

	//! Main closed-loop receding setpoint position control
	while (std::abs(zOffsetRemaining) > posThresholdInM)
	{
		// Check timeout
		if (elapsedTime >= timeoutInMs)
		{
			break;
		}
		if (abortMission)
		{
			break;
		}
		//MovementControl API call

		flight->setMovementControl(flag,0, 0, zCmd, 0);

		//sleep(20)
		QEventLoop eventloop;
		QTimer::singleShot(20, &eventloop, SLOT(quit()));
		eventloop.exec();

		elapsedTime += 20;

		//Get current position in required coordinates and units
		curPosition = api->getBroadcastData().pos;

		//See how much farther we have to go
		zOffsetRemaining = zDesired - curPosition.height;
		//See if we need to modify the setpoint

	}
	return 1;
}
int  PowerLinePatrol::moveByPositionZOffset(float32_t zOffsetDesired, int timeoutInMs, float posThresholdInCm)
{
	uint8_t flag = 0x9b; //Position Control

	// Get current poition
	PositionData curPosition = api->getBroadcastData().pos;
	PositionData originPosition = curPosition;
	DJI::Vector3dData curLocalOffset;


	//Convert position offset from first position to local coordinates
	localOffsetFromGpsOffset(curLocalOffset, &curPosition, &originPosition);

	//See how much farther we have to go

	float32_t zOffsetRemaining = zOffsetDesired - curLocalOffset.z;

	//Conversions
	float32_t posThresholdInM = posThresholdInCm / 100;
	int elapsedTime = 0;
	
	float zCmd;

	/*! Calculate the inputs to send the position controller. We implement basic
	receding setpoint position control and the setpoint is always 1 m away
	from the current position - until we get within a threshold of the goal.
	From that point on, we send the remaining distance as the setpoint.
	!*/

	zCmd = curPosition.height + zOffsetDesired;

	//! Main closed-loop receding setpoint position control
	while (std::abs(zOffsetRemaining) > posThresholdInM)
	{
		// Check timeout
		if (elapsedTime >= timeoutInMs)
		{
			break;
		}
		if (abortMission)
		{
			break;
		}
		//MovementControl API call

		flight->setMovementControl(flag, 0, 0, zCmd, 0);

		//sleep(20)
		QEventLoop eventloop;
		QTimer::singleShot(20, &eventloop, SLOT(quit()));
		eventloop.exec();

		elapsedTime += 20;

		//Get current position in required coordinates and units
		curPosition = api->getBroadcastData().pos;
		localOffsetFromGpsOffset(curLocalOffset, &curPosition, &originPosition);

		//See how much farther we have to go
		zOffsetRemaining = zOffsetDesired - curLocalOffset.z;
		//See if we need to modify the setpoint


	}
	return 1;
}

int PowerLinePatrol::obstacle(int health){
	int t = 0;
	if (health == 2){
		PositionData curPosition = api->getBroadcastData().pos;
	     float32_t z = curPosition.height;
		if (z > 2 && z < 5){
			t = 1;
		}
	}
	return t;

}
int  PowerLinePatrol::CalculateRadOffset(PositionData* targetPosition){
	
	
	
	double curYawInRad = Flight::toEulerAngle(api->getBroadcastData().q).yaw;
	double yawRemaining = 0;
	

	PositionData curPosition = api->getBroadcastData().pos;
	DJI::Vector3dData curLocalOffset;
	//Convert position offset from first position to local coordinates
	localOffsetFromGpsOffset(curLocalOffset, targetPosition, &curPosition);

	//Conversions

	Angle radOffset = 0;
	radOffset = RAD2DEG*atan2(fabs(curLocalOffset.y), fabs(curLocalOffset.x));
	if (curLocalOffset.x<0 && curLocalOffset.y<0)
		radOffset = -180.0 + radOffset;
	else if (curLocalOffset.x>0 && curLocalOffset.y<0)
		radOffset = -radOffset;
	else if (curLocalOffset.x<0 && curLocalOffset.y>0)
		radOffset = 180 - radOffset;
	double yawDesiredRad = radOffset*DEG2RAD;
	if (yawDesiredRad>0 && curYawInRad<0 && (fabs(yawDesiredRad) + fabs(curYawInRad))>(180 * DEG2RAD)){
		yawRemaining = curYawInRad - yawDesiredRad;
	}
	else
		yawRemaining = yawDesiredRad - curYawInRad;
	if (fabs(yawRemaining) > (90 * DEG2RAD)){
		return 0;
	}
	else 
		return 1;
}
void PowerLinePatrol::abortSignalSlot(const QString &abortMission)
{
    if(!abortMission.isEmpty())
        this->abortMission=true;
}
