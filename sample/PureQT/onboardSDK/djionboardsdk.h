#ifndef DJIONBOARDSDK_H
#define DJIONBOARDSDK_H

#include <QMainWindow>
#include <QAbstractButton>
#include <QTimer>
#include <QTextStream>
#ifdef GROUNDSTATION
#include <QWebEngineView>
#endif
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <QList>
#include <QFile>
#include <QComboBox>

#include <fstream>
#ifdef SDK_DEV
#include "uidev.h"
#endif
#include "QonboardSDK.h"
#include "powerlinepatrol.h"

#define   C_EARTH (double) 6378137.0
#define   DEG2RAD (double)0.01745329252
#define   RAD2DEG (double)57.29577951308
#define   SDKCOM  "COM6"
#define   GPRSCOM "COM3"
#define   ACTIVEPERIOD 1000
using namespace DJI;
using namespace DJI::onboardSDK;

namespace Ui
{
class DJIonboardSDK;
}

class DJIonboardSDK : public QMainWindow
{
    Q_OBJECT

  public:
    explicit DJIonboardSDK(QWidget *parent = 0);
    ~DJIonboardSDK();
    void releaseControl();

  private:
    void setBaudrate();
    void setGPRSBaudrate();
    void setPort();
    void setGPRSPort();
    void openPort();
    void openGPRSPort();
    void closePort();
    void closeGPRSPort();
    void refreshPort();
    void GPRSPortCtl();
    void GPRSDataRead();
    void GPRSDataSend(QString GPRSDATA);

    //GPRS PROTOCOL COMMAND
    void GPRSProtocolRead();
    void GPRSProtocolSend_0(double Height, double v);//发送飞行器状态信息P
    void GPRSProtocolSend_1(char res);//发送状态设置回复S
    void GPRSProtocolSend_2(char res);//发送路径信息设置结果D
    void GPRSProtocolSend_3(int CommandType,char res);//发送飞行器控制命令回复C
    void GPRSProtocolSend_4(int ErrorNum, QString ErrorType,double Lon, double Lan);//发送故障检测信息E
    void GPRSProtocolSend_5(double Lon,double Lan,double height,double v,int status);//发送心跳数据L
    void GPRSProtocolSend_6(QString StatusCode);//发送状态编码T

  protected:
    void closeEvent(QCloseEvent *);
    void timerEvent(QTimerEvent *);

    //! @note callback static class pointer and callback functions
  public:
    static void setControlCallback(CoreAPI *This, Header *header, UserData userData);
    static void activationCallback(CoreAPI *This, Header *header, UserData userData);
    static void hotpintReadCallback(CoreAPI *This, Header *header, UserData userData);

  private:
    void updateFlightAcc();
    void updateFlightPal();
    void updateFlightMagnet();
    void updateFlightVelocity();
    void updateFlightPosition();
    void updateFlightQuaternion();
    void updateVirturalRCData();
    void updateCameraPitch();
    void updateCameraRoll();
    void updateCameraYaw();
    void updateGPS();
    void updateRTK();

    QStandardItemModel *initAction();
    void wpAddPoint();
    void wpRemovePoint();

    void initSDK();
    void initFlight();
    void initCamera();
    void initFollow();
    void initDisplay();
    void initWayPoint();
    void initVirtualRC();
    void initGPRS();
    void plpMission();
    void localOffsetFromGpsOffset(DJI::Vector3dData& deltaNed, PositionData* target, PositionData* origin);
    int moveByPositionOffset(float32_t xOffsetDesired, float32_t yOffsetDesired, float32_t zOffsetDesired, float32_t yawDesired,
                             int timeoutInMs=60000, float yawThresholdInDeg=0.5, float posThresholdInCm=30.0);
    int moveByPositionBodyFrame(PositionData* targetPosition,int timeoutInMs=60000, float yawThresholdInDeg=0.5, float posThresholdInCm=30.0);
    int moveByYawRate(float32_t yawDesired, float32_t zDesired, int timeoutInMs=60000, float yawThresholdIndeg=0.5, float posDesiredInCm=10);
    int moveBySpeedBodyFrame(PositionData* targetPosition, int timeoutInMs=60000, float yawThresholdInDeg=0.5, float posThresholdInCm=35.0);
    void sleepmSec(int mSec);
    void mouseClicked(QWidget* wid);

  private slots:
    //! @note too much slots, tired to rename.
    void autoActivateSDK();
    void autoActivateGPRS();
    void on_tmr_autoSendStatus();
    void on_btn_portRefresh_clicked();
    void on_btn_portOpen_clicked();
    void on_comboBox_portName_currentIndexChanged(int index);

