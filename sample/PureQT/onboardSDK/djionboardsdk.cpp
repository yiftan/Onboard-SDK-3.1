#include "djionboardsdk.h"
#include "ui_djionboardsdk.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTime>
#include <QDir>
#include <QMouseEvent>
#include <QScrollBar>
#include <iostream>
#include <fstream>
#include <string>

#include <QDebug>
#include "DJI_utility.h"
#include "DJI_guidance.h"
using namespace std;
#pragma comment(lib,"../onboardSDK/DJI_guidance.lib")
PowerLinePatrol p(new CoreAPI(),new Flight());
//unsigned short distance_front;
int my_callback(int data_type, int data_len, char *content)
{
    p.enter();
    if ( e_obstacle_distance == data_type && NULL != content )
    {
        obstacle_distance *oa = (obstacle_distance*)content;
		distance_down = oa->distance[0]*0.01;
        //qDebug() << distance_down;
		distance_front = oa->distance[1]*0.01;
        //qDebug() << distance_front;
    }
    p.set_event();
    p.leave();
    return 0;
}

std::ostream& operator<<(std::ostream& out, const e_sdk_err_code value){
    const char* s = 0;
    static char str[100]={0};
#define PROCESS_VAL(p) case(p): s = #p; break;
    switch(value){
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


void DJIonboardSDK::guidanceTest(){
    int err_code;
    if(!initGuiFlag){
        reset_config();  // clear all data subscription
        err_code = init_transfer(); //wait for board ready and init transfer thread
        qDebug() << err_code;
        if(err_code==0){
            initGuiFlag=true;
        }

    }
    else{
      //  RETURN_IF_ERR( err_code );
	select_obstacle_distance();
	user_call_back ucb = my_callback;
    err_code = set_sdk_event_handler(ucb);
    if(err_code!=0){
        initGuiFlag=false;
    }
	//qDebug() << err_code;
	err_code = start_transfer();
    p.wait_event();
    err_code = stop_transfer();
	//	qDebug() << err_code;

   //     err_code = release_transfer();
	//	qDebug() << err_code;
    }
}

void DJIonboardSDK::guidance(){
	guidance_obstacle = new QTimer();
	guidance_obstacle->setInterval(500); // 10Hz
	connect(guidance_obstacle, SIGNAL(timeout()), this, SLOT(guidanceTest()));
    guidance_obstacle->start();
}

void DJIonboardSDK::initFollow()
{
    FollowTarget targetBase;
    targetBase.latitude = 0;
    targetBase.longitude = 0;
    targetBase.angle = 0;
    targetBase.height = 0;
    follow->setTarget(targetBase);
    followSend = new QTimer();
    followSend->setInterval(20); // 50Hz
   connect(followSend, SIGNAL(timeout()), this, SLOT(on_tmr_follow_send()));
}

void DJIonboardSDK::initDisplay()
{
    ui->tb_display->setUndoRedoEnabled(false);
    ui->tb_display->document()->setMaximumBlockCount(-1);
    ui->tb_display->setReadOnly(true);
    ui->tb_display->setContextMenuPolicy(Qt::CustomContextMenu);
    driver->setDisplay(ui->tb_display);
    GPRSdriver->setDisplay(ui->tb_display);
}

void DJIonboardSDK::initWayPoint()
{
    waypointData = new QStandardItemModel();
    waypointData->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("No.")));
    waypointData->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Latitude")));
    waypointData->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("Longitude")));
    waypointData->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("Altitude")));
    waypointData->setHorizontalHeaderItem(4, new QStandardItem(QObject::tr("Damping")));
    waypointData->setHorizontalHeaderItem(5, new QStandardItem(QObject::tr("Yaw")));
    waypointData->setHorizontalHeaderItem(6, new QStandardItem(QObject::tr("Pitch")));
    waypointData->setHorizontalHeaderItem(7, new QStandardItem(QObject::tr("TurnMode")));
    waypointData->setHorizontalHeaderItem(8, new QStandardItem(QObject::tr("Anction Num")));
    waypointData->setHorizontalHeaderItem(9, new QStandardItem(QObject::tr("Time limit")));
    waypointData->setHorizontalHeaderItem(10, new QStandardItem(QObject::tr("Repeat times")));

    nullAction = initAction();
    currentAction = nullAction;

    ui->tv_waypoint_actions->setItemDelegateForColumn(0, new ReadOnlyDelegate());
    ui->tv_waypoint_actions->setItemDelegateForColumn(1, new ReadOnlyDelegate());
    ui->tv_waypoint_actions->setItemDelegateForColumn(2, new ActionDelegate());
    ui->tv_waypoint_actions->setModel(currentAction);
    ui->tv_waypoint_actions->verticalHeader()->hide();
    ui->tv_waypoint_actions->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tv_waypoint_data->setItemDelegateForColumn(0, new ReadOnlyDelegate());
    ui->tv_waypoint_data->setItemDelegateForColumn(7, new TurnModeDelegate());
    ui->tv_waypoint_data->setItemDelegateForColumn(8, new ReadOnlyDelegate());
    ui->tv_waypoint_data->setModel(waypointData);
    ui->tv_waypoint_data->verticalHeader()->hide();
    ui->tv_waypoint_data->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(waypointData, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), this,
            SLOT(on_waypoint_data_changed(QModelIndex, QModelIndex, QVector<int>)));

    ui->cb_waypoint_point->addItem("Null");
}

void DJIonboardSDK::initVirtualRC()
{
    vrcSend = new QTimer();
    vrcSend->setInterval(200); // 5Hz
    connect(vrcSend, SIGNAL(timeout()), this, SLOT(on_tmr_VRC_autosend()));
}

void DJIonboardSDK::initFlight()
{
    connect(ui->btg_flightHL, SIGNAL(buttonClicked(QAbstractButton *)), this,
            SLOT(on_btg_flight_HL(QAbstractButton *)));
    connect(ui->btg_flightVL, SIGNAL(buttonClicked(QAbstractButton *)), this,
            SLOT(on_btg_flight_VL(QAbstractButton *)));
    connect(ui->btg_flightYL, SIGNAL(buttonClicked(QAbstractButton *)), this,
            SLOT(on_btg_flight_YL(QAbstractButton *)));
    connect(ui->btg_flightCL, SIGNAL(buttonClicked(QAbstractButton *)), this,
            SLOT(on_btg_flight_CL(QAbstractButton *)));
    connect(ui->btg_flightSM, SIGNAL(buttonClicked(QAbstractButton *)), this,
            SLOT(on_btg_flight_SM(QAbstractButton *)));

    on_btg_flight_HL(ui->btg_flightHL->checkedButton());
    on_btg_flight_VL(ui->btg_flightVL->checkedButton());
    on_btg_flight_YL(ui->btg_flightYL->checkedButton());
    on_btg_flight_CL(ui->btg_flightCL->checkedButton());
    on_btg_flight_SM(ui->btg_flightSM->checkedButton());

    resetFlightData();

    autoSend = new QTimer();
    autoSend->setInterval(50); // 20Hz
    connect(autoSend, SIGNAL(timeout()), this, SLOT(filght_autosend()));
}

void DJIonboardSDK::initCamera()
{
    connect(ui->btg_cameraAngle, SIGNAL(buttonClicked(QAbstractButton *)), this,
            SLOT(on_btg_cameraAngle(QAbstractButton *)));
    connect(ui->btg_cameraYaw, SIGNAL(buttonClicked(QAbstractButton *)), this,
            SLOT(on_btg_cameraYaw(QAbstractButton *)));
    connect(ui->btg_cameraRoll, SIGNAL(buttonClicked(QAbstractButton *)), this,
            SLOT(on_btg_cameraRoll(QAbstractButton *)));
    connect(ui->btg_cameraPitch, SIGNAL(buttonClicked(QAbstractButton *)), this,
            SLOT(on_btg_cameraPitch(QAbstractButton *)));

    camFlag = 0;

    on_btg_cameraAngle(ui->btg_cameraAngle->checkedButton());
    on_btg_cameraYaw(ui->btg_cameraYaw->checkedButton());
    on_btg_cameraRoll(ui->btg_cameraRoll->checkedButton());
    on_btg_cameraPitch(ui->btg_cameraPitch->checkedButton());
    updateCameraFlag();

    cameraSend = new QTimer();
    cameraSend->setInterval(100); // 10Hz
    connect(cameraSend, SIGNAL(timeout()), this, SLOT(on_tmr_Camera_autosend()));
}

void DJIonboardSDK::initGPRS()
{
    GPRSautoSend=new QTimer();
    GPRSautoRead=new QTimer();
    GPRSautoSend->setInterval(2000);
    GPRSautoRead->setInterval(500);
    connect(GPRSautoSend,SIGNAL(timeout()), this, SLOT(on_tmr_GPRS_autosend()));
    connect(GPRSautoRead,SIGNAL(timeout()), this, SLOT(on_tmr_GPRS_autoread()));
}

void DJIonboardSDK::functionAlloc()
{
    if (api->getSDKVersion() == versionM100_23)
    {
        ui->gb_CoreData->setEnabled(false);
        ui->gb_VRC->setEnabled(false);
        ui->gb_RTK->setEnabled(false);
        ui->gb_GPS->setEnabled(false);
        ui->gb_Mission->setEnabled(false);
        ui->gb_wp->setEnabled(false);
    }
    else if (api->getSDKVersion() == versionM100_31)
    {
        ui->gb_CoreData->setEnabled(true);
        ui->gb_VRC->setEnabled(true);
        ui->gb_RTK->setEnabled(false);
        ui->gb_GPS->setEnabled(false);
        ui->gb_Mission->setEnabled(true);
        ui->gb_wp->setEnabled(true);
    }
    else
    {
        ui->gb_CoreData->setEnabled(true);
        ui->gb_VRC->setEnabled(true);
        ui->gb_RTK->setEnabled(true);
        ui->gb_GPS->setEnabled(true);
        ui->gb_Mission->setEnabled(true);
        ui->gb_wp->setEnabled(true);
    }
}

DJIonboardSDK::DJIonboardSDK(QWidget *parent) : QMainWindow(parent), ui(new Ui::DJIonboardSDK)
{
    abortMission=false;
    initGuiFlag=false;
    CommandData=0;
    GPRSflag=0;
    GPRSst=0;
    for(int i=0;i<6;i++){
        ProtocolFlag[i]=false;
    }
    GPRSCommand[0]=QString("AT+CGCLASS=\"B\"");
    GPRSCommand[1]=QString("AT+CGDCONT=1,\"IP\",\"CMNET\"");
    GPRSCommand[2]=QString("AT+CGATT=1");
    GPRSCommand[3]=QString("AT+CIPCSGP=1,\"CMNET\"");
    GPRSCommand[4]=QString("AT+CLPORT=\"TCP\",\"2000\"");
    GPRSCommand[5]=QString("AT+CIPSTART=\"TCP\",\"115.230.110.58\",\"9876\"");//IP
    GPRSCommand[6]=QString("AT+CIPSHUT");
    GPRSConnectflag=0;
    setspeed=2.0;
    FlightStatusSet.v=2.0;
    FlightStatusSet.Height=2.0;
    FlightStatusSet.distance=2.0;
    FlightDirectSet.pointData=NULL;
    ui->setupUi(this);

    ui->btn_flightCheckStatus->setVisible(false);
    ui->btn_waypointDownload->setVisible(false);
    ui->btn_waypoint_reset->setVisible(false);
    ui->tabWidget->removeTab(3);

    initSDK();
    initDisplay();
    initFlight();
    initCamera();
    initFollow();
    initWayPoint();
    initVirtualRC();
    initGPRS();
	guidance();////
#ifdef GROUNDSTATION
    initGroundStation();
#endif // GROUNDSTATION

    QFile f("settings.ini");
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->lineEdit_ID->setText("1033762");
        ui->lineEdit_Key->setText("0bb939906a5e2232ca39d69e7345778cbac3cf1ba609f189248b13c816850674");
        qDebug() << "fail to open";
    }
    else
    {
        while (!f.atEnd())
        {
            QByteArray line = f.readLine();
            if (line.startsWith("ID:"))
                ui->lineEdit_ID->setText(line.remove(0, 3));
            else if (line.startsWith("KEY:"))
                ui->lineEdit_Key->setText(line.remove(0, 4));
        }
        f.close();
    }

    //! @code version control
    timerBroadcast = new QTimer();
    on_cb_core_mechine_activated(ui->cb_core_mechine->currentIndex());
    functionAlloc();
    //! @endcode

    connect(timerBroadcast, SIGNAL(timeout()), this, SLOT(on_tmr_Broadcast()));
    timerBroadcast->start(300);

//! @todo test codes
#define link(x) #x
    qDebug() << sizeof(DJI::onboardSDK::MagnetData) << link(DJI::onboardSDK::MagnetData);
    startTimer(2);

#ifdef SDK_DEV
    dev = new UIDev(this, api, port);
    QHBoxLayout *devLayout = new QHBoxLayout();
    devLayout->addWidget(dev);
    ui->tbDev->setLayout(devLayout);
#endif // SDK_DEV

    activateSDKTimer = new QTimer();
    activateSDKTimer->setInterval(ACTIVEPERIOD);
    connect(activateSDKTimer, SIGNAL(timeout()), this, SLOT(autoActivateSDK()));
    activateGPRSTimer = new QTimer();
    activateGPRSTimer->setInterval(1000);
    connect(activateGPRSTimer, SIGNAL(timeout()), this, SLOT(autoActivateGPRS()));
    plpTimer = new QTimer();
    plpTimer->setInterval(1000);
    connect(plpTimer,SIGNAL(timeout()),this,SLOT(plpMissionCheck()));
    activateSDKTimer->start();
    activateGPRSTimer->start();
    plpTimer->start();
    autoSendStatus = new QTimer();
    autoSendStatus->setInterval(8000);
    connect(autoSendStatus,SIGNAL(timeout()),this,SLOT(on_tmr_autoSendStatus()));
    autoSendStatus->start();
    ui->btn_GPRSportSend->setText(QString("started"));
}

