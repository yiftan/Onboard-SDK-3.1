#include "powerlinepatrol.h"
using namespace DJI::onboardSDK;


unsigned short distance_front;
#include "DJI_utility.h"
#include "DJI_guidance.h"

#include <QDebug>

#define   C_EARTH (double) 6378137.0
#define   DEG2RAD (double)0.01745329252
#define   RAD2DEG (double)57.29577951308
//PowerLinePatrol p;
/*int my_callback(int data_type, int data_len, char *content)
{
	p.enter();
	if (e_obstacle_distance == data_type && NULL != content)
	{
		obstacle_distance *oa = (obstacle_distance*)content;
		distance_front = oa->distance[1];
		printf("obstacle distance:");

		for (int i = 0; i < CAMERA_PAIR_NUM; ++i)
			printf(" %f ", 0.01f * oa->distance[i]);

		printf("\n");
		printf("frame index:%d,stamp:%d\n", oa->frame_index, oa->time_stamp);
		printf("%f", distance_front);
	}
	p.set_event();
	p.leave();
	return 0;
}

std::ostream& operator<<(std::ostream& out, const e_sdk_err_code value){
	const char* s = 0;
	static char str[100] = { 0 };
#define PROCESS_VAL(p) case(p): s = #p; break;
	switch (value){
		PROCESS_VAL(e_OK);
		PROCESS_VAL(e_load_libusb_err);
		PROCESS_VAL(e_sdk_not_inited);
		PROCESS_VAL(e_disparity_not_allowed);
		PROCESS_VAL(e_image_frequency_not_allowed);
		PROCESS_VAL(e_config_not_ready);
		PROCESS_VAL(e_online_flag_not_ready);
		PROCESS_VAL(e_stereo_cali_not_ready);
		PROCESS_VAL(e_libusb_io_err);
		PROCESS_VAL(e_timeout);
	default:
		strcpy(str, "Unknown error");
		s = str;
		break;
	}
#undef PROCESS_VAL

	return out << s;
}


int PowerLinePatrol::guidanceTest(){
	reset_config();  // clear all data subscription
	int err_code = init_transfer(); //wait for board ready and init transfer thread
	qDebug() << err_code;

	//  RETURN_IF_ERR( err_code );
	select_obstacle_distance();
	user_call_back ucb = my_callback;
	err_code = set_sdk_event_handler(ucb);
	qDebug() << err_code;
	// RETURN_IF_ERR( err_code );
	err_code = start_transfer();
	//   RETURN_IF_ERR( err_code );

	qDebug() << err_code;


	p.wait_event();


	err_code = stop_transfer();
	qDebug() << err_code;
	//RETURN_IF_ERR( err_code );
	//make sure the ack packet from GUIDANCE is received
	//sleep( 1000000 );
	err_code = release_transfer();
	qDebug() << err_code;
	//  RETURN_IF_ERR( err_code );
	return 0;
}
*/
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
    setSpeed=2.0;
    setheight=2.0;
	InitializeCriticalSection( &m_critical_section );
    SECURITY_ATTRIBUTES   sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    CreatePipe( &m_pipe_read, &m_pipe_write, &sa, 0 );
}

PowerLinePatrol::PowerLinePatrol()
{
    CloseHandle( m_pipe_read );
    CloseHandle( m_pipe_write );
}

void PowerLinePatrol::stop()
{
    stopped=true;
}
void PowerLinePatrol::run()
{
    while(!stopped)
    {
        plpMission();
    }
    stopped=false;
    //qDebug()<<QString("PLP Thread stoped");
}

void PowerLinePatrol::enter()
{
    EnterCriticalSection( &m_critical_section );
}

void PowerLinePatrol::leave()
{
    LeaveCriticalSection( &m_critical_section );
}

int PowerLinePatrol::set_event()
{
    char buffer[1] = {0};
    int count = 0;
    int ret = WriteFile( m_pipe_write, (LPWORD)buffer, 1, (LPDWORD)&count, NULL );
    return ret;
}