    void on_btn_coreSet_clicked();
    void on_btn_coreActive_clicked();
    void on_btn_coreVersion_clicked();
    void on_btn_coreSetControl_clicked();
    void on_btn_core_setSync_clicked();
    void on_btn_coreRead_clicked();
    void on_btn_mobile_clicked();
    void on_tmr_Broadcast();
    void on_cb_core_mechine_activated(int index);

    void upDateTime();
    void upDateCapacity();
    void upDateFlightStatus();
    void updateControlDevice();

    void on_btn_VRC_resetAll_clicked();
    void on_btn_VRC_resetLeft_clicked();
    void on_btn_VRC_resetRight_clicked();
    void on_btn_virtualRC_send_clicked();
    void on_btn_virtualRC_init_clicked();
    void on_btn_vrc_down_pressed();
    void on_btn_vrc_up_pressed();
    void on_tmr_VRC_autosend();
    void on_btn_vrc_w_pressed();
    void on_btn_vrc_A_pressed();
    void on_btn_vrc_S_pressed();
    void on_btn_vrc_D_pressed();
    void on_btn_vrcRead_clicked();
    void on_btn_vrc_left_pressed();
    void on_btn_vrc_right_pressed();

    void on_btn_cameraRead_clicked();
    void on_btn_camera_0_clicked();
    void on_btn_camera_8_pressed();
    void on_btn_camera_2_pressed();
    void on_btn_camera_7_pressed();
    void on_btn_camera_9_pressed();
    void on_btn_camera_4_pressed();
    void on_btn_camera_6_pressed();
    void on_tmr_Camera_autosend();
    void on_cb_camera_send_clicked(bool checked);

    void on_btn_cameraRecord_clicked(bool checked);
    void on_btn_cameraShoot_clicked();
    void on_btn_camera_send_clicked();

    void on_btr_camera_angle_clicked();
    void on_btr_camera_speed_clicked();
    void on_btg_cameraAngle(QAbstractButton *button);
    void on_btg_cameraYaw(QAbstractButton *button);
    void on_btg_cameraRoll(QAbstractButton *button);
    void on_btg_cameraPitch(QAbstractButton *button);
    void resetCameraAngle();
    void updateCameraFlag();

    void on_btn_flight_frount_pressed();
    void on_btn_flight_back_pressed();
    void on_btn_flight_send_clicked();
    void on_btn_flight_runTask_clicked();
    void on_btn_flight_arm_clicked(bool checked);
    void on_btn_flight_up_pressed();
    void on_btn_flight_left_pressed();
    void on_btn_flight_right_pressed();
    void on_btn_flight_down_pressed();
    void on_btn_flight_leftYaw_pressed();
    void on_btn_flight_rightYaw_pressed();
    void on_btg_flight_HL(QAbstractButton *button);
    void on_btg_flight_VL(QAbstractButton *button);
    void on_btg_flight_YL(QAbstractButton *button);
    void on_btg_flight_CL(QAbstractButton *button);
    void on_btg_flight_SM(QAbstractButton *button);

    void updateFlightFlag();
    void updateFlightX();
    void updateFlightY();
    void updateFlightZ();
    void updateFlightYaw();
    void resetFlightData();
    void flightSend();
    void on_btn_FlightAcc_clicked();
    void on_btn_FlightPal_clicked();
    void on_btn_FlightMag_clicked();
    void on_btn_FlightQua_clicked();
    void on_btn_FlightVel_clicked();
    void on_btn_FlightPos_clicked();

    void on_lineEdit_flight_X_editingFinished();
    void on_lineEdit_flight_Y_editingFinished();
    void on_lineEdit_flight_Z_editingFinished();
    void on_lineEdit_flight_Yaw_editingFinished();
    void on_lineEdit_flight_X_returnPressed();
    void on_lineEdit_flight_Y_returnPressed();
    void on_lineEdit_flight_Z_returnPressed();
    void on_lineEdit_flight_Yaw_returnPressed();
    void on_cb_flight_autoSend_clicked(bool checked);
    void on_btn_flight_dataReset_clicked();
    void filght_autosend();

    void on_btn_log_clean_clicked();
    void on_btn_log_save_clicked();

    void on_cb_mission_hp_clicked(bool checked);
    void on_cb_mission_wp_clicked(bool checked);
    void on_cb_mission_follow_clicked(bool checked);

    void on_btn_hotPoint_start_clicked();
    void on_btn_hotPoint_stop_clicked();
    void on_btn_hotPoint_current_clicked();
    void on_btn_hp_pause_clicked(bool checked);
    void on_btn_hp_setPal_clicked();
    void on_btn_hp_setRadius_clicked();
    void on_btn_hp_setYaw_clicked();
    void on_btn_hp_data_clicked();