DJIonboardSDK::~DJIonboardSDK()
{
    delete ui;
}

void DJIonboardSDK::closeEvent(QCloseEvent *)
{
    qDebug() << "close";
    QFile f("settings.ini");
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        qDebug() << "fail to open";
    else
    {
        f.write(QString("ID:").append(ui->lineEdit_ID->text()).append("\r\n").toUtf8());
        f.write(QString("KEY:").append(ui->lineEdit_Key->text()).append("\r\n").toUtf8());
        f.close();
    }
    if (ui->btn_coreSetControl->text() == "Release Control")
        api->setControl(false, DJIonboardSDK::setControlCallback, this);
    /*if(GPRSConnectflag==1)
    {
        GPRSDataSend("AT+CIPSHUT");
    }*/
}

void DJIonboardSDK::timerEvent(QTimerEvent *)
{
    api->sendPoll();
    api->readPoll();
    //    scriptSDK->addTask("addTask");
    //    scriptSDK->run();
}
void DJIonboardSDK::sleepmSec(int mSec)
{
    QTime dieTime = QTime::currentTime().addMSecs(mSec);
    while( QTime::currentTime() <dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
void DJIonboardSDK::mouseClicked(QWidget* wid)
{
    QPoint pos;
    pos.setX(1);
    pos.setY(1);
    QMouseEvent mEvnPress(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent mEvnRelease(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(wid, &mEvnPress);
    QApplication::sendEvent(wid, &mEvnRelease);
}
void DJIonboardSDK::on_tmr_autoSendStatus()
{
    //static int count=0;
    /*if(flight->getStatus()==1)
        plp->plpstatus=1;*/
    int plpstatus;
    //plp->statusMutex->lock();
    plpstatus=plp->plpstatus;
    //plp->statusMutex->unlock();
    if(GPRSConnectflag)
    {
        GPRSProtocolSend_5(api->getBroadcastData().pos.longitude*RAD2DEG,api->getBroadcastData().pos.latitude*RAD2DEG,api->getBroadcastData().pos.height,api->getBroadcastData().v.x,plpstatus);
        //count=1;
    }
}

void DJIonboardSDK::plpMissionCheck()
{
    static uint32_t origintime=0;
    static int sendActivateOk=0;
    uint32_t curtime=api->getTime().time;
    uint32_t timeinterval=curtime-origintime;
    origintime=curtime;
    if(timeinterval==0&&!activateSDKTimer->isActive())
    {
        activateSDKTimer->start();
    }
    if(!GPRSport->isOpen()&&!activateGPRSTimer->isActive())
        activateGPRSTimer->start();

    if(sendActivateOk==0&&GPRSConnectflag&&ui->btn_coreSetControl->text()=="Release Control")
    {
        GPRSProtocolSend_6(QString("1001"));
        GPRSProtocolSend_6(QString("2001"));
        sendActivateOk=1;
    }
    if(ProtocolFlag[5])
    {
        plp->goHome.latitude=api->getBroadcastData().pos.latitude;
        plp->goHome.longitude=api->getBroadcastData().pos.longitude;
        GPRSProtocolSend_7('Y',plp->goHome.longitude*RAD2DEG,plp->goHome.latitude*RAD2DEG);
        ProtocolFlag[5]=false;
    }
    if(ProtocolFlag[3])
    {
        switch(CommandData){
            case 1: if(flight->getStatus()==1)
                    {
                        ui->radioButton_4->setChecked(true);//Take off
                        //mouseClicked(ui->btn_flight_runTask);
                        plp->goHome.latitude=api->getBroadcastData().pos.latitude;
                        plp->goHome.longitude=api->getBroadcastData().pos.longitude;
                        on_btn_flight_runTask_clicked();
                        sleepmSec(1000);
                        if(flight->getStatus()==3)
                        {
                            plp->plpstatus=3;
                            GPRSProtocolSend_3(CommandData,'Y');
                            GPRSProtocolSend_6(QString("2002"));
                        }
                        else
                        {
                            GPRSProtocolSend_3(CommandData,'N');
                        }
                    }
                    break;
            case 2: if(plp->isRunning||plp->getInfo().indexNumber==0)
                        GPRSProtocolSend_3(CommandData,'N');
                    else
                    {
                        //mouseClicked(ui->btn_plp_start_stop);//Start mission
                        plp->isStart=true;
                        plp->plpstatus=4;
                        GPRSProtocolSend_3(CommandData,'Y');
                    }
                    break;
            case 3: if(!plp->isRunning)
                        GPRSProtocolSend_3(CommandData,'N');
                    else
                    {
                        //mouseClicked(ui->btn_Abortplp);//Abort mission
                        GPRSProtocolSend_3(CommandData,'Y');
                        on_btn_Abortplp_clicked();
                        plp->plpstatus=5;

                    }
                    break;
            case 4: if(flight->getStatus()==3&&!plp->isRunning)
                    {
                        ui->radioButton_7->setChecked(true);//Landing
                        //mouseClicked(ui->btn_flight_runTask);
                        on_btn_flight_runTask_clicked();
                        sleepmSec(1000);
                        if(flight->getStatus()==4||flight->getStatus()==5)
                        {
                            plp->plpstatus=6;
                            GPRSProtocolSend_3(CommandData,'Y');
                            GPRSProtocolSend_6(QString("2003"));
                        }
                        else
                        {
                            GPRSProtocolSend_3(CommandData,'N');
                        }
                    }
                    else
                        GPRSProtocolSend_3(CommandData,'N');
                    break;
            case 5: if(!plp->isRunning)
                    {
                        ui->radioButton_8->setChecked(true);//Go home
                        plp->plpstatus=8;
                        //mouseClicked(ui->btn_flight_runTask);
                        //on_btn_flight_runTask_clicked();
                        plp->start();
                        GPRSProtocolSend_3(CommandData,'Y');
                        GPRSProtocolSend_6(QString("2004"));
                    }
                    else
                        GPRSProtocolSend_3(CommandData,'N');
                    break;
            default: break;
        }
        CommandData=0;
        ProtocolFlag[3]=false;
    }
    if(ProtocolFlag[2])
    {
        plp->isUsingGPRSData=true;
        //mouseClicked(ui->btn_plp_loadAll);
        on_btn_plp_loadAll_clicked();
        ProtocolFlag[2]=false;
    }
    if(ProtocolFlag[1])
    {
        plp->setSpeed=FlightStatusSet.v;
        plp->setheight=FlightStatusSet.Height;
        ProtocolFlag[1]=false;
    }

    if(ProtocolFlag[0])
    {
        GPRSProtocolSend_0(FlightStatusSet.Height,FlightStatusSet.v,plp->goHome.height,plp->goHomeSpeed,FlightStatusSet.distance);
        ProtocolFlag[0]=false;
    }
    if(plp->isStart&&!plp->isRunning)
    {
        plp->isStart=false;
        //mouseClicked(ui->btn_plp_start_stop);
        on_btn_plp_start_stop_clicked();
    }
    if(plp->plpstatus==6&&flight->getStatus()==1)//go home finnished and landing
    {
        plp->plpstatus=1;
    }
}

void DJIonboardSDK::plpMission()
{
    plp->startMission();
    PositionData nextPos=plp->nextPosition();
    while(nextPos.health)
    {
        sprintf(DJI::onboardSDK::buffer, "%s%d","PLPMission, Fly to point ",nextPos.health);
        api->serialDevice->displayLog();
        GPRSProtocolSend_6(QString("300"+QString(QString::number(nextPos.health))));
        //control yaw in ground frame, then control position in body frame, position offset calculated from gps.
		if (nextPos.health == plp->getInfo().indexNumber)
		{
			//moveByPositionBodyFrame(&nextPos,60000,0.5,15);
			moveBySpeedBodyFrame(&nextPos, 60000, 0.5, 15);


		}
    
        else
        {
            //moveByPositionBodyFrame(&nextPos);
            moveBySpeedBodyFrame(&nextPos);

		
        }
        if(plp->abortMission)
        {
            break;
        }
        nextPos=plp->nextPosition();
    }
    if(plp->abortMission)
    {
        sprintf(DJI::onboardSDK::buffer, "%s","PLPMission, Mission aborted");
        api->serialDevice->displayLog();
        plp->abortMission=false;
        plp->isRunning=false;
    }
    else
    {
        plp->plpstatus=7;
        plp->isRunning=false;
        GPRSProtocolSend_6(QString("4006"));
        sprintf(DJI::onboardSDK::buffer, "%s","PLPMission, Finished...");
        api->serialDevice->displayLog();
        //flight->task(Flight::TASK_GOHOME);
    }
    on_btn_coreSetControl_clicked();
    sleepmSec(1000);
    on_btn_coreSetControl_clicked();
    //flight->task(Flight::TASK_GOHOME);
}

void DJIonboardSDK::localOffsetFromGpsOffset(DJI::Vector3dData& deltaNed,
     PositionData* target, PositionData* origin)
{
    double deltaLon = target->longitude - origin->longitude;
    double deltaLat = target->latitude - origin->latitude;
    deltaNed.x = deltaLat * C_EARTH;
    deltaNed.y = deltaLon * C_EARTH * cos(target->latitude/2.0+origin->latitude/2.0);
    deltaNed.z = target->height - origin->height;
}
int DJIonboardSDK::moveByYawRate(float32_t yawDesired, float32_t zDesired, int timeoutInMs, float yawThresholdIndeg, float posDesiredInCm)
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
      if(plp->abortMission)
      {
          break;
      }
      //MovementControl API call

      flight->setMovementControl(flag,0, 0, zDesired, yawCmd*RAD2DEG);
      sleepmSec(20);

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

int DJIonboardSDK::moveBySpeedBodyFrame(PositionData* targetPosition, int timeoutInMs, float yawThresholdInDeg, float posThresholdInCm)
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
    float speedFactor = FlightStatusSet.v;
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
    while(std::abs(curLocalOffset.x) > posThresholdInM || std::abs(curLocalOffset.y) > posThresholdInM || std::abs(curLocalOffset.z) > posThresholdInM)
    {
      // Check timeout
      if (elapsedTime >= timeoutInMs)
      {
          break;
      }
      if(plp->abortMission)
      {
          break;
      }
      //MovementControl API call
	 



	  //obstacle_avoidance
    //飞行之前做避障判断
	  
	  flight->setMovementControl(flag,xCmd, 0, zCmd,  radOffset);
      sleepmSec(20);
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

int DJIonboardSDK::moveByPositionBodyFrame(PositionData* targetPosition,int timeoutInMs, float yawThresholdInDeg, float posThresholdInCm)
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
    float speedFactor = FlightStatusSet.v;
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
    while(std::abs(curLocalOffset.x) > posThresholdInM || std::abs(curLocalOffset.y) > posThresholdInM || std::abs(curLocalOffset.z) > posThresholdInM)
    {
      // Check timeout
      if (elapsedTime >= timeoutInMs)
      {
          break;
      }
      if(plp->abortMission)
      {
          break;
      }
      //MovementControl API call
      flight->setMovementControl(flag,xCmd, 0, zCmd, radOffset);
      sleepmSec(20);
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

int DJIonboardSDK::moveByPositionOffset(float32_t xOffsetDesired, float32_t yOffsetDesired, float32_t zOffsetDesired, float32_t yawDesired,
                         int timeoutInMs, float yawThresholdInDeg, float posThresholdInCm)
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
  while(std::abs(xOffsetRemaining) > posThresholdInM || std::abs(yOffsetRemaining) > posThresholdInM || std::abs(zOffsetRemaining) > posThresholdInM || std::abs(curEuler.yaw - yawDesiredRad) > yawThresholdInRad)
  {
    // Check timeout
    if (elapsedTime >= timeoutInMs)
    {
        break;
    }
    if(plp->abortMission)
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

void DJIonboardSDK::setControlCallback(CoreAPI *This, Header *header, UserData userData)
{
    DJIonboardSDK *sdk = (DJIonboardSDK *)userData;
    unsigned short ack_data = ACK_COMMON_NO_RESPONSE;
    unsigned char data = 0x1;

    if (header->length - EXC_DATA_SIZE <= 2)
    {
        memcpy((unsigned char *)&ack_data, ((unsigned char *)header) + sizeof(Header),
               (header->length - EXC_DATA_SIZE));
    }
    else
    {
        API_LOG(sdk->driver, ERROR_LOG, "ACK is exception,seesion id %d,sequence %d\n",
                header->sessionID, header->sequenceNumber);
    }

    switch (ack_data)
    {
        case ACK_SETCONTROL_NEED_MODE_F:
            if (sdk)
                sdk->ui->btn_coreSetControl->setText("Switch to mod F");
            else
                API_LOG(sdk->driver, ERROR_LOG, "known SDK pointer 0.");
            break;
        case ACK_SETCONTROL_RELEASE_SUCCESS:
            if (sdk)
                sdk->ui->btn_coreSetControl->setText("Obtain Control");
            else
                API_LOG(sdk->driver, ERROR_LOG, "known SDK pointer 0.");
            break;
        case ACK_SETCONTROL_OBTAIN_SUCCESS:
            if (sdk)
                sdk->ui->btn_coreSetControl->setText("Release Control");
            else
                API_LOG(sdk->driver, ERROR_LOG, "known SDK pointer 0.");
            break;
        case ACK_SETCONTROL_OBTAIN_RUNNING:
            This->send(2, encrypt, SET_CONTROL, CODE_SETCONTROL, &data, 1, 500, 2,
                       DJIonboardSDK::setControlCallback, userData);
            break;
        case ACK_SETCONTROL_RELEASE_RUNNING:
            data = 0;
            This->send(2, encrypt, SET_CONTROL, CODE_SETCONTROL, &data, 1, 500, 2,
                       DJIonboardSDK::setControlCallback, userData);
            break;
    }
}

void DJIonboardSDK::activationCallback(CoreAPI *This, Header *header, UserData userData)
{
    DJIonboardSDK *sdk = (DJIonboardSDK *)userData;
    volatile unsigned short ack_data;
    if (header->length - EXC_DATA_SIZE <= 2)
    {
        memcpy((unsigned char *)&ack_data, ((unsigned char *)header) + sizeof(Header),
               (header->length - EXC_DATA_SIZE));
        if (ack_data == ACK_ACTIVE_NEW_DEVICE)
        {
            sdk->ui->btn_coreActive->setText("New Device");
        }
        else
        {
            if (ack_data == ACK_ACTIVE_SUCCESS)
            {
                sdk->ui->btn_coreActive->setText("Activation Okay");
            }
            else
            {
                sdk->ui->btn_coreActive->setText("Activation Error");
            }
        }
    }
    else
    {
        sdk->ui->btn_coreActive->setText("Decode Error");
    }
    This->activateCallback(This, header);
}

void DJIonboardSDK::hotpintReadCallback(CoreAPI *This, Header *header, UserData userData)
{
    DJIonboardSDK *sdk = (DJIonboardSDK *)userData;
    HotPoint::readCallback(This, header, sdk->hp);
    API_LOG(This->getDriver(), STATUS_LOG, "Refreshing data");
    sdk->ui->le_hp_la->setText(QString::number(sdk->hp->getData().latitude));
    sdk->ui->le_hp_lo->setText(QString::number(sdk->hp->getData().longitude));
    sdk->ui->le_hp_al->setText(QString::number(sdk->hp->getData().height));
    sdk->ui->le_hp_pa->setText(QString::number(sdk->hp->getData().yawRate));
    sdk->ui->le_hp_ra->setText(QString::number(sdk->hp->getData().radius));
    sdk->ui->cb_hp_cl->setChecked(sdk->hp->getData().clockwise ? true : false);
    sdk->ui->cb_hp_yaw->setCurrentIndex(sdk->hp->getData().yawMode);
}

void DJIonboardSDK::on_btn_portRefresh_clicked()
{
    refreshPort();
}

void DJIonboardSDK::refreshPort()
{
    /*i->comboBox_portName->clear();
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    QStringList list;
    for (int i = 0; i < ports.length(); ++i)
    {
        list.append(ports[i].portName());
    }
    ui->comboBox_portName->addItems(list);*/
    QList<QSerialPortInfo> portsInfo = QSerialPortInfo::availablePorts();
    ports.clear();
    for (int i = 0; i < portsInfo.length(); ++i)
    {
        ports.append(portsInfo[i].portName());
    }
}

void DJIonboardSDK::setBaudrate()
{
    int baudrate = ui->lineEdit_portBaudrate->text().toInt();
    driver->setBaudrate(baudrate);
}

void DJIonboardSDK::setPort()
{
    //port->setPortName(ui->comboBox_portName->currentText());
    port->setPortName(SDKCOM);
    //port->setPortName("COM4");
}

void DJIonboardSDK::openPort()
{
    driver->init();
    if (port->isOpen())
        ui->btn_portOpen->setText(port->portName().append(" is open"));
    else
        ui->btn_portOpen->setText(port->portName().append(" not exit"));
}

void DJIonboardSDK::closePort()
{
    port->close();
    if (!port->isOpen())
    {
        ui->btn_portOpen->setText(port->portName().append(" closed"));
    }
}

//GPRSPortSet
void DJIonboardSDK::setGPRSBaudrate()
{
    int baudrate = ui->lineEdit_GPRSportBaudrate->text().toInt();
    GPRSdriver->setBaudrate(baudrate);
}

void DJIonboardSDK::setGPRSPort()
{
    GPRSport->setPortName(GPRSCOM);
}

void DJIonboardSDK::openGPRSPort()
{
    GPRSdriver->init();
    if(GPRSport->isOpen())
        ui->btn_GPRSportOpen->setText(GPRSport->portName().append(" is open"));
    else
        ui->btn_GPRSportOpen->setText(GPRSport->portName().append(" not exit"));
}

void DJIonboardSDK::closeGPRSPort()
{
    GPRSport->close();
    if (!GPRSport->isOpen())
    {
        ui->btn_GPRSportOpen->setText(GPRSport->portName().append(" closed"));
    }
}

void DJIonboardSDK::GPRSDataRead()
{
    int GPRSBUF_len=0,cnt=0;
    QString head="Rev: ";

    char* tmp=(char*)malloc(sizeof(char)*BUFFER_SIZE);

    if(GPRSBUF.isEmpty())
    {
        GPRSBUF_len=GPRSdriver->charreadall(tmp,BUFFER_SIZE);
    }

    if (GPRSBUF_len >0)
    {
        for(int i=0;i<GPRSBUF_len-1;i++)
        {
            if(tmp[i]==0x0d&&tmp[i+1]==0x0a)
            {
                cnt++;
                if(cnt>2)
                {
                    for(int j=i;j<GPRSBUF_len;j++)
                    {
                        tmp[j]='\0';
                    }
                }
            }
        }
        tmp[GPRSBUF_len]='\0';
        GPRSBUF=QString(QLatin1String(tmp));
        ui->tb_GPRSDisplay->append(head.append(GPRSBUF));
    }
}

void DJIonboardSDK::GPRSPortCtl()
{
    static int cnt=0;
    ui->lineEdit_GPRSres->setText(QString(QString::number(GPRSflag)));
    if(GPRSst==0)
    {
        if(cnt==0)
        {
            emit GPRSDataSend(GPRSCommand[GPRSflag]);
            cnt++;
        }
        else if(GPRSBUF.contains("OK",Qt::CaseSensitive)&&GPRSBUF.contains(GPRSCommand[GPRSflag],Qt::CaseSensitive)&&GPRSflag<5)
        {
            GPRSflag++;
            cnt=0;
            if(GPRSflag>4)
                GPRSst=1;
        }
        else if(cnt>2)
        {
            ui->lineEdit_GPRSres->setText("connect failed");
            if(GPRSBUF.contains("ERROR",Qt::CaseSensitive)&&GPRSBUF.contains(GPRSCommand[GPRSflag],Qt::CaseSensitive))
            {
                GPRSflag=6;
                emit GPRSDataSend(GPRSCommand[GPRSflag]);
            }
            cnt++;
            if(GPRSBUF.contains("SHUT OK",Qt::CaseSensitive))
            {
                GPRSst=0;
                cnt=0;
                GPRSflag=0;
                emit GPRSDataSend("ATE1");
            }
            else if(cnt>5)
            {
                emit GPRSDataSend(GPRSCommand[6]);
            }
        }
        else //if(!GPRSBUF.isEmpty())
        {
            emit GPRSDataSend(GPRSCommand[GPRSflag]);
            cnt++;  
        }
    }
    else if(GPRSst==1)
    {
        if(cnt==0)
        {
            emit GPRSDataSend(GPRSCommand[5]);
            cnt++;
        }
        else if(GPRSBUF.contains("CONNECT OK",Qt::CaseSensitive)||GPRSBUF.contains("ALREADY CONNECT",Qt::CaseSensitive))
        {
            GPRSst=2;
            cnt=0;
            emit GPRSDataSend("AT+CIPSPRT=2");
            emit GPRSDataSend("ATE0");
        }
        else if((GPRSBUF.contains("ERROR",Qt::CaseSensitive)&&GPRSBUF.contains(GPRSCommand[GPRSflag],Qt::CaseSensitive)))
        {
            emit GPRSDataSend(GPRSCommand[GPRSflag]);
            cnt++;
            if(cnt>2)
            {
                ui->lineEdit_GPRSres->setText("connect failed");
                GPRSflag=6;
                emit GPRSDataSend(GPRSCommand[GPRSflag]);
                cnt++;
                if(GPRSBUF.contains("SHUT OK",Qt::CaseSensitive)||cnt>5)
                {
                    GPRSst=0;
                    cnt=0;
                    GPRSflag=0;
                    emit GPRSDataSend("ATE1");
                }
            }
        }
        else if(GPRSBUF.contains("STATE",Qt::CaseSensitive))
        {
            emit GPRSDataSend(GPRSCommand[6]);
            cnt++;
        }
        else if(GPRSBUF.contains("SHUT OK",Qt::CaseSensitive))
        {
            GPRSst=0;
            cnt=0;
            GPRSflag=0;
            emit GPRSDataSend("ATE1");
        }
    }
    else if(GPRSst==2)
    {
        GPRSConnectflag=1;
        ui->lineEdit_GPRSres->setText("connect ok");
        //st=3;
    }
    GPRSBUF.clear();
}

//GPRS PROTOCOL COMMAND
void DJIonboardSDK::GPRSProtocolRead()
{
    if(GPRSConnectflag==1&&GPRSBUF!=NULL)
    //if(GPRSBUF!=NULL)
    {
        //GPRSBUF.remove(">");
        //GPRSBUF.remove("SEND OK");
        //GPRSBUF=GPRSBUF.simplified();
        if(!GPRSBUF.contains("SEND",Qt::CaseSensitive))
        {
            char X=GPRSBUF[0].toLatin1();
            for(int i=1;i<GPRSBUF.length()-1;i++)
            {
                X^=GPRSBUF[i].toLatin1();
            }
            QStringList Protocol=GPRSBUF.split("=");
            if(Protocol.length()>2&&Protocol[1].contains("Z",Qt::CaseSensitive))
            {
                //QString tmp=ProtocolHead;
                ProtocolHead=Protocol[0];

                char head=Protocol[2][0].toLatin1();
                char tail=Protocol[Protocol.length()-1][0].toLatin1();
                switch(head)
                {
                case 'P': //飞行器参数状态查询
                    ProtocolFlag[0]=false;
                    if(Protocol.length()<5)
                    {
                        if(tail==X&&Protocol.length()>3)
                        {

                            //GPRSProtocolSend_0
                            ProtocolFlag[0]=true;

                        }
                        else
                        {
                            ProtocolFlag[0]=false;
                            GPRSProtocolSend_0('N');
                        }
                    }
                    else
                    {
                        ProtocolFlag[0]=false;
                    }
                    break;
                case 'S'://飞行器参数设置
                    ProtocolFlag[1]=false;
                    if(Protocol.length()>8)
                    {
                        if(tail==X)
                        {
                            FlightStatusSet.Height=Protocol[3].toDouble();
                            FlightStatusSet.v=Protocol[4].toDouble();
                            plp->goHome.height=Protocol[5].toDouble();
                            plp->goHomeSpeed=Protocol[6].toDouble();
                            FlightStatusSet.distance=Protocol[7].toDouble();
                            plp->avoidDistanceFront=FlightStatusSet.distance;
                            ProtocolFlag[1]=true;
                            GPRSProtocolSend_1('Y');
                        }
                        else
                        {
                            ProtocolFlag[1]=false;
                            GPRSProtocolSend_1('N');
                        }
                    }
                    else
                    {
                        ProtocolFlag[1]=false;
                        GPRSProtocolSend_1('N');
                    }
                    break;
                case 'D'://飞行器路径点设置
                    ProtocolFlag[2]=false;
                    if(tail==X&&Protocol.length()>5)
                    {
                        FlightDirectSet.pointnumber=Protocol[3].toInt();
                        if((Protocol.length()-4)>FlightDirectSet.pointnumber)
                        {
                            if(FlightDirectSet.pointData!=NULL)
                            {
                                delete []FlightDirectSet.pointData;
                                FlightDirectSet.pointData=NULL;
                            }
                            FlightDirectSet.pointData=new point[FlightDirectSet.pointnumber];
                            for(int i=0;i<FlightDirectSet.pointnumber;i++)
                            {
                                QStringList DirectInfo=Protocol[4+i].split("|");
                                if(i==0)
                                {
                                    FlightDirectSet.pointData[0].Lon=api->getBroadcastData().pos.longitude*RAD2DEG+DirectInfo[0].toDouble()/1000000000;
                                    FlightDirectSet.pointData[0].Lat=api->getBroadcastData().pos.latitude*RAD2DEG+DirectInfo[1].toDouble()/1000000000;
                                }
                                else
                                {
                                    FlightDirectSet.pointData[i].Lon=FlightDirectSet.pointData[i-1].Lon+DirectInfo[0].toDouble()/1000000000;
                                    FlightDirectSet.pointData[i].Lat=FlightDirectSet.pointData[i-1].Lat+DirectInfo[1].toDouble()/1000000000;
                                    //FlightDirectSet.pointData[i].Height=DirectInfo[2].toDouble();
                                    //GPRSDataSend(QString::number(FlightDirectSet.pointData[i].Lon,'.',11));
                                }
                            }
                            //GPRSProtocolSend_2('Y');
                            ProtocolFlag[2]=true;
                        }
                        else
                        {
                            GPRSProtocolSend_2('N');
                            ProtocolFlag[2]=false;
                        }

                    }
                    else
                    {
                        GPRSProtocolSend_2('N');
                        ProtocolFlag[2]=false;
                    }
                    break;
                case 'C'://飞行器控制命令设置
                    ProtocolFlag[3]=false;
                    if(Protocol.length()<6)
                    {
                        if(tail==X)
                        {
                            CommandData=Protocol[3].toInt();
                            ProtocolFlag[3]=true;
                        }
                        else
                        {
                            ProtocolFlag[3]=false;
                            GPRSProtocolSend_3('N');
                        }
                    }
                    else
                    {
                        ProtocolFlag[3]=false;
                        //GPRSProtocolSend_3('N');
                    }
                    break;
                case 'E'://故障信息上传结果
                    ProtocolFlag[4]=false;
                    if(tail==X&&Protocol.length()<6)
                    {
                        if(Protocol[2][1].toLatin1()!='Y')
                        {
                             //GPRSProtocolSend_4
                            ProtocolFlag[4]=true;
                        }
                        else
                        {
                            ProtocolFlag[4]=false;
                        }
                    }
                    else
                    {
                         ProtocolFlag[4]=false;
                    }
                    break;
                case 'R': //飞行器返航点设置
                    ProtocolFlag[5]=false;
                    if(Protocol.length()<5)
                    {
                        if(tail==X&&Protocol.length()>3)
                        {

                            //GPRSProtocolSend_0
                            ProtocolFlag[5]=true;

                        }
                        else
                        {
                            ProtocolFlag[5]=false;
                            GPRSProtocolSend_7('N');
                        }
                    }
                    else
                    {
                        ProtocolFlag[5]=false;
                    }
                    break;
                default:
                    break;
                }
            }
            Protocol.clear();
            //GPRSBUF.clear();
        }
        GPRSBUF.clear();
    }
}

//发送飞行器状态信息（H,v,goHomeH,goHomev,dis）;协议解析结果（N）
void DJIonboardSDK::GPRSProtocolSend_0(double Height, double v, double goHomeHeight, double goHomeSpeed, double distance)
{
    char s=0x1a;

    //GPRSDataSend("AT+CIPSEND");
    //sleepmSec(1000);
    QString tmp=QString("AT+CIPSEND\r|")+ProtocolHead+"=U=P="+QString::number(Height,'.',2)+"="+QString::number(v,'.',1)+"="\
            +QString::number(goHomeHeight,'.',2)+"="+QString::number(goHomeSpeed,'.',1)+"="+QString::number(distance,'.',2)+"=";
    char X=tmp[0].toLatin1();
    for(int i=1;i<tmp.length();i++)
    {
        X^=tmp[i].toLatin1();
    }
    tmp+=X;

    QString temp(tmp.append("\r").append(s));
    emit GPRSDataSend(temp);
    //GPRSDataSend(QString(s));
}

void DJIonboardSDK::GPRSProtocolSend_0(char res)
{
    char s=0x1a;

    //GPRSDataSend("AT+CIPSEND");
    //sleepmSec(1000);
    QString tmp=QString("AT+CIPSEND\r|")+ProtocolHead+"=U=P="+QString(res)+"=";
    char X=tmp[0].toLatin1();
    for(int i=1;i<tmp.length();i++)
    {
        X^=tmp[i].toLatin1();
    }
    tmp+=X;

    QString temp(tmp.append("\r").append(s));
    emit GPRSDataSend(temp);
    //GPRSDataSend(QString(s));
}

//发送状态设置回复
void DJIonboardSDK::GPRSProtocolSend_1(char res)
{
    char s=0x1a;

    //GPRSDataSend("AT+CIPSEND");
    //sleepmSec(1000);
    QString tmp=QString("AT+CIPSEND\r|")+ProtocolHead+"=U=S="+QString(res)+"=";
    char X=tmp[0].toLatin1();
    for(int i=1;i<tmp.length();i++)
    {
        X^=tmp[i].toLatin1();
    }
    tmp+=X;

    QString temp(tmp.append("\r").append(s));
    emit GPRSDataSend(temp);
    //GPRSDataSend(QString(s));
}

//发送路径信息设置结果
void DJIonboardSDK::GPRSProtocolSend_2(char res)
{
    char s=0x1a;

    //GPRSDataSend("AT+CIPSEND");
    //sleepmSec(1000);
    QString tmp=QString("AT+CIPSEND\r|")+ProtocolHead+"=U=D="+QString(res)+"=";
    char X=tmp[0].toLatin1();
    for(int i=1;i<tmp.length();i++)
    {
        X^=tmp[i].toLatin1();
    }
    tmp+=X;

    QString temp(tmp.append("\r").append(s));
    emit GPRSDataSend(temp);
    //GPRSDataSend(QString(s));
}

//发送飞行器控制命令回复（CT,Res）;协议解析结果（N）
void DJIonboardSDK::GPRSProtocolSend_3(int CommandType, char res)
{
    char s=0x1a;

    //GPRSDataSend("AT+CIPSEND");
    //sleepmSec(1000);
    QString tmp=QString("AT+CIPSEND\r|")+ProtocolHead+"=U=C="+QString::number(CommandType,10)+"="+QString(res)+"=";
    char X=tmp[0].toLatin1();
    for(int i=1;i<tmp.length();i++)
    {
        X^=tmp[i].toLatin1();
    }
    tmp+=X;

    QString temp(tmp.append("\r").append(s));
    emit GPRSDataSend(temp);
    //GPRSDataSend(QString(s));
}

void DJIonboardSDK::GPRSProtocolSend_3(char res)
{
    char s=0x1a;

    //GPRSDataSend("AT+CIPSEND");
    //sleepmSec(1000);
    QString tmp=QString("AT+CIPSEND\r|")+ProtocolHead+"=U=C="+QString(res)+"="+QString(res)+"=";
    char X=tmp[0].toLatin1();
    for(int i=1;i<tmp.length();i++)
    {
        X^=tmp[i].toLatin1();
    }
    tmp+=X;

    QString temp(tmp.append("\r").append(s));
    emit GPRSDataSend(temp);
    //GPRSDataSend(QString(s));
}

//发送故障检测信息
void DJIonboardSDK::GPRSProtocolSend_4(int ErrorNum, QString ErrorType, double Lon, double Lat)
{
    char s=0x1a;

    //GPRSDataSend("AT+CIPSEND");
    //sleepmSec(1000);
    QTime b=QTime(0,0,0,0);
    QTime t=QTime::currentTime();
    QString Head=QString::number(b.msecsTo(t),10);
    QString tmp=QString("AT+CIPSEND\r|")+Head+"=U=E="+QString::number(ErrorNum,10)+"="+ErrorType+"="+\
            QString::number(Lon,'.',11)+"="+QString::number(Lat,'.',12)+"=";
    char X=tmp[0].toLatin1();
    for(int i=1;i<tmp.length();i++)
    {
        X^=tmp[i].toLatin1();
    }
    tmp+=X;

    QString temp(tmp.append("\r").append(s));
    emit GPRSDataSend(temp);
    //GPRSDataSend(QString(s));
}

//发送心跳数据
void DJIonboardSDK::GPRSProtocolSend_5(double Lon, double Lat, double height, double v, int status)
{
    char s=0x1a;

    //GPRSDataSend("AT+CIPSEND");
    //sleepmSec(1000);
    QString tmp=QString("AT+CIPSEND\r|")+"H=U=L="+QString::number(Lon,'.',11)+"="+QString::number(Lat,'.',12)+"="+\
                QString::number(height,'.',2)+"="+QString::number(v,'.',1)+"="+\
                QString::number(status,10)+"=";
    char X=tmp[0].toLatin1();
    for(int i=1;i<tmp.length();i++)
    {
        X^=tmp[i].toLatin1();
    }
    tmp+=X;

    QString temp(tmp.append("\r").append(s));
    emit GPRSDataSend(temp);
    //GPRSDataSend(QString(s));
}

//发送状态编码
void DJIonboardSDK::GPRSProtocolSend_6(QString StatusCode)
{
    char s=0x1a;

    //GPRSDataSend("AT+CIPSEND");
    //sleepmSec(1000);
    QTime b=QTime(0,0,0,0);
    QTime t=QTime::currentTime();
    QString Head=QString::number(b.msecsTo(t),10);
    QString tmp=QString("AT+CIPSEND\r|")+Head+"=U=T="+StatusCode+"=";
    char X=tmp[0].toLatin1();
    for(int i=1;i<tmp.length();i++)
    {
        X^=tmp[i].toLatin1();
    }
    tmp+=X;

    QString temp(tmp.append("\r").append(s));
    emit GPRSDataSend(temp);
    //GPRSDataSend(QString(s));
}

void DJIonboardSDK::GPRSProtocolSend_7(char res, double Lon, double Lat)
{
    char s=0x1a;

    //GPRSDataSend("AT+CIPSEND");
    //sleepmSec(1000);
    QString tmp=QString("AT+CIPSEND\r|")+ProtocolHead+"=U=R="+QString(res)+"="+\
            QString::number(Lon,'.',11)+"="+QString::number(Lat,'.',12)+"=";
    char X=tmp[0].toLatin1();
    for(int i=1;i<tmp.length();i++)
    {
        X^=tmp[i].toLatin1();
    }
    tmp+=X;

    QString temp(tmp.append("\r").append(s));
    emit GPRSDataSend(temp);
    //GPRSDataSend(QString(s));
}

void DJIonboardSDK::GPRSProtocolSend_7(char res)
{
    char s=0x1a;

    //GPRSDataSend("AT+CIPSEND");
    //sleepmSec(1000);
    QString tmp=QString("AT+CIPSEND\r|")+ProtocolHead+"=U=R="+QString(res)+"=";
    char X=tmp[0].toLatin1();
    for(int i=1;i<tmp.length();i++)
    {
        X^=tmp[i].toLatin1();
    }
    tmp+=X;

    QString temp(tmp.append("\r").append(s));
    emit GPRSDataSend(temp);
    //GPRSDataSend(QString(s));
}

void DJIonboardSDK::on_tmr_GPRS_autosend()
{
    //GPRSPortCtl();
}

void DJIonboardSDK::on_tmr_GPRS_autoread()
{
    GPRSDataRead();
    GPRSProtocolRead();
}

void DJIonboardSDK::on_btn_portOpen_clicked()
{
    if (port == 0)
        ;
    else
    {
        if (port->isOpen())
            closePort();
        else
        {
            setPort();
            setBaudrate();
            openPort();
        }
    }
}

void DJIonboardSDK::on_btn_GPRSportOpen_clicked()
{
    if(GPRSport==0)
        ;
    else
    {
        if(GPRSport->isOpen())
        {
            GPRSautoSend->stop();
            GPRSautoRead->stop();
            closeGPRSPort();
            GPRSConnectflag=0;
            gprs->stop();
        }
        else
        {
            setGPRSPort();
            setGPRSBaudrate();
            openGPRSPort();
            gprs->start();
            GPRSautoSend->start();
            GPRSautoRead->start(); 
            //ui->lineEdit_GPRSres->setText("connect ok");
        }
    }

}

void DJIonboardSDK::on_btn_GPRSportSend_clicked()
{
    if(autoSendStatus->isActive())
    {
        autoSendStatus->stop();
        ui->btn_GPRSportSend->setText(QString("stopped"));
    }
    else
    {
        autoSendStatus->start();
        ui->btn_GPRSportSend->setText(QString("started"));
    }
    /*CommandData=ui->lineEdit_GPRSres->text().toInt();
    ProtocolFlag[3]=true;*/
    //GPRSPortCtl();
    //GPRSDATA=ui->lineEdit_GPRSsend->text();
    //GPRSDataSend(GPRSDATA);
    //GPRSProtocolSend_0(120.13143165691,30.272977524721,20.12,1.0,1);
    //GPRSProtocolSend_4(1,"error",120.13143165691,30.272977524721);
    //if(GPRSConnectflag==1)
        //GPRSProtocolSend_5(api->getBroadcastData().pos.longitude*RAD2DEG,api->getBroadcastData().pos.latitude*RAD2DEG,api->getBroadcastData().pos.altitude,api->getBroadcastData().v.x,plp->plpstatus);
}

void DJIonboardSDK::on_btn_GPRSportRead_clicked()
{
    GPRSDataRead();
}

void DJIonboardSDK::on_btn_GPRSportClear_clicked()
{
    ui->tb_GPRSDisplay->clear();
}

void DJIonboardSDK::on_lineEdit_GPRSsend_textChanged(const QString &text)
{
    GPRSCommand[5]=QString("AT+CIPSTART=\"TCP\",\"").append(&text).append(QString("\",\"9876\""));
    if(GPRSConnectflag==1)
    {
        GPRSConnectflag=0;
        GPRSflag=6;
        GPRSDataSend(GPRSCommand[GPRSflag]);
    }
    GPRSst=0;
    GPRSflag=0;
}

void DJIonboardSDK::on_comboBox_portName_currentIndexChanged(int index)
{
    if (index != -1)
    {
        closePort();
        setPort();
        setBaudrate();
        closePort();
    }
}

void DJIonboardSDK::on_btn_coreActive_clicked()
{
    ActivateData data;
    data.ID = ui->lineEdit_ID->text().toInt();
    *key = ui->lineEdit_Key->text().toLocal8Bit();
    data.encKey = key->data();

    api->activate(&data, DJIonboardSDK::activationCallback, this);
}

void DJIonboardSDK::on_btn_coreVersion_clicked()
{
    api->getDroneVersion();
}

void DJIonboardSDK::on_btn_coreSetControl_clicked()
{
    if (ui->btn_coreSetControl->text() == "Release Control")
        api->setControl(false, DJIonboardSDK::setControlCallback, this);
    else
        api->setControl(true, DJIonboardSDK::setControlCallback, this);
}

void DJIonboardSDK::on_btn_VRC_resetAll_clicked()
{
    on_btn_VRC_resetRight_clicked();
    on_btn_VRC_resetLeft_clicked();
}

void DJIonboardSDK::on_btn_VRC_resetLeft_clicked()
{
    ui->slider_VRC_LH->setValue(1024);
    ui->slider_VRC_LV->setValue(1024);
}

void DJIonboardSDK::on_btn_VRC_resetRight_clicked()
{

    ui->slider_VRC_RH->setValue(1024);
    ui->slider_VRC_RV->setValue(1024);
}

void DJIonboardSDK::on_tmr_VRC_autosend()
{
    VirtualRCData data;
    data.throttle = ui->slider_VRC_LV->value();
    data.roll = ui->slider_VRC_RH->value();
    data.pitch = ui->slider_VRC_RV->value();
    data.yaw = ui->slider_VRC_LH->value();

    if (ui->btg_vrcMode->checkedButton()->text() == "F")
        data.mode = 496;
    else if (ui->btg_vrcMode->checkedButton()->text() == "A")
        data.mode = 1024;
    else
        data.mode = 1552;

    if (ui->btg_vrcGear->checkedButton()->text() == "Up")
        data.gear = 1684;
    else
        data.gear = 1324;

    vrc->sendData(data);
}

void DJIonboardSDK::updateFlightFlag()
{
    ui->lineEdit_flightFlag->clear();
    ui->lineEdit_flightFlag->setText(QString::number(flightFlag, 16));
}

void DJIonboardSDK::on_btg_flight_HL(QAbstractButton *button)
{
    QString name = button->text();
    flightFlag &= 0x3F;
    if (name == "Angle")
        flightFlag |= Flight::HORIZONTAL_ANGLE;
    else if (name == "Velocity")
        flightFlag |= Flight::HORIZONTAL_VELOCITY;
    else if (name == "Position")
        flightFlag |= Flight::HORIZONTAL_POSITION;
    updateFlightFlag();
}

void DJIonboardSDK::on_btg_flight_VL(QAbstractButton *button)
{
    QString name = button->text();
    flightFlag &= 0xCF;
    if (name == "Thrust")
        flightFlag |= Flight::VERTICAL_THRUST;
    else if (name == "Velocity")
        flightFlag |= Flight::VERTICAL_VELOCITY;
    else if (name == "Position")
        flightFlag |= Flight::VERTICAL_POSITION;
    updateFlightFlag();
}

void DJIonboardSDK::on_btg_flight_YL(QAbstractButton *button)
{
    QString name = button->text();
    flightFlag &= 0xF7;
    if (name == "Angle")
        flightFlag |= Flight::YAW_ANGLE;
    else
        flightFlag |= Flight::YAW_RATE;
    updateFlightFlag();
}

void DJIonboardSDK::on_btg_flight_CL(QAbstractButton *button)
{
    QString name = button->text();
    flightFlag &= 0xF9;
    if (name == "Ground")
        flightFlag |= Flight::HORIZONTAL_GROUND;
    else
        flightFlag |= Flight::HORIZONTAL_BODY;
    updateFlightFlag();
}

void DJIonboardSDK::on_btg_flight_SM(QAbstractButton *button)
{
    if (api->getSDKVersion() != versionM100_23)
    {
        QString name = button->text();
        flightFlag &= 0xFE;
        if (name == "Disable")
            flightFlag |= Flight::SMOOTH_DISABLE;
        else
            flightFlag |= Flight::SMOOTH_ENABLE;
        updateFlightFlag();
    }
    else
    {
        ui->groupBox9->setDisabled(true);
    }
}

void DJIonboardSDK::updateFlightX()
{
    ui->lineEdit_flight_X->setText(QString::number(flightX));
}

void DJIonboardSDK::updateFlightY()
{
    ui->lineEdit_flight_Y->setText(QString::number(flightY));
}

void DJIonboardSDK::updateFlightZ()
{
    ui->lineEdit_flight_Z->setText(QString::number(flightZ));
}

void DJIonboardSDK::updateFlightYaw()
{
    ui->lineEdit_flight_Yaw->setText(QString::number(flightYaw));
}

void DJIonboardSDK::resetFlightData()
{
    flightX = 0;
    flightY = 0;
    flightZ = 0;
    flightYaw = 0;
    updateFlightX();
    updateFlightY();
    updateFlightZ();
    updateFlightYaw();
}

void DJIonboardSDK::on_btn_flight_frount_pressed()
{
    flightX += 0.1f;
    updateFlightX();
}

void DJIonboardSDK::on_btn_flight_back_pressed()
{
    flightX -= 0.1f;
    updateFlightX();
}

void DJIonboardSDK::flightSend()
{
    qDebug() << flightFlag << flightX << flightY << flightZ << flightYaw;
    flight->control(flightFlag, flightX, flightY, flightZ, flightYaw);
}

/*void DJIonboardSDK::on_btn_flight_send_clicked()
{
    flightSend();
	guidanceTest();
}*/



void DJIonboardSDK::filght_autosend()
{
    flightSend();
}

void DJIonboardSDK::on_btn_flight_runTask_clicked()
{
    QString name = ui->btg_flightTask->checkedButton()->text();
    Flight::TASK type = Flight::TASK_GOHOME;
    if (name == "Take off")
        type = Flight::TASK_TAKEOFF;
    else if (name == "Landing")
        type = Flight::TASK_LANDING;

    flight->task(type);
}

void DJIonboardSDK::on_btn_flight_arm_clicked(bool checked)
{

    if (checked)       {
            flight->setArm(checked);
            ui->btn_flight_arm->setText("Disarm");
    }
    else
    {
            flight->setArm(checked);
            ui->btn_flight_arm->setText("Arm");
    }

}

void DJIonboardSDK::on_btn_flight_left_pressed()
{
    flightY += 0.1f;
    updateFlightY();
}

void DJIonboardSDK::on_btn_flight_right_pressed()
{
    flightY -= 0.1f;
    updateFlightY();
}

void DJIonboardSDK::on_btn_flight_down_pressed()
{
    flightZ -= 0.1f;
    updateFlightZ();
}

void DJIonboardSDK::on_btn_flight_up_pressed()
{
    flightZ += 0.1f;
    updateFlightZ();
}

void DJIonboardSDK::on_btn_flight_leftYaw_pressed()
{
    flightYaw += 1.0f;
    updateFlightYaw();
}

void DJIonboardSDK::on_btn_flight_rightYaw_pressed()
{
    flightYaw -= 1.0f;
    updateFlightYaw();
}

void DJIonboardSDK::on_btn_flight_dataReset_clicked()
{
    resetFlightData();
}

void DJIonboardSDK::on_lineEdit_flight_X_returnPressed()
{
    flightX = ui->lineEdit_flight_X->text().toFloat();
}

void DJIonboardSDK::on_lineEdit_flight_Y_returnPressed()
{
    flightY = ui->lineEdit_flight_Y->text().toFloat();
}

void DJIonboardSDK::on_lineEdit_flight_Z_returnPressed()
{
    flightZ = ui->lineEdit_flight_Z->text().toFloat();
}

void DJIonboardSDK::on_lineEdit_flight_Yaw_returnPressed()
{
    flightYaw = ui->lineEdit_flight_Yaw->text().toFloat();
}

void DJIonboardSDK::on_cb_flight_autoSend_clicked(bool checked)
{
    if (checked)
        autoSend->start();
    else
        autoSend->stop();
}

void DJIonboardSDK::on_btn_virtualRC_send_clicked()
{
    vrc->setControl(true, VirtualRC::CutOff_ToRealRC);
    vrcSend->start();
    vrc->sendData();
}

void DJIonboardSDK::on_btn_virtualRC_init_clicked()
{
    vrc->setControl(true, VirtualRC::CutOff_ToRealRC);
}

void DJIonboardSDK::on_btn_coreSet_clicked()
{
    uint8_t data[16];
    data[0] = ui->comboBox_1->currentIndex();
    data[1] = ui->comboBox_2->currentIndex();
    data[2] = ui->comboBox_3->currentIndex();
    data[3] = ui->comboBox_4->currentIndex();
    data[4] = ui->comboBox_5->currentIndex();
    data[5] = ui->comboBox_6->currentIndex();
    data[6] = ui->comboBox_7->currentIndex();
    if (api->getSDKVersion() == versionM100_31)
    {
        data[7] = ui->comboBox_8->currentIndex();
        data[8] = ui->comboBox_9->currentIndex();
        data[9] = ui->comboBox_10->currentIndex();
        data[10] = ui->comboBox_11->currentIndex();
        data[11] = ui->comboBox_12->currentIndex();
        data[12] = 0;
        data[13] = 0;
        data[14] = 0;
        data[15] = 0;
    }
    else if (api->getSDKVersion() == versionA3_31)
    {
        data[7] = ui->comboBox_13->currentIndex(); // rtk
        data[8] = ui->comboBox_14->currentIndex(); // gps
        data[9] = ui->comboBox_8->currentIndex();
        data[10] = ui->comboBox_9->currentIndex();
        data[11] = ui->comboBox_10->currentIndex();
        data[12] = ui->comboBox_11->currentIndex();
        data[13] = ui->comboBox_12->currentIndex();
        data[14] = 0;
        data[15] = 0;
    }
    api->setBroadcastFreq(data);
}

void DJIonboardSDK::on_btn_vrc_down_pressed()
{
    ui->slider_VRC_RV->setValue(ui->slider_VRC_RV->value() - 10);
}

void DJIonboardSDK::on_btn_vrc_up_pressed()
{
    ui->slider_VRC_RV->setValue(ui->slider_VRC_RV->value() + 10);
}

void DJIonboardSDK::on_btn_vrc_left_pressed()
{
    ui->slider_VRC_RH->setValue(ui->slider_VRC_RH->value() - 10);
}

void DJIonboardSDK::on_btn_vrc_right_pressed()
{
    ui->slider_VRC_RH->setValue(ui->slider_VRC_RH->value() + 10);
}

void DJIonboardSDK::on_btn_vrc_w_pressed()
{
    ui->slider_VRC_LV->setValue(ui->slider_VRC_LV->value() + 10);
}

void DJIonboardSDK::on_btn_vrc_S_pressed()
{
    ui->slider_VRC_LV->setValue(ui->slider_VRC_LV->value() - 10);
}

void DJIonboardSDK::on_btn_vrc_A_pressed()
{
    ui->slider_VRC_LH->setValue(ui->slider_VRC_LH->value() - 10);
}

void DJIonboardSDK::on_btn_vrc_D_pressed()
{
    ui->slider_VRC_LH->setValue(ui->slider_VRC_LH->value() + 10);
}

void DJIonboardSDK::on_btr_camera_speed_clicked()
{
    ui->hs_camera_yaw->setMinimum(-1800);
    ui->hs_camera_yaw->setMaximum(1800);
    ui->hs_camera_roll->setMinimum(-1800);
    ui->hs_camera_roll->setMaximum(1800);
    ui->hs_camera_pitch->setMinimum(-1800);
    ui->hs_camera_pitch->setMaximum(1800);
    resetCameraAngle();
    ui->gb_cameraFlag->setEnabled(false);
}

void DJIonboardSDK::updateCameraFlag()
{
    ui->lineEdit_cameraFlag->clear();
    ui->lineEdit_cameraFlag->setText(QString::number(camFlag, 16));
}

void DJIonboardSDK::on_btg_cameraAngle(QAbstractButton *button)
{
    camFlag &= 0xFE;
    if (button->text() == "Absolute")
        camFlag |= 0x01;
    updateCameraFlag();
}

void DJIonboardSDK::on_btg_cameraYaw(QAbstractButton *button)
{
    camFlag &= 0xFD;
    if (button->text() == "Disable")
        camFlag |= 0x02;
    updateCameraFlag();
}

void DJIonboardSDK::on_btg_cameraRoll(QAbstractButton *button)
{
    camFlag &= 0xFB;
    if (button->text() == "Disable")
        camFlag |= 0x04;
    updateCameraFlag();
}

void DJIonboardSDK::on_btg_cameraPitch(QAbstractButton *button)
{
    camFlag &= 0xF7;
    if (button->text() == "Disable")
        camFlag |= 0x08;
    updateCameraFlag();
}

void DJIonboardSDK::resetCameraAngle()
{
    ui->hs_camera_yaw->setValue(0);
    ui->hs_camera_roll->setValue(0);
    ui->hs_camera_pitch->setValue(0);
}

void DJIonboardSDK::on_btr_camera_angle_clicked()
{
    ui->hs_camera_yaw->setMinimum(-3200);
    ui->hs_camera_yaw->setMaximum(3200);
    ui->hs_camera_roll->setMinimum(-350);
    ui->hs_camera_roll->setMaximum(350);
    ui->hs_camera_pitch->setMinimum(-900);
    ui->hs_camera_pitch->setMaximum(300);
    ui->gb_cameraFlag->setEnabled(true);
    resetCameraAngle();
}

void DJIonboardSDK::on_btn_cameraRecord_clicked(bool checked)
{
    if (checked)
    {
        ui->btn_cameraRecord->setText("Stop");
        cam->setCamera(Camera::CODE_CAMERA_VIDEO_START);
    }
    else
    {
        ui->btn_cameraRecord->setText("Record");
        cam->setCamera(Camera::CODE_CAMERA_VIDEO_STOP);
    }
}

void DJIonboardSDK::on_btn_cameraShoot_clicked()
{
    cam->setCamera(Camera::CODE_CAMERA_SHOT);
}

void DJIonboardSDK::on_btn_camera_send_clicked()
{
    GimbalSpeedData speedData;
    GimbalAngleData angleData;
    if (ui->btg_cameraMode->checkedButton()->text() == "Speed")
    {
        speedData.yaw = ui->hs_camera_yaw->value();
        speedData.roll = ui->hs_camera_roll->value();
        speedData.pitch = ui->hs_camera_pitch->value();
        cam->setGimbalSpeed(&speedData);
    }
    else
    {
        angleData.yaw = ui->hs_camera_yaw->value();
        angleData.roll = ui->hs_camera_roll->value();
        angleData.pitch = ui->hs_camera_pitch->value();
        angleData.mode = camFlag;
        angleData.duration = ui->lineEdit_cameraTime->text().toInt();
        cam->setGimbalAngle(&angleData);
    }
}

void DJIonboardSDK::on_btn_camera_0_clicked()
{
    resetCameraAngle();
}

void DJIonboardSDK::on_btn_camera_8_pressed()
{
    ui->hs_camera_pitch->setValue(ui->hs_camera_pitch->value() + 5);
}

void DJIonboardSDK::on_btn_camera_2_pressed()
{
    ui->hs_camera_pitch->setValue(ui->hs_camera_pitch->value() - 5);
}

void DJIonboardSDK::on_btn_camera_7_pressed()
{
    ui->hs_camera_roll->setValue(ui->hs_camera_roll->value() - 5);
}

void DJIonboardSDK::on_btn_camera_9_pressed()
{
    ui->hs_camera_roll->setValue(ui->hs_camera_roll->value() + 5);
}

void DJIonboardSDK::on_btn_camera_4_pressed()
{
    ui->hs_camera_yaw->setValue(ui->hs_camera_yaw->value() - 5);
}

void DJIonboardSDK::on_btn_camera_6_pressed()
{
    ui->hs_camera_yaw->setValue(ui->hs_camera_yaw->value() + 5);
}

void DJIonboardSDK::on_tmr_Camera_autosend()
{
    on_btn_camera_send_clicked();
}

void DJIonboardSDK::on_cb_camera_send_clicked(bool checked)
{
    if (checked)
        cameraSend->start();
    else
        cameraSend->stop();
}

void DJIonboardSDK::upDateTime()
{
    if (api->getSDKVersion() == versionM100_23)
    {
        ui->le_coreTimeStamp->setText(QString::number(api->getTime().time));
    }
    else
    {
        ui->le_coreTimeStamp->setText(QString::number(api->getTime().time));
        ui->le_coreNanoStamp->setText(QString::number(api->getTime().nanoTime));
        ui->le_coreSyncFlag->setText(QString::number(api->getTime().syncFlag));
    }
}

void DJIonboardSDK::upDateCapacity()
{
    ui->le_coreCapacity->setText(QString::number(api->getBatteryCapacity()));
}

void DJIonboardSDK::on_btn_coreRead_clicked()
{
    upDateTime();
    upDateCapacity();
    upDateFlightStatus();
    updateControlDevice();
}

void DJIonboardSDK::upDateFlightStatus()
{
    ui->le_coreFlightStatus->setText(QString::number((api->getFlightStatus())));
}

void DJIonboardSDK::updateControlDevice()
{
    ui->le_coreControlDevice->setText(QString::number((api->getCtrlInfo().deviceStatus)));
}

void DJIonboardSDK::on_tmr_Broadcast()
{
    //! @note this function cost too much time to run.
    //! it is better run outside the broadcastCallback.
    if (ui->cb_coreTimeStamp->isChecked())
        upDateTime();
    if (ui->cb_coreCapacity->isChecked())
        upDateCapacity();
    if (ui->cb_coreFlightStatus->isChecked())
        upDateFlightStatus();
    if (ui->cb_coreControlDevice->isChecked())
        updateControlDevice();
    if (ui->cb_cameraYaw->isChecked())
        updateCameraYaw();
    if (ui->cb_cameraRoll->isChecked())
        updateCameraRoll();
    if (ui->cb_cameraPitch->isChecked())
        updateCameraPitch();
    if (ui->cb_vrcData->isChecked())
        updateVirturalRCData();
    if (ui->cb_FlightA->isChecked())
        updateFlightAcc();
    if (ui->cb_FlightP->isChecked())
        updateFlightPal();
    if (ui->cb_FlightM->isChecked())
        updateFlightMagnet();
    if (ui->cb_FlightQ->isChecked())
        updateFlightQuaternion();
    if (ui->cb_FlightV->isChecked())
        updateFlightVelocity();
    if (ui->cb_FlightPos->isChecked())
        updateFlightPosition();
    if (ui->cb_gps->isChecked())
        updateGPS();
    if (ui->cb_rtk->isChecked())
        updateRTK();

#ifdef SDK_DEV
    dev->updateBroadcast();
#endif

    ui->tb_display->verticalScrollBar()->setValue(
        ui->tb_display->verticalScrollBar()->maximum());
    ui->tb_display->repaint();

    // qDebug()<<api->getFilter().recvIndex;
}

void DJIonboardSDK::updateCameraYaw()
{
    ui->le_cameraYaw->setText(QString::number(cam->getYaw()));
    if (cam->isYawLimit())
        ui->cb_cameraYawLimit->setChecked(true);
    else
        ui->cb_cameraYawLimit->setChecked(false);
}

void DJIonboardSDK::updateCameraRoll()
{
    ui->le_cameraRoll->setText(QString::number(cam->getRoll()));
    if (cam->isRollLimit())
        ui->cb_cameraRollLimit->setChecked(true);
    else
        ui->cb_cameraRollLimit->setChecked(false);
}

void DJIonboardSDK::updateCameraPitch()
{
    ui->le_cameraPitch->setText(QString::number(cam->getPitch()));
    if (cam->isPitchLimit())
        ui->cb_cameraPitchLimit->setChecked(true);
    else
        ui->cb_cameraPitchLimit->setChecked(false);
}

void DJIonboardSDK::updateVirturalRCData()
{
    ui->le_vrcYaw->setText(QString::number(vrc->getRCData().yaw));
    ui->le_vrcRoll->setText(QString::number(vrc->getRCData().roll));
    ui->le_vrcPitch->setText(QString::number(vrc->getRCData().pitch));
    ui->le_vrcThrottle->setText(QString::number(vrc->getRCData().throttle));
    ui->le_vrcMode->setText(QString::number(vrc->getRCData().mode));
    ui->le_vrcGear->setText(QString::number(vrc->getRCData().gear));
}

void DJIonboardSDK::updateFlightAcc()
{
    ui->le_Flight_accx->setText(QString::number(flight->getAcceleration().x));
    ui->le_Flight_accy->setText(QString::number(flight->getAcceleration().y));
    ui->le_Flight_accz->setText(QString::number(flight->getAcceleration().z));
}

void DJIonboardSDK::updateFlightPal()
{
    ui->le_Flight_palx->setText(QString::number(flight->getYawRate().x));
    ui->le_Flight_paly->setText(QString::number(flight->getYawRate().y));
    ui->le_Flight_palz->setText(QString::number(flight->getYawRate().z));
}

void DJIonboardSDK::updateFlightMagnet()
{
    ui->le_Flight_magx->setText(QString::number(flight->getMagnet().x));
    ui->le_Flight_magy->setText(QString::number(flight->getMagnet().y));
    ui->le_Flight_magz->setText(QString::number(flight->getMagnet().z));
}

void DJIonboardSDK::updateFlightQuaternion()
{
    ui->le_Flight_Q0->setText(QString::number(flight->getQuaternion().q0));
    ui->le_Flight_Q1->setText(QString::number(flight->getQuaternion().q1));
    ui->le_Flight_Q2->setText(QString::number(flight->getQuaternion().q2));
    ui->le_Flight_Q3->setText(QString::number(flight->getQuaternion().q3));
}

void DJIonboardSDK::updateFlightVelocity()
{
    ui->le_Flight_Vx->setText(QString::number(flight->getVelocity().x));
    ui->le_Flight_Vy->setText(QString::number(flight->getVelocity().y));
    ui->le_Flight_Vz->setText(QString::number(flight->getVelocity().z));
    ui->le_Flight_VS->setText(QString::number(flight->getVelocity().sensorID));
    ui->le_Flight_VH->setText(QString::number(flight->getVelocity().health));
}

void DJIonboardSDK::updateFlightPosition()
{
    ui->le_Flight_PosH->setText(QString::number(flight->getPosition().height));
    ui->le_Flight_PosLa->setText(QString::number(flight->getPosition().latitude));
    ui->le_Flight_PosLo->setText(QString::number(flight->getPosition().longitude));
    ui->le_Flight_PosAl->setText(QString::number(flight->getPosition().altitude));
    ui->le_Flight_PosHealth->setText(QString::number(flight->getPosition().health));
}

void DJIonboardSDK::updateGPS()
{
    ui->le_gps_date->setText(QString::number(api->getBroadcastData().gps.date));
    ui->le_gps_time->setText(QString::number(api->getBroadcastData().gps.time));
    ui->le_gps_longitude->setText(QString::number(api->getBroadcastData().gps.longitude));
    ui->le_gps_latitude->setText(QString::number(api->getBroadcastData().gps.latitude));
    ui->le_gps_hmsl->setText(QString::number(api->getBroadcastData().gps.Hmsl));
    ui->le_gps_VN->setText(QString::number(api->getBroadcastData().gps.velocityNorth));
    ui->le_gps_VE->setText(QString::number(api->getBroadcastData().gps.velocityEast));
    ui->le_gps_VD->setText(QString::number(api->getBroadcastData().gps.velocityGround));
    //ui->le_gps_hdop->setText(QString::number(api->getBroadcastData().gps.Hdop));
    //ui->le_gps_pdop->setText(QString::number(api->getBroadcastData().gps.Pdop));
    /*
    ui->le_gps_GNSSFlag->setText(QString::number(api->getBroadcastData().gps.GNSSFlag));
    ui->le_gps_gnssFix->setText(QString::number(api->getBroadcastData().gps.GNSSFix));
    ui->le_gps_hacc->setText(QString::number(api->getBroadcastData().gps.hacc));
    ui->le_gps_sacc->setText(QString::number(api->getBroadcastData().gps.sacc));
    ui->le_gps_guse->setText(QString::number(api->getBroadcastData().gps.GPSUsed));
    ui->le_gps_gnuse->setText(QString::number(api->getBroadcastData().gps.GNSSUsed));
    ui->le_gps_svn->setText(QString::number(api->getBroadcastData().gps.SVNum));
    ui->le_gps_gps->setText(QString::number(api->getBroadcastData().gps.GPSStatus));
    */
}

void DJIonboardSDK::updateRTK()
{
    ui->le_rtk_date->setText(QString::number(api->getBroadcastData().rtk.date));
    ui->le_rtk_time->setText(QString::number(api->getBroadcastData().rtk.time));
    ui->le_rtk_longitude->setText(QString::number(api->getBroadcastData().rtk.longitude));
    ui->le_rtk_latitude->setText(QString::number(api->getBroadcastData().rtk.latitude));
    ui->le_rtk_hmsl->setText(QString::number(api->getBroadcastData().rtk.Hmsl));
    //ui->le_rtk_longitude_2->setText(QString::number(api->getBroadcastData().rtk.longitudeS));
    //ui->le_rtk_latitude_2->setText(QString::number(api->getBroadcastData().rtk.latitudeS));
    //ui->le_rtk_hmsl_2->setText(QString::number(api->getBroadcastData().rtk.HmslS));
    //ui->le_rtk_VN->setText(QString::number(api->getBroadcastData().rtk.velocityNorth));
    //ui->le_rtk_VE->setText(QString::number(api->getBroadcastData().rtk.velocityEast));
    ui->le_rtk_VD->setText(QString::number(api->getBroadcastData().rtk.velocityGround));
    ui->le_rtk_yaw->setText(QString::number(api->getBroadcastData().rtk.yaw));
    //ui->le_rtk_SVNS->setText(QString::number(api->getBroadcastData().rtk.SVNS));
    //ui->le_rtk_SVNP->setText(QString::number(api->getBroadcastData().rtk.SVNP));
    /*
    ui->le_rtk_hdop->setText(QString::number(api->getBroadcastData().rtk.Hdop));
    ui->le_rtk_pdop->setText(QString::number(api->getBroadcastData().rtk.Pdop));
    ui->le_rtk_flag_1->setText(QString::number(api->getBroadcastData().rtk.posFlag[0]));
    ui->le_rtk_flag_2->setText(QString::number(api->getBroadcastData().rtk.posFlag[1]));
    ui->le_rtk_flag_3->setText(QString::number(api->getBroadcastData().rtk.posFlag[2]));
    ui->le_rtk_flag_4->setText(QString::number(api->getBroadcastData().rtk.posFlag[3]));
    ui->le_rtk_flag_5->setText(QString::number(api->getBroadcastData().rtk.posFlag[4]));
    ui->le_rtk_flag_6->setText(QString::number(api->getBroadcastData().rtk.posFlag[5]));
    ui->le_rtk_status->setText(QString::number(api->getBroadcastData().rtk.status));
    ui->le_rtk_new->setText(QString::number(api->getBroadcastData().rtk.rtkNew));
    */
}

QStandardItemModel *DJIonboardSDK::initAction()
{
    QStandardItemModel *action = new QStandardItemModel();
    action->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("No.")));
    action->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Index")));
    action->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("Type")));
    action->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("Data")));
    return action;
}

void DJIonboardSDK::on_btn_vrcRead_clicked()
{
    updateVirturalRCData();
}

void DJIonboardSDK::on_btn_FlightAcc_clicked()
{
    updateFlightAcc();
}

void DJIonboardSDK::on_btn_FlightPal_clicked()
{
    updateFlightPal();
}

void DJIonboardSDK::on_btn_FlightMag_clicked()
{
    updateFlightMagnet();
}

void DJIonboardSDK::on_btn_FlightVel_clicked()
{
    updateFlightVelocity();
}

void DJIonboardSDK::on_btn_FlightQua_clicked()
{
    updateFlightQuaternion();
}

void DJIonboardSDK::on_btn_FlightPos_clicked()
{
    updateFlightPosition();
}

void DJIonboardSDK::on_btn_cameraRead_clicked()
{
    updateCameraYaw();
    updateCameraRoll();
    updateCameraPitch();
}

void DJIonboardSDK::on_btn_hotPoint_start_clicked()
{
    hp->setHotPoint(ui->le_hp_lo->text().toFloat()*DEG2RAD, ui->le_hp_la->text().toFloat()*DEG2RAD,
                    ui->le_hp_al->text().toFloat());
    hp->setClockwise(ui->cb_hp_cl->isChecked());
    hp->setYawRate(ui->le_hp_pa->text().toFloat());
    hp->setRadius(ui->le_hp_ra->text().toFloat());
    hp->setYawMode((HotPoint::YawMode)ui->cb_hp_yaw->currentIndex());
    hp->start();
}

void DJIonboardSDK::on_btn_hp_setPal_clicked()
{
    hp->updateYawRate(ui->le_hp_pa->text().toFloat(), ui->cb_hp_cl->isChecked());
}

void DJIonboardSDK::on_btn_hotPoint_current_clicked()
{
    ui->le_hp_lo->setText(QString::number(flight->getPosition().longitude/DEG2RAD));
    ui->le_hp_la->setText(QString::number(flight->getPosition().latitude/DEG2RAD));
    ui->le_hp_al->setText(QString::number(flight->getPosition().altitude));

}

void DJIonboardSDK::on_cb_mission_hp_clicked(bool checked)
{
    api->setHotPointData(checked);
}

void DJIonboardSDK::on_cb_mission_wp_clicked(bool checked)
{
    api->setWayPointData(checked);
}

void DJIonboardSDK::on_cb_mission_follow_clicked(bool checked)
{
    api->setFollowData(checked);
}

void DJIonboardSDK::on_btn_hotPoint_stop_clicked()
{
    hp->stop();
}

void DJIonboardSDK::on_btn_hp_data_clicked()
{
    hp->readData(hotpintReadCallback, this);
}

void DJIonboardSDK::on_btn_hp_setRadius_clicked()
{
    hp->updateRadius(ui->le_hp_ra->text().toFloat());
}

void DJIonboardSDK::on_btn_hp_setYaw_clicked()
{
    ui->cb_hp_yaw->setCurrentIndex(1);
    hp->resetYaw();
}

void DJIonboardSDK::on_btn_follow_current_clicked()
{
    ui->le_follow_la->setText(QString::number(0.0));
    ui->le_follow_lo->setText(QString::number(0.0));
    ui->le_follow_hi->setText(QString::number(0.0));
    ui->le_follow_an->setText(QString::number(0.0));
    ui->hs_follow_al->setValue(0);
    ui->hs_follow_lo->setValue(0);
    ui->hs_follow_la->setValue(0);
    ui->hs_follow_an->setValue(0);
}

void DJIonboardSDK::on_hs_follow_la_valueChanged(int value)
{
    ui->le_follow_la->setText(QString::number(value / 10000.0));
}

void DJIonboardSDK::on_hs_follow_lo_valueChanged(int value)
{
    ui->le_follow_lo->setText(QString::number(value / 10000.0));
}

void DJIonboardSDK::on_hs_follow_al_valueChanged(int value)
{
    ui->le_follow_hi->setText(QString::number(value / 100.0));
}

void DJIonboardSDK::on_hs_follow_an_valueChanged(int value)
{
    ui->le_follow_an->setText(QString::number(value / 114.59156));
}

void DJIonboardSDK::on_btn_follow_update_clicked()
{
    on_tmr_follow_send();
}

void DJIonboardSDK::on_btn_follow_stop_clicked()
{
    follow->stop();
    followSend->stop();
}

void DJIonboardSDK::on_btn_follow_pause_clicked(bool checked)
{
    if (checked)
        ui->btn_follow_pause->setText("Resume");
    else
        ui->btn_follow_pause->setText("Pause");
    follow->pause(checked);
}

void DJIonboardSDK::on_btn_follow_start_clicked()
{
    follow->start();
    followSend->start();
}

void DJIonboardSDK::on_tmr_follow_send()
{
    follow->updateTarget(ui->le_follow_la->text().toDouble() + flight->getPosition().latitude,
                         ui->le_follow_lo->text().toDouble() + flight->getPosition().longitude,
                         ui->le_follow_hi->text().toInt(), ui->le_follow_an->text().toInt());
}

void DJIonboardSDK::wpAddPoint()
{
    if(FlightDirectSet.pointnumber&&plp->isUsingGPRSData)
    {
        while(ui->cb_waypoint_point->count() != 1)
        {
            wpRemovePoint();
        }
        ui->le_waypoint_number->setText(QString::number(waypointData->rowCount()));
        for(int number = 0;number<FlightDirectSet.pointnumber;number++)
        {
            waypointData->setItem(number, 0, new QStandardItem(QString::number(number)));
            waypointData->setItem(number, 1,
                                  new QStandardItem(QString::number(FlightDirectSet.pointData[number].Lat,'g',11)));
            waypointData->setItem(number, 2,
                                  new QStandardItem(QString::number(FlightDirectSet.pointData[number].Lon,'g',11)));
            waypointData->setItem(number, 3,
                                  new QStandardItem(QString::number(FlightStatusSet.Height,'g',11)));
            waypointData->setItem(number, 4, new QStandardItem("not available now"));
            waypointData->setItem(number, 5, new QStandardItem("0"));
            waypointData->setItem(number, 6, new QStandardItem("0"));
            waypointData->setItem(number, 7, new QStandardItem("Clockwise"));
            waypointData->setItem(number, 8, new QStandardItem("0"));

            actionData.append(initAction());
            ui->cb_waypoint_point->addItem(QString::number(ui->cb_waypoint_point->count() - 1));
        }
    }
    else
    {
        int number = waypointData->rowCount();
        waypointData->setItem(number, 0, new QStandardItem(QString::number(number)));
        waypointData->setItem(number, 1,
                              new QStandardItem(QString::number(flight->getPosition().latitude/DEG2RAD,'g',11)));
        waypointData->setItem(number, 2,
                              new QStandardItem(QString::number(flight->getPosition().longitude/DEG2RAD,'g',11)));
        waypointData->setItem(number, 3,
                              new QStandardItem(QString::number(flight->getPosition().altitude,'g',11)));
        waypointData->setItem(number, 4, new QStandardItem("not available now"));
        waypointData->setItem(number, 5, new QStandardItem("0"));
        waypointData->setItem(number, 6, new QStandardItem("0"));
        waypointData->setItem(number, 7, new QStandardItem("Clockwise"));
        waypointData->setItem(number, 8, new QStandardItem("0"));

        actionData.append(initAction());
        ui->cb_waypoint_point->addItem(QString::number(ui->cb_waypoint_point->count() - 1));
    }
}

void DJIonboardSDK::on_btn_waypoint_init_clicked()
{
    WayPointInitData data = wp->getInfo();
    data.indexNumber = ui->le_waypoint_number->text().toInt();
    data.maxVelocity = ui->le_wp_mv->text().toFloat();
    data.idleVelocity = ui->le_wp_iv->text().toFloat();
    data.finishAction = ui->cb_wp_fa->currentIndex();
    data.executiveTimes = ui->le_wp_exec->text().toInt();
    data.yawMode = ui->cb_wp_yaw->currentIndex();
    //! @note not available for flight control yet.
    data.traceMode = 0; // ui->cb_wp_mov->currentIndex()
    data.RCLostAction = ui->cb_wp_rcl->currentIndex();
    data.gimbalPitch = ui->cb_wp_gb->currentIndex();
    data.latitude = ui->le_wp_la->text().toDouble();
    data.longitude = ui->le_wp_lo->text().toDouble();
    data.altitude = ui->le_wp_al->text().toFloat();
    //! @note these are two different way to offer a same init.
    // wp->setInfo(data);
    // wp->init();
    if(!plp->Missionclicked)
        wp->init(&data);
    else
        plp->init(&data);
    //on_btn_waypoint_add_clicked();
#ifdef GROUNDSTATION
    initMap();
#endif // GROUNDSTATION
}

void DJIonboardSDK::on_waypoint_data_changed(const QModelIndex &topLeft __UNUSED,
                                             const QModelIndex &bottomRight __UNUSED,
                                             const QVector<int> &roles __UNUSED)
{
    //! @todo waypoint data modify
    //    API_LOG(api->getDriver(), STATUS_LOG, "c: %d r: %d %s", bottomRight.column(),
    //          bottomRight.row(), topLeft.data().toByteArray().data());
    //    API_LOG(api->getDriver(), STATUS_LOG, "c: %d r: %d %lf", bottomRight.column(),
    //        bottomRight.row(), topLeft.data().toDouble());
}

void DJIonboardSDK::wpRemovePoint()
{
    if (ui->cb_waypoint_point->count() != 1)
    {
        ui->cb_waypoint_point->removeItem(ui->cb_waypoint_point->count() - 1);
        QStandardItemModel *last = actionData.last();
        actionData.removeLast();
        delete last;
        waypointData->removeRow(waypointData->rowCount() - 1);
    }
}

void DJIonboardSDK::initSDK()
{
    port = new QSerialPort(this);
    GPRSport = new QSerialPort(this);
    driver = new QtOnboardsdkPortDriver(port);
    GPRSdriver = new QtOnboardsdkPortDriver(GPRSport);
    api = new CoreAPI(driver);
    key = new QByteArray;
    flight = new Flight(api);
    follow = new Follow(api);
    vrc = new VirtualRC(api);
    cam = new Camera(api);
    hp = new HotPoint(api);
    wp = new WayPoint(api);
    plp = new PowerLinePatrol(api,flight);
    gprs=new GPRSSendMessage(GPRSdriver,GPRSport,ui->tb_GPRSDisplay);
    ss = new socketServer();
    connect(this,SIGNAL(GPRSDataSend(const QString&)),gprs,SLOT(GPRSSignalRev(const QString&)));
    connect(this,SIGNAL(abortEmit(const QString&)),plp,SLOT(abortSignalSlot(const QString&)));
    connect(plp,SIGNAL(emitLog(const QString&)),this,SLOT(logSignalRecv(const QString&)));
    connect(ss,SIGNAL(emitMalfunction(const QString&)),this,SLOT(malfunctionSlot(const QString&)));
    refreshPort();
    setPort();
    setBaudrate();
    setGPRSPort();
    setGPRSBaudrate();
    ss->start();
}

void DJIonboardSDK::on_cb_waypoint_point_currentIndexChanged(int index)
{
    if (index != 0)
    {
        if (actionData.length() >= (index - 1))
        {
            ui->tv_waypoint_actions->setModel(actionData[index - 1]);
            currentAction = actionData[index - 1];
        }
    }
    else
    {
        ui->tv_waypoint_actions->setModel(nullAction);
        currentAction = nullAction;
    }
}

void DJIonboardSDK::on_btn_log_clean_clicked()
{
    ui->tb_display->clear();
}

void DJIonboardSDK::on_btn_log_save_clicked()
{
    //! @todo save the file, not just pretending did it.
    QString data = QFileDialog::getSaveFileName(this, "Save", QString(), "Text (*.txt)");
    API_LOG(api->getDriver(), STATUS_LOG, "%s", data.toLatin1().data());
}

void DJIonboardSDK::on_btn_waypoint_action_clicked()
{
    if (currentAction != nullAction)
    {
        int number = currentAction->rowCount();
        if (number <= 15 && ui->cb_waypoint_point->currentIndex() != -1)
        {
            currentAction->setItem(number, 0, new QStandardItem(QString::number(number)));
            currentAction->setItem(number, 1, new QStandardItem(QString::number(
                                                  ui->cb_waypoint_point->currentIndex() - 1)));
            waypointData->setData(
                waypointData->index(ui->cb_waypoint_point->currentIndex() - 1, 8),
                currentAction->rowCount());
        }
        else
        {
            API_LOG(api->getDriver(), STATUS_LOG, "The maximum number of actions is 15");
        }
    }
}

void DJIonboardSDK::on_btn_waypoint_reset_clicked()
{
    while (actionData.length() != 0)
    {
        on_btn_waypoint_remove_clicked();
    }
}

void DJIonboardSDK::on_btn_waypoint_removeAction_clicked()
{
    //! @todo implement removeAction
}

void DJIonboardSDK::on_btn_core_setSync_clicked()
{
    api->setSyncFreq(ui->le_coreSyncFeq->text().toInt());
}

void DJIonboardSDK::on_le_waypoint_number_editingFinished()
{
    while (ui->le_waypoint_number->text().toInt() != waypointData->rowCount())
    {
        if (ui->le_waypoint_number->text().toInt() > waypointData->rowCount())
            wpAddPoint();
        else
            wpRemovePoint();
    }
}

void DJIonboardSDK::on_btn_waypoint_viewPoint_clicked()
{
    ui->le_wp_la->setText(QString::number(flight->getPosition().latitude/DEG2RAD));
    ui->le_wp_lo->setText(QString::number(flight->getPosition().longitude/DEG2RAD));
    ui->le_wp_al->setText(QString::number(flight->getPosition().altitude));
}

void DJIonboardSDK::on_btn_wp_ivset_clicked()
{
    wp->updateIdleVelocity(ui->le_wp_iv->text().toFloat());
}

void DJIonboardSDK::on_btn_wp_ivRead_clicked() { wp->readIdleVelocity(); }

void DJIonboardSDK::on_btn_waypoint_add_clicked()
{
    wpAddPoint();
    ui->le_waypoint_number->setText(QString::number(waypointData->rowCount()));
    //    waypointData->item(waypointData->rowCount()-1,1)->setText(QString::number(10.0));//api->getBroadcastData().pos.latitude);
    //    waypointData->item(waypointData->rowCount()-1,2)->setText(QString::number(10.0));//api->getBroadcastData().pos.longitude);
    //    waypointData->item(waypointData->rowCount()-1,3)->setText(QString::number(10.0));//api->getBroadcastData().pos.altitude);
}

void DJIonboardSDK::on_btn_waypoint_remove_clicked()
{
    wpRemovePoint();
    ui->le_waypoint_number->setText(QString::number(waypointData->rowCount()));
    plp->isUsingGPRSData=false;
}

void DJIonboardSDK::on_btn_wp_pr_clicked(bool checked)
{
    if (checked)
        ui->btn_wp_pr->setText("Resume");
    else
        ui->btn_wp_pr->setText("Pause");
    wp->pause(checked);
}

void DJIonboardSDK::on_le_wp_exec_editingFinished()
{
    //! @note the range of exectimes is 1-255, 0xFF means infinite loop
    int times = ui->le_wp_exec->text().toInt();
    if (times > 255)
        ui->le_wp_exec->setText("255");
    if (times <= 0)
        ui->le_wp_exec->setText("1");
}

void DJIonboardSDK::on_btn_wp_start_stop_clicked(bool checked)
{
     wp->start();
/*
    if (checked)
    {
        wp->start();
        ui->btn_wp_start_stop->setText("Stop");
    }
    else
    {
        wp->stop();
        ui->btn_wp_start_stop->setText("Start");
    }
*/

}

void DJIonboardSDK::on_btn_wp_loadOne_clicked()
{
    if (ui->cb_waypoint_point->currentIndex() != 0)
    {
        int index = ui->cb_waypoint_point->currentIndex() - 1;
        //WayPointData data;
        wayPointDataTmp.index = index;
        wayPointDataTmp.latitude = waypointData->index(index, 1).data().toDouble()*DEG2RAD;
        //qDebug()<<waypointData->index(index, 1).data();
        wayPointDataTmp.longitude = waypointData->index(index, 2).data().toDouble()*DEG2RAD;
        //qDebug()<<waypointData->index(index, 2).data();
        wayPointDataTmp.altitude = waypointData->index(index, 3).data().toDouble();
        wayPointDataTmp.damping = 0; //! @note not available now

        wayPointDataTmp.yaw = waypointData->index(index, 5).data().toInt();
        wayPointDataTmp.gimbalPitch = waypointData->index(index, 6).data().toInt();
        wayPointDataTmp.turnMode =
            waypointData->index(index, 7).data().toByteArray() == "Clockwise" ? 0 : 1;

        wayPointDataTmp.hasAction = 0;
        wayPointDataTmp.actionTimeLimit = 0;
        wayPointDataTmp.actionNumber = 0;
        wayPointDataTmp.actionRepeat = 0;
        for (int i = 0; i < 16; ++i)
        {
            wayPointDataTmp.commandList[i] = 0;
            wayPointDataTmp.commandParameter[i] = 0;
        }

        if (waypointData->index(index, 8).data().toInt() != 0)
        {
            wayPointDataTmp.hasAction = 1;
            wayPointDataTmp.actionNumber = waypointData->index(index, 8).data().toInt();
            wayPointDataTmp.actionTimeLimit = waypointData->index(index, 9).data().toInt();
            wayPointDataTmp.actionRepeat = waypointData->index(index, 10).data().toInt();
            for (int i = 0; i < 15; ++i)
            {
                if (i < actionData[index]->rowCount())
                {
                    ActionDelegate temp;
                    QComboBox *find = static_cast<QComboBox *>(
                        temp.createEditor(0, QStyleOptionViewItem(), QModelIndex()));
                    wayPointDataTmp.commandList[i] = find->findText(
                        actionData[index]->index(i, 2).data().toByteArray().data());
                    wayPointDataTmp.commandParameter[i] = actionData[index]->index(i, 3).data().toInt();
                }
                else
                    break;
            }
        }
        //! @note test code
        //        qDebug() << wayPointDataTmp.index << wayPointDataTmp.latitude << wayPointDataTmp.longitude << wayPointDataTmp.altitude
        //                 << wayPointDataTmp.damping;
        //        qDebug() << wayPointDataTmp.yaw << wayPointDataTmp.gimbalPitch << wayPointDataTmp.turnMode;
        //        qDebug() << wayPointDataTmp.hasAction << wayPointDataTmp.actionTimeLimit << wayPointDataTmp.actionNumber
        //                 << wayPointDataTmp.actionRepeat;
        //        for (int i = 0; i < 15; ++i)
        //            qDebug() << wayPointDataTmp.commandList[i] << wayPointDataTmp.commandParameter[i];
        if(!plp->Missionclicked)
        {
            if (!wp->uploadIndexData(&wayPointDataTmp))
                qDebug() << "fail";
        }
        else
        {
            if(!plp->uploadIndexData(&wayPointDataTmp))
                qDebug() << "fail";
        }
    }
}

void DJIonboardSDK::on_btn_wp_loadAll_clicked()
{
    for (int i = 0; i < ui->cb_waypoint_point->count() - 1; ++i)
    {
        ui->cb_waypoint_point->setCurrentIndex(i + 1);
        on_btn_wp_loadOne_clicked();
    }
}

void DJIonboardSDK::on_btn_hp_pause_clicked(bool checked)
{
    if (checked)
        ui->btn_hp_pause->setText("Resume");
    else
        ui->btn_hp_pause->setText("Pause");
    hp->pause(checked);
}

void DJIonboardSDK::on_cb_core_mechine_activated(int index)
{
    switch (index)
    {
        case 0:
            api->setVersion(versionM100_31);
            break;
        case 1:
            api->setVersion(versionM100_23);
            break;
        case 2:
            api->setVersion(versionA3_31);
            break;
    }
    functionAlloc();
}

void DJIonboardSDK::on_btn_mobile_clicked()
{
    if (ui->le_mobile->text().length() < 100)
        api->sendToMobile((uint8_t *)ui->le_mobile->text().toLatin1().data(),
                          ui->le_mobile->text().length());
    else
        API_LOG(api->getDriver(), ERROR_LOG, "too much data for mobile sending.\n");
}

void DJIonboardSDK::on_btn_gps_read_clicked()
{
    updateGPS();
}

void DJIonboardSDK::on_btn_rtk_read_clicked()
{
    updateRTK();
}

void DJIonboardSDK::on_btn_webTool_clicked(bool checked)
{
    if (checked)
    {
        ui->gb_wp->setMaximumHeight(20);
        ui->gb_wp->setMinimumHeight(20);
    }
    else
    {
        ui->gb_wp->setMinimumHeight(480);
        ui->gb_wp->setMaximumHeight(480);
    }
}

void DJIonboardSDK::on_lineEdit_flight_X_editingFinished()
{
    on_lineEdit_flight_X_returnPressed();
}

void DJIonboardSDK::on_lineEdit_flight_Y_editingFinished()
{
    on_lineEdit_flight_Y_returnPressed();
}

void DJIonboardSDK::on_lineEdit_flight_Z_editingFinished()
{
    on_lineEdit_flight_Z_returnPressed();
}

void DJIonboardSDK::on_lineEdit_flight_Yaw_editingFinished()
{
    on_lineEdit_flight_Yaw_returnPressed();
}

#ifdef GROUNDSTATION
void DJIonboardSDK::initGroundStation()
{
    webView = new QWebEngineView(this);
    QHBoxLayout *weblayout = new QHBoxLayout();
    ui->widget_web->setLayout(weblayout);
    weblayout->addWidget(webView);
}

void DJIonboardSDK::initMap()
{
    QString java;
    java.append("init(");
    java.append(QString::number(api->getBroadcastData().pos.latitude));
    java.append(",");
    java.append(QString::number(api->getBroadcastData().pos.longitude));
    java.append(");");
    webView->page()->runJavaScript(java);
}

void DJIonboardSDK::on_btn_webLoad_clicked()
{
    webView->load(QUrl("http://dji-brainhole.github.io/groundmelon/"));
}

void DJIonboardSDK::on_btn_webRefresh_clicked()
{
    initMap();
}

#endif // GROUNDSTATION

void DJIonboardSDK::on_btn_AbortWaypoint_clicked()
{

    wp->stop();

}

void DJIonboardSDK::on_btn_plp_init_clicked()
{
    GPRSProtocolSend_6(QString("4001"));
    plp->Missionclicked=true;
    on_btn_waypoint_init_clicked();
    sprintf(DJI::onboardSDK::buffer, "%s","PLPMission, Waypoint inited");
    api->serialDevice->displayLog();
}

void DJIonboardSDK::on_btn_plp_loadAll_clicked()
{
    if(plp->isRunning)
    {
        GPRSProtocolSend_2('N');
        GPRSProtocolSend_6(QString("4003"));
        sprintf(DJI::onboardSDK::buffer, "%s","PLPMission, Waypoint loaded fialed, mission is running");
        api->serialDevice->displayLog();
        return;
    }
    if(FlightDirectSet.pointnumber&&plp->isUsingGPRSData)
    {
        on_btn_waypoint_add_clicked();
        plp->Missionclicked=false;
        GPRSProtocolSend_2('Y');
    }
    if(plp->Missionclicked==false)
        on_btn_plp_init_clicked();
    for (int i = 0; i < ui->cb_waypoint_point->count() - 1; ++i)
    {
        ui->cb_waypoint_point->setCurrentIndex(i + 1);
        on_btn_wp_loadOne_clicked();
    }
    GPRSProtocolSend_6(QString("4002"));
    plp->plpstatus=2;
    plp->isLoadWayPoint=true;
    plp->Missionclicked=false;
    sprintf(DJI::onboardSDK::buffer, "%s","PLPMission, Waypoint loaded");
    api->serialDevice->displayLog();
}

void DJIonboardSDK::on_btn_plp_start_stop_clicked()
{
    if(plp->isLoadWayPoint&&!plp->isRunning)
    {
        sprintf(DJI::onboardSDK::buffer, "%s","PLPMission, Running...");
        api->serialDevice->displayLog();
        plp->isRunning=true;
        GPRSProtocolSend_6(QString("3000"));
        //plpMission();
        plp->start();
    }
}

void DJIonboardSDK::autoActivateGPRS()
{
    static int cnt=0;
    if(cnt==0)
    {
        if(!GPRSport->isOpen())
        {
            GPRSConnectflag=0;
            GPRSflag=0;
            GPRSst=0;
            on_btn_portRefresh_clicked();
            if(ports.contains(GPRSCOM))
            {
                setGPRSPort();
                on_btn_GPRSportOpen_clicked();
            }
            else
                ui->btn_GPRSportOpen->setText(GPRSport->portName().append(" not exit"));
        }
        else
        {
            cnt=1;
        }
    }
    else if(cnt==1)
    {
        if(!GPRSport->isOpen())
            cnt=0;
        else
        {
            GPRSPortCtl();
            if(GPRSConnectflag==1)
                cnt=2;
        }
    }
    else if(cnt==2)
    {
       cnt=0;
       activateGPRSTimer->stop();
       GPRSst=0;
       GPRSflag=0;
       if(flight->getStatus()==1)
           plp->plpstatus=1;
       GPRSautoRead->stop();
       GPRSautoRead->setInterval(2000);
       GPRSautoRead->start();
    }
}

void DJIonboardSDK::autoActivateSDK()
{
    static int cnt=0,origintime=0,cntin=0;
    int curtime=api->getTime().time;
    int tinterval=curtime-origintime;
    origintime=curtime;
    if(cnt==0){
        plp->isObtainControl=false;
        if(!port->isOpen()){
            on_btn_portRefresh_clicked();
            /*int findindex=-1;
            findindex=ui->comboBox_portName->findText(SDKCOM);
            if(findindex!=-1)*/
            if(ports.contains(SDKCOM))
            {
                //ui->comboBox_portName->setCurrentIndex(findindex);
                setPort();
                on_btn_portOpen_clicked();
            }
            else
                ui->btn_portOpen->setText(port->portName().append(" not exit"));
        }
        else
        {
            if(tinterval>0)
            {
                ui->btn_coreActive->setText("Activation");
                cnt=1;
            }
            else
            {
               port->close();
            }
        }
    }
    else if(cnt==1)
    {
        //auto activate
        if(!port->isOpen()||tinterval<=0)
        {
            cnt=0;
        }
        else
        {
            if(ui->btn_coreActive->text()=="Activation Okay")
                cnt=2;
            else
                on_btn_coreActive_clicked();
        }
    }
    else if(cnt==2)
    {
        if(!port->isOpen()||tinterval<=0)
        {
            cnt=0;
        }
        else
        {
            if(cntin==0||ui->btn_coreSetControl->text() != "Release Control")
                on_btn_coreSetControl_clicked();
            else
            {
                cnt=3;
                cntin=-1;
            }
            cntin++;
        }
    }
    else if(cnt==3)
    {
        if(!port->isOpen()||tinterval<=0)
        {
            cnt=0;
        }
        else
        {
            if(cntin==0)
            {
                if(ui->btn_coreSetControl->text() == "Release Control")
                    on_btn_coreSetControl_clicked();
                else
                {
                    cnt=0;
                    cntin=-1;
                }
            }
            else if(cntin==1)
            {
                if(ui->btn_coreSetControl->text() != "Release Control")
                    on_btn_coreSetControl_clicked();
                else
                {
                    cnt=0;
                    cntin=-1;
                }
            }
            else if(ui->btn_coreSetControl->text() == "Release Control")
            {
                plp->isObtainControl=true;
                activateSDKTimer->stop();
                cnt=0;
                cntin=-1;
                //GPRSProtocolSend_6(QString("2001"));
            }
            else
            {
                cnt=0;
                cntin=-1;
            }
            cntin++;
        }
    }
}

void DJIonboardSDK::on_btn_Abortplp_clicked()
{
    if(plp->isRunning){
        abortMission=true;
        emit abortEmit(QString("1"));
        GPRSProtocolSend_6(QString("4004"));
    }
    else
    {
        GPRSProtocolSend_6(QString("4005"));
        sprintf(DJI::onboardSDK::buffer, "%s","PLPMission, Abort failed, mission is not running");
        api->serialDevice->displayLog();
    }
}
void DJIonboardSDK::logSignalRecv(const QString &log)
{
    GPRSProtocolSend_6(log);
}
void DJIonboardSDK::malfunctionSlot(const QString &mal)
{
    static int cnt=1;
    PositionData curPos=api->getBroadcastData().pos;
    GPRSProtocolSend_4(cnt++, mal,curPos.longitude, curPos.latitude);//发送故障检测信息E
}