int PowerLinePatrol::wait_event()
{
    char buffer[1] = {0};
    int count = 0;
    int ret = ReadFile( m_pipe_read, (LPWORD)buffer, 1, (LPDWORD)&count, NULL );
    return ret;
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



///

void PowerLinePatrol::plpMission()
{
    startMission();
    PositionData nextPos=nextPosition();
	
    while(nextPos.health)
	{
		int avoidance_flag = 0;
        sprintf(DJI::onboardSDK::buffer, "%s%d","PLPMission, Fly to point ",nextPos.health);
        api->serialDevice->displayLog();
        //GPRSProtocolSend_6(QString("300"+QString(QString::number(nextPos.health))));
        //control yaw in ground frame, then control position in body frame, position offset calculated from gps.
        if(nextPos.health==getInfo().indexNumber)
        {
           // moveByPositionBodyFrame(&nextPos,60000,0.5,15);
			moveBySpeedBodyFrame(&nextPos, 60000, 0.5, 15);

			//如果有障碍，执行避障算法；
			if (distance_front < 2){
				avoidance_flag = 1;  
				PositionData curPosition = api->getBroadcastData().pos;//记录当前高度
				float32_t z = curPosition.height;
				do{
					moveByPositionZOffset(1, 60000, 30);
		
				} while (distance_front < 2);
				moveByPositionXOffset(2, 60000, 30);
				moveByPositionZDesired(z, 60000, 30);
			}

			
		}
				
        else
        {
			
            //moveByPositionBodyFrame(&nextPos);
			moveBySpeedBodyFrame(&nextPos);
			if (distance_front < 2){
			    avoidance_flag = 1;
				PositionData curPosition = api->getBroadcastData().pos;
				float32_t z = curPosition.height;
				do{
					moveByPositionZOffset(1, 60000, 30);
				
				} while (distance_front < 2);
				moveByPositionXOffset(2,60000, 30);
				moveByPositionZDesired(z, 60000, 30);
			   }

			
			}
        if(abortMission)
        {
            break;
        }
		if (avoidance_flag == 0){
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
        plpstatus=7;
        isRunning=false;
        sprintf(DJI::onboardSDK::buffer, "%s","PLPMission, Finished...");
        api->serialDevice->displayLog();
        //flight->task(Flight::TASK_GOHOME);
    }
    /*on_btn_coreSetControl_clicked();
    sleepmSec(1000);
    on_btn_coreSetControl_clicked();*/
    //flight->task(Flight::TASK_GOHOME);
    stopped=true;
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

	  //guidance abort here
	  //if(前方有障碍）
	  //记录当前的高度信息
	  //break
	  if (distance_front < 2){

		  break;

	  }

      flight->setMovementControl(flag,0, 0, zDesired, yawCmd*RAD2DEG);
      QEventLoop eventloop;
      QTimer::singleShot(20, &eventloop, SLOT(quit()));
      eventloop.exec();

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
            moveByYawRate(radOffset,curPosition.height);
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

	

      flight->setMovementControl(flag,xCmd, 0, zCmd,  radOffset);
      QEventLoop eventloop;
      QTimer::singleShot(20, &eventloop, SLOT(quit()));
      eventloop.exec();
      elapsedTime += 20;
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

int PowerLinePatrol::moveByPositionBodyFrame(PositionData* targetPosition,int timeoutInMs, float yawThresholdInDeg, float posThresholdInCm)
{
    uint8_t flag=0x93;//body frame, position control
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
        int cnt=50;
        //Run multiple times to fix the influence of the inertia
        while(cnt--)
        {
            moveByYawRate(radOffset,curPosition.height);
        }
    }
    if (xCmd > speedFactor)
      xCmd = speedFactor;
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
      flight->setMovementControl(flag,xCmd, 0, zCmd, radOffset);
      QEventLoop eventloop;
      QTimer::singleShot(20, &eventloop, SLOT(quit()));
      eventloop.exec();
      elapsedTime += 20;
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
      if(xCmd<2*speedFactor)
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
    QEventLoop eventloop;
    QTimer::singleShot(20, &eventloop, SLOT(quit()));
    eventloop.exec();

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

		flight->setMovementControl(flag, xCmd, 0, 0, 0);

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
		float32_t zOffsetRemaining = zDesired - curPosition.height;
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
			t = 1;
	}
	return t;

}