    void on_btn_follow_current_clicked();
    void on_hs_follow_la_valueChanged(int value);
    void on_hs_follow_lo_valueChanged(int value);
    void on_hs_follow_al_valueChanged(int value);
    void on_hs_follow_an_valueChanged(int value);
    void on_btn_follow_update_clicked();
    void on_btn_follow_start_clicked();
    void on_btn_follow_pause_clicked(bool checked);
    void on_btn_follow_stop_clicked();
    void on_tmr_follow_send();

    void on_cb_waypoint_point_currentIndexChanged(int index);
    void on_le_waypoint_number_editingFinished();
    void on_btn_waypoint_action_clicked();
    void on_btn_waypoint_reset_clicked();
    void on_btn_waypoint_removeAction_clicked();
    void on_btn_waypoint_viewPoint_clicked();
    void on_btn_wp_ivset_clicked();
    void on_btn_wp_ivRead_clicked();
    void on_btn_waypoint_add_clicked();
    void on_btn_waypoint_remove_clicked();
    void on_btn_waypoint_init_clicked();
    void on_btn_wp_pr_clicked(bool checked);
    void on_le_wp_exec_editingFinished();
    void on_btn_wp_loadAll_clicked();
    void on_btn_wp_start_stop_clicked(bool checked);
    void on_btn_wp_loadOne_clicked();
    void on_waypoint_data_changed(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                                  const QVector<int> &roles __UNUSED);

    void on_btn_gps_read_clicked();
    void on_btn_rtk_read_clicked();

    void on_tmr_GPRS_autosend();
    void on_tmr_GPRS_autoread();

    void plpMissionCheck();

    //! @todo sort

    void on_btn_webTool_clicked(bool checked);

    void on_btn_AbortWaypoint_clicked();

    void on_btn_plp_init_clicked();

    void on_btn_plp_loadAll_clicked();

    void on_btn_plp_start_stop_clicked(bool checked);

    void on_btn_Abortplp_clicked();
	
    void on_btn_GPRSportOpen_clicked();

    void on_btn_GPRSportSend_clicked();

    void on_btn_GPRSportRead_clicked();

    void on_btn_GPRSportClear_clicked();

private:
#ifdef GROUNDSTATION
  private:
    void initGroundStation();
    void initMap();

  private slots:
    void on_btn_webLoad_clicked();
    void on_btn_webRefresh_clicked();

  private:
    QWebEngineView *webView;
#endif // GROUNDSTATION

  private:
    Ui::DJIonboardSDK *ui;

    CoreAPI *api;
    QtOnboardsdkPortDriver *driver;
    QSerialPort *port;

    //GPRS
    QtOnboardsdkPortDriver *GPRSdriver;
    QSerialPort *GPRSport;
    QByteArray *key;
    QString GPRSBUF;
    QString GPRSCommand[6];
    QTimer *GPRSautoSend;
    QTimer *GPRSautoRead;
    int GPRSflag;
    int GPRSst;
    int GPRSConnectflag;
    QString ProtocolHead;
    bool ProtocolFlag[5];/*协议解析结果
                          (0:飞行器参数状态查询;
                           1:飞行器参数设置;
                           2:导航路径设置;
                           3:飞行器控制命令设置;
                           4:故障信息上传结果)*/
    struct FlightStaSet{
        double Height;
        double v;
    }FlightStatusSet;//飞行高度和速度设置
    struct point{
        double Lon;
        double Lan;
        //double Height;
    };//路径点信息
    struct FlightDirSet{
        int pointnumber;
        struct point *pointData;
    }FlightDirectSet;
    int CommandData;/*飞行器控制命令类型
                     (1:起飞;
                      2:执行任务;
                      3:终止任务;
                      4:降落;
                      5:返航)*/

    Flight *flight;
    uint8_t flightFlag;
    float32_t flightX;
    float32_t flightY;
    float32_t flightZ;
    float32_t flightYaw;
    QTimer *autoSend;

    VirtualRC *vrc;
    QTimer *vrcSend;

    HotPoint *hp;

    Camera *cam;
    uint8_t camFlag;
    QTimer *cameraSend;

    Follow *follow;
    QTimer *followSend;

    WayPoint *wp;
    QStandardItemModel *waypointData;
    QStandardItemModel *currentAction;
    QStandardItemModel *nullAction;
    QList<QStandardItemModel *> actionData;

    QTimer *timerBroadcast;

    QTimer *activateSDKTimer;

    QTimer *activateGPRSTimer;

    QTimer *plpTimer;

    WayPointData wayPointDataTmp;

    PowerLinePatrol *plp;

    QStringList ports;

    float setspeed;

    QTimer *autoSendStatus;

    int plpstatus;



#ifdef SDK_DEV
    UIDev *dev;
#endif // SDK_DEV
    void functionAlloc();
};

#endif // DJIONBOARDSDK_H
