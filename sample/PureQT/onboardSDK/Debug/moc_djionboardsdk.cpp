/****************************************************************************
** Meta object code from reading C++ file 'djionboardsdk.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../djionboardsdk.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'djionboardsdk.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DJIonboardSDK_t {
    QByteArrayData data[169];
    char stringdata0[3989];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DJIonboardSDK_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DJIonboardSDK_t qt_meta_stringdata_DJIonboardSDK = {
    {
QT_MOC_LITERAL(0, 0, 13), // "DJIonboardSDK"
QT_MOC_LITERAL(1, 14, 12), // "GPRSDataSend"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 1), // "s"
QT_MOC_LITERAL(4, 30, 9), // "abortEmit"
QT_MOC_LITERAL(5, 40, 12), // "abortMission"
QT_MOC_LITERAL(6, 53, 15), // "autoActivateSDK"
QT_MOC_LITERAL(7, 69, 16), // "autoActivateGPRS"
QT_MOC_LITERAL(8, 86, 21), // "on_tmr_autoSendStatus"
QT_MOC_LITERAL(9, 108, 26), // "on_btn_portRefresh_clicked"
QT_MOC_LITERAL(10, 135, 23), // "on_btn_portOpen_clicked"
QT_MOC_LITERAL(11, 159, 40), // "on_comboBox_portName_currentI..."
QT_MOC_LITERAL(12, 200, 5), // "index"
QT_MOC_LITERAL(13, 206, 13), // "logSignalRecv"
QT_MOC_LITERAL(14, 220, 3), // "log"
QT_MOC_LITERAL(15, 224, 22), // "on_btn_coreSet_clicked"
QT_MOC_LITERAL(16, 247, 25), // "on_btn_coreActive_clicked"
QT_MOC_LITERAL(17, 273, 26), // "on_btn_coreVersion_clicked"
QT_MOC_LITERAL(18, 300, 29), // "on_btn_coreSetControl_clicked"
QT_MOC_LITERAL(19, 330, 27), // "on_btn_core_setSync_clicked"
QT_MOC_LITERAL(20, 358, 23), // "on_btn_coreRead_clicked"
QT_MOC_LITERAL(21, 382, 21), // "on_btn_mobile_clicked"
QT_MOC_LITERAL(22, 404, 16), // "on_tmr_Broadcast"
QT_MOC_LITERAL(23, 421, 28), // "on_cb_core_mechine_activated"
QT_MOC_LITERAL(24, 450, 10), // "upDateTime"
QT_MOC_LITERAL(25, 461, 14), // "upDateCapacity"
QT_MOC_LITERAL(26, 476, 18), // "upDateFlightStatus"
QT_MOC_LITERAL(27, 495, 19), // "updateControlDevice"
QT_MOC_LITERAL(28, 515, 27), // "on_btn_VRC_resetAll_clicked"
QT_MOC_LITERAL(29, 543, 28), // "on_btn_VRC_resetLeft_clicked"
QT_MOC_LITERAL(30, 572, 29), // "on_btn_VRC_resetRight_clicked"
QT_MOC_LITERAL(31, 602, 29), // "on_btn_virtualRC_send_clicked"
QT_MOC_LITERAL(32, 632, 29), // "on_btn_virtualRC_init_clicked"
QT_MOC_LITERAL(33, 662, 23), // "on_btn_vrc_down_pressed"
QT_MOC_LITERAL(34, 686, 21), // "on_btn_vrc_up_pressed"
QT_MOC_LITERAL(35, 708, 19), // "on_tmr_VRC_autosend"
QT_MOC_LITERAL(36, 728, 20), // "on_btn_vrc_w_pressed"
QT_MOC_LITERAL(37, 749, 20), // "on_btn_vrc_A_pressed"
QT_MOC_LITERAL(38, 770, 20), // "on_btn_vrc_S_pressed"
QT_MOC_LITERAL(39, 791, 20), // "on_btn_vrc_D_pressed"
QT_MOC_LITERAL(40, 812, 22), // "on_btn_vrcRead_clicked"
QT_MOC_LITERAL(41, 835, 23), // "on_btn_vrc_left_pressed"
QT_MOC_LITERAL(42, 859, 24), // "on_btn_vrc_right_pressed"
QT_MOC_LITERAL(43, 884, 25), // "on_btn_cameraRead_clicked"
QT_MOC_LITERAL(44, 910, 23), // "on_btn_camera_0_clicked"
QT_MOC_LITERAL(45, 934, 23), // "on_btn_camera_8_pressed"
QT_MOC_LITERAL(46, 958, 23), // "on_btn_camera_2_pressed"
QT_MOC_LITERAL(47, 982, 23), // "on_btn_camera_7_pressed"
QT_MOC_LITERAL(48, 1006, 23), // "on_btn_camera_9_pressed"
QT_MOC_LITERAL(49, 1030, 23), // "on_btn_camera_4_pressed"
QT_MOC_LITERAL(50, 1054, 23), // "on_btn_camera_6_pressed"
QT_MOC_LITERAL(51, 1078, 22), // "on_tmr_Camera_autosend"
QT_MOC_LITERAL(52, 1101, 25), // "on_cb_camera_send_clicked"
QT_MOC_LITERAL(53, 1127, 7), // "checked"
QT_MOC_LITERAL(54, 1135, 27), // "on_btn_cameraRecord_clicked"
QT_MOC_LITERAL(55, 1163, 26), // "on_btn_cameraShoot_clicked"
QT_MOC_LITERAL(56, 1190, 26), // "on_btn_camera_send_clicked"
QT_MOC_LITERAL(57, 1217, 27), // "on_btr_camera_angle_clicked"
QT_MOC_LITERAL(58, 1245, 27), // "on_btr_camera_speed_clicked"
QT_MOC_LITERAL(59, 1273, 18), // "on_btg_cameraAngle"
QT_MOC_LITERAL(60, 1292, 16), // "QAbstractButton*"
QT_MOC_LITERAL(61, 1309, 6), // "button"
QT_MOC_LITERAL(62, 1316, 16), // "on_btg_cameraYaw"
QT_MOC_LITERAL(63, 1333, 17), // "on_btg_cameraRoll"
QT_MOC_LITERAL(64, 1351, 18), // "on_btg_cameraPitch"
QT_MOC_LITERAL(65, 1370, 16), // "resetCameraAngle"
QT_MOC_LITERAL(66, 1387, 16), // "updateCameraFlag"
QT_MOC_LITERAL(67, 1404, 28), // "on_btn_flight_frount_pressed"
QT_MOC_LITERAL(68, 1433, 26), // "on_btn_flight_back_pressed"
QT_MOC_LITERAL(69, 1460, 29), // "on_btn_flight_runTask_clicked"
QT_MOC_LITERAL(70, 1490, 25), // "on_btn_flight_arm_clicked"
QT_MOC_LITERAL(71, 1516, 24), // "on_btn_flight_up_pressed"
QT_MOC_LITERAL(72, 1541, 26), // "on_btn_flight_left_pressed"
QT_MOC_LITERAL(73, 1568, 27), // "on_btn_flight_right_pressed"
QT_MOC_LITERAL(74, 1596, 26), // "on_btn_flight_down_pressed"
QT_MOC_LITERAL(75, 1623, 29), // "on_btn_flight_leftYaw_pressed"
QT_MOC_LITERAL(76, 1653, 30), // "on_btn_flight_rightYaw_pressed"
QT_MOC_LITERAL(77, 1684, 16), // "on_btg_flight_HL"
QT_MOC_LITERAL(78, 1701, 16), // "on_btg_flight_VL"
QT_MOC_LITERAL(79, 1718, 16), // "on_btg_flight_YL"
QT_MOC_LITERAL(80, 1735, 16), // "on_btg_flight_CL"
QT_MOC_LITERAL(81, 1752, 16), // "on_btg_flight_SM"
QT_MOC_LITERAL(82, 1769, 16), // "updateFlightFlag"
QT_MOC_LITERAL(83, 1786, 13), // "updateFlightX"
QT_MOC_LITERAL(84, 1800, 13), // "updateFlightY"
QT_MOC_LITERAL(85, 1814, 13), // "updateFlightZ"
QT_MOC_LITERAL(86, 1828, 15), // "updateFlightYaw"
QT_MOC_LITERAL(87, 1844, 15), // "resetFlightData"
QT_MOC_LITERAL(88, 1860, 10), // "flightSend"
QT_MOC_LITERAL(89, 1871, 24), // "on_btn_FlightAcc_clicked"
QT_MOC_LITERAL(90, 1896, 24), // "on_btn_FlightPal_clicked"
QT_MOC_LITERAL(91, 1921, 24), // "on_btn_FlightMag_clicked"
QT_MOC_LITERAL(92, 1946, 24), // "on_btn_FlightQua_clicked"
QT_MOC_LITERAL(93, 1971, 24), // "on_btn_FlightVel_clicked"
QT_MOC_LITERAL(94, 1996, 24), // "on_btn_FlightPos_clicked"
QT_MOC_LITERAL(95, 2021, 36), // "on_lineEdit_flight_X_editingF..."
QT_MOC_LITERAL(96, 2058, 36), // "on_lineEdit_flight_Y_editingF..."
QT_MOC_LITERAL(97, 2095, 36), // "on_lineEdit_flight_Z_editingF..."
QT_MOC_LITERAL(98, 2132, 38), // "on_lineEdit_flight_Yaw_editin..."
QT_MOC_LITERAL(99, 2171, 34), // "on_lineEdit_flight_X_returnPr..."
QT_MOC_LITERAL(100, 2206, 34), // "on_lineEdit_flight_Y_returnPr..."
QT_MOC_LITERAL(101, 2241, 34), // "on_lineEdit_flight_Z_returnPr..."
QT_MOC_LITERAL(102, 2276, 36), // "on_lineEdit_flight_Yaw_return..."
QT_MOC_LITERAL(103, 2313, 29), // "on_cb_flight_autoSend_clicked"
QT_MOC_LITERAL(104, 2343, 31), // "on_btn_flight_dataReset_clicked"
QT_MOC_LITERAL(105, 2375, 15), // "filght_autosend"
QT_MOC_LITERAL(106, 2391, 24), // "on_btn_log_clean_clicked"
QT_MOC_LITERAL(107, 2416, 23), // "on_btn_log_save_clicked"
QT_MOC_LITERAL(108, 2440, 24), // "on_cb_mission_hp_clicked"
QT_MOC_LITERAL(109, 2465, 24), // "on_cb_mission_wp_clicked"
QT_MOC_LITERAL(110, 2490, 28), // "on_cb_mission_follow_clicked"
QT_MOC_LITERAL(111, 2519, 29), // "on_btn_hotPoint_start_clicked"
QT_MOC_LITERAL(112, 2549, 28), // "on_btn_hotPoint_stop_clicked"
QT_MOC_LITERAL(113, 2578, 31), // "on_btn_hotPoint_current_clicked"
QT_MOC_LITERAL(114, 2610, 23), // "on_btn_hp_pause_clicked"
QT_MOC_LITERAL(115, 2634, 24), // "on_btn_hp_setPal_clicked"
QT_MOC_LITERAL(116, 2659, 27), // "on_btn_hp_setRadius_clicked"
QT_MOC_LITERAL(117, 2687, 24), // "on_btn_hp_setYaw_clicked"
QT_MOC_LITERAL(118, 2712, 22), // "on_btn_hp_data_clicked"
QT_MOC_LITERAL(119, 2735, 29), // "on_btn_follow_current_clicked"
QT_MOC_LITERAL(120, 2765, 28), // "on_hs_follow_la_valueChanged"
QT_MOC_LITERAL(121, 2794, 5), // "value"
QT_MOC_LITERAL(122, 2800, 28), // "on_hs_follow_lo_valueChanged"
QT_MOC_LITERAL(123, 2829, 28), // "on_hs_follow_al_valueChanged"
QT_MOC_LITERAL(124, 2858, 28), // "on_hs_follow_an_valueChanged"
QT_MOC_LITERAL(125, 2887, 28), // "on_btn_follow_update_clicked"
QT_MOC_LITERAL(126, 2916, 27), // "on_btn_follow_start_clicked"
QT_MOC_LITERAL(127, 2944, 27), // "on_btn_follow_pause_clicked"
QT_MOC_LITERAL(128, 2972, 26), // "on_btn_follow_stop_clicked"
QT_MOC_LITERAL(129, 2999, 18), // "on_tmr_follow_send"
QT_MOC_LITERAL(130, 3018, 40), // "on_cb_waypoint_point_currentI..."
QT_MOC_LITERAL(131, 3059, 37), // "on_le_waypoint_number_editing..."
QT_MOC_LITERAL(132, 3097, 30), // "on_btn_waypoint_action_clicked"
QT_MOC_LITERAL(133, 3128, 29), // "on_btn_waypoint_reset_clicked"
QT_MOC_LITERAL(134, 3158, 36), // "on_btn_waypoint_removeAction_..."
QT_MOC_LITERAL(135, 3195, 33), // "on_btn_waypoint_viewPoint_cli..."
QT_MOC_LITERAL(136, 3229, 23), // "on_btn_wp_ivset_clicked"
QT_MOC_LITERAL(137, 3253, 24), // "on_btn_wp_ivRead_clicked"
QT_MOC_LITERAL(138, 3278, 27), // "on_btn_waypoint_add_clicked"
QT_MOC_LITERAL(139, 3306, 30), // "on_btn_waypoint_remove_clicked"
QT_MOC_LITERAL(140, 3337, 28), // "on_btn_waypoint_init_clicked"
QT_MOC_LITERAL(141, 3366, 20), // "on_btn_wp_pr_clicked"
QT_MOC_LITERAL(142, 3387, 29), // "on_le_wp_exec_editingFinished"
QT_MOC_LITERAL(143, 3417, 25), // "on_btn_wp_loadAll_clicked"
QT_MOC_LITERAL(144, 3443, 28), // "on_btn_wp_start_stop_clicked"
QT_MOC_LITERAL(145, 3472, 25), // "on_btn_wp_loadOne_clicked"
QT_MOC_LITERAL(146, 3498, 24), // "on_waypoint_data_changed"
QT_MOC_LITERAL(147, 3523, 7), // "topLeft"
QT_MOC_LITERAL(148, 3531, 11), // "bottomRight"
QT_MOC_LITERAL(149, 3543, 12), // "QVector<int>"
QT_MOC_LITERAL(150, 3556, 5), // "roles"
QT_MOC_LITERAL(151, 3562, 23), // "on_btn_gps_read_clicked"
QT_MOC_LITERAL(152, 3586, 23), // "on_btn_rtk_read_clicked"
QT_MOC_LITERAL(153, 3610, 20), // "on_tmr_GPRS_autosend"
QT_MOC_LITERAL(154, 3631, 20), // "on_tmr_GPRS_autoread"
QT_MOC_LITERAL(155, 3652, 15), // "plpMissionCheck"
QT_MOC_LITERAL(156, 3668, 22), // "on_btn_webTool_clicked"
QT_MOC_LITERAL(157, 3691, 28), // "on_btn_AbortWaypoint_clicked"
QT_MOC_LITERAL(158, 3720, 23), // "on_btn_plp_init_clicked"
QT_MOC_LITERAL(159, 3744, 26), // "on_btn_plp_loadAll_clicked"
QT_MOC_LITERAL(160, 3771, 23), // "on_btn_Abortplp_clicked"
QT_MOC_LITERAL(161, 3795, 27), // "on_btn_GPRSportOpen_clicked"
QT_MOC_LITERAL(162, 3823, 27), // "on_btn_GPRSportSend_clicked"
QT_MOC_LITERAL(163, 3851, 27), // "on_btn_GPRSportRead_clicked"
QT_MOC_LITERAL(164, 3879, 28), // "on_btn_GPRSportClear_clicked"
QT_MOC_LITERAL(165, 3908, 29), // "on_btn_plp_start_stop_clicked"
QT_MOC_LITERAL(166, 3938, 12), // "guidanceTest"
QT_MOC_LITERAL(167, 3951, 32), // "on_lineEdit_GPRSsend_textChanged"
QT_MOC_LITERAL(168, 3984, 4) // "text"

    },
    "DJIonboardSDK\0GPRSDataSend\0\0s\0abortEmit\0"
    "abortMission\0autoActivateSDK\0"
    "autoActivateGPRS\0on_tmr_autoSendStatus\0"
    "on_btn_portRefresh_clicked\0"
    "on_btn_portOpen_clicked\0"
    "on_comboBox_portName_currentIndexChanged\0"
    "index\0logSignalRecv\0log\0on_btn_coreSet_clicked\0"
    "on_btn_coreActive_clicked\0"
    "on_btn_coreVersion_clicked\0"
    "on_btn_coreSetControl_clicked\0"
    "on_btn_core_setSync_clicked\0"
    "on_btn_coreRead_clicked\0on_btn_mobile_clicked\0"
    "on_tmr_Broadcast\0on_cb_core_mechine_activated\0"
    "upDateTime\0upDateCapacity\0upDateFlightStatus\0"
    "updateControlDevice\0on_btn_VRC_resetAll_clicked\0"
    "on_btn_VRC_resetLeft_clicked\0"
    "on_btn_VRC_resetRight_clicked\0"
    "on_btn_virtualRC_send_clicked\0"
    "on_btn_virtualRC_init_clicked\0"
    "on_btn_vrc_down_pressed\0on_btn_vrc_up_pressed\0"
    "on_tmr_VRC_autosend\0on_btn_vrc_w_pressed\0"
    "on_btn_vrc_A_pressed\0on_btn_vrc_S_pressed\0"
    "on_btn_vrc_D_pressed\0on_btn_vrcRead_clicked\0"
    "on_btn_vrc_left_pressed\0"
    "on_btn_vrc_right_pressed\0"
    "on_btn_cameraRead_clicked\0"
    "on_btn_camera_0_clicked\0on_btn_camera_8_pressed\0"
    "on_btn_camera_2_pressed\0on_btn_camera_7_pressed\0"
    "on_btn_camera_9_pressed\0on_btn_camera_4_pressed\0"
    "on_btn_camera_6_pressed\0on_tmr_Camera_autosend\0"
    "on_cb_camera_send_clicked\0checked\0"
    "on_btn_cameraRecord_clicked\0"
    "on_btn_cameraShoot_clicked\0"
    "on_btn_camera_send_clicked\0"
    "on_btr_camera_angle_clicked\0"
    "on_btr_camera_speed_clicked\0"
    "on_btg_cameraAngle\0QAbstractButton*\0"
    "button\0on_btg_cameraYaw\0on_btg_cameraRoll\0"
    "on_btg_cameraPitch\0resetCameraAngle\0"
    "updateCameraFlag\0on_btn_flight_frount_pressed\0"
    "on_btn_flight_back_pressed\0"
    "on_btn_flight_runTask_clicked\0"
    "on_btn_flight_arm_clicked\0"
    "on_btn_flight_up_pressed\0"
    "on_btn_flight_left_pressed\0"
    "on_btn_flight_right_pressed\0"
    "on_btn_flight_down_pressed\0"
    "on_btn_flight_leftYaw_pressed\0"
    "on_btn_flight_rightYaw_pressed\0"
    "on_btg_flight_HL\0on_btg_flight_VL\0"
    "on_btg_flight_YL\0on_btg_flight_CL\0"
    "on_btg_flight_SM\0updateFlightFlag\0"
    "updateFlightX\0updateFlightY\0updateFlightZ\0"
    "updateFlightYaw\0resetFlightData\0"
    "flightSend\0on_btn_FlightAcc_clicked\0"
    "on_btn_FlightPal_clicked\0"
    "on_btn_FlightMag_clicked\0"
    "on_btn_FlightQua_clicked\0"
    "on_btn_FlightVel_clicked\0"
    "on_btn_FlightPos_clicked\0"
    "on_lineEdit_flight_X_editingFinished\0"
    "on_lineEdit_flight_Y_editingFinished\0"
    "on_lineEdit_flight_Z_editingFinished\0"
    "on_lineEdit_flight_Yaw_editingFinished\0"
    "on_lineEdit_flight_X_returnPressed\0"
    "on_lineEdit_flight_Y_returnPressed\0"
    "on_lineEdit_flight_Z_returnPressed\0"
    "on_lineEdit_flight_Yaw_returnPressed\0"
    "on_cb_flight_autoSend_clicked\0"
    "on_btn_flight_dataReset_clicked\0"
    "filght_autosend\0on_btn_log_clean_clicked\0"
    "on_btn_log_save_clicked\0"
    "on_cb_mission_hp_clicked\0"
    "on_cb_mission_wp_clicked\0"
    "on_cb_mission_follow_clicked\0"
    "on_btn_hotPoint_start_clicked\0"
    "on_btn_hotPoint_stop_clicked\0"
    "on_btn_hotPoint_current_clicked\0"
    "on_btn_hp_pause_clicked\0"
    "on_btn_hp_setPal_clicked\0"
    "on_btn_hp_setRadius_clicked\0"
    "on_btn_hp_setYaw_clicked\0"
    "on_btn_hp_data_clicked\0"
    "on_btn_follow_current_clicked\0"
    "on_hs_follow_la_valueChanged\0value\0"
    "on_hs_follow_lo_valueChanged\0"
    "on_hs_follow_al_valueChanged\0"
    "on_hs_follow_an_valueChanged\0"
    "on_btn_follow_update_clicked\0"
    "on_btn_follow_start_clicked\0"
    "on_btn_follow_pause_clicked\0"
    "on_btn_follow_stop_clicked\0"
    "on_tmr_follow_send\0"
    "on_cb_waypoint_point_currentIndexChanged\0"
    "on_le_waypoint_number_editingFinished\0"
    "on_btn_waypoint_action_clicked\0"
    "on_btn_waypoint_reset_clicked\0"
    "on_btn_waypoint_removeAction_clicked\0"
    "on_btn_waypoint_viewPoint_clicked\0"
    "on_btn_wp_ivset_clicked\0"
    "on_btn_wp_ivRead_clicked\0"
    "on_btn_waypoint_add_clicked\0"
    "on_btn_waypoint_remove_clicked\0"
    "on_btn_waypoint_init_clicked\0"
    "on_btn_wp_pr_clicked\0on_le_wp_exec_editingFinished\0"
    "on_btn_wp_loadAll_clicked\0"
    "on_btn_wp_start_stop_clicked\0"
    "on_btn_wp_loadOne_clicked\0"
    "on_waypoint_data_changed\0topLeft\0"
    "bottomRight\0QVector<int>\0roles\0"
    "on_btn_gps_read_clicked\0on_btn_rtk_read_clicked\0"
    "on_tmr_GPRS_autosend\0on_tmr_GPRS_autoread\0"
    "plpMissionCheck\0on_btn_webTool_clicked\0"
    "on_btn_AbortWaypoint_clicked\0"
    "on_btn_plp_init_clicked\0"
    "on_btn_plp_loadAll_clicked\0"
    "on_btn_Abortplp_clicked\0"
    "on_btn_GPRSportOpen_clicked\0"
    "on_btn_GPRSportSend_clicked\0"
    "on_btn_GPRSportRead_clicked\0"
    "on_btn_GPRSportClear_clicked\0"
    "on_btn_plp_start_stop_clicked\0"
    "guidanceTest\0on_lineEdit_GPRSsend_textChanged\0"
    "text"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DJIonboardSDK[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
     154,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  784,    2, 0x06 /* Public */,
       4,    1,  787,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,  790,    2, 0x08 /* Private */,
       7,    0,  791,    2, 0x08 /* Private */,
       8,    0,  792,    2, 0x08 /* Private */,
       9,    0,  793,    2, 0x08 /* Private */,
      10,    0,  794,    2, 0x08 /* Private */,
      11,    1,  795,    2, 0x08 /* Private */,
      13,    1,  798,    2, 0x08 /* Private */,
      15,    0,  801,    2, 0x08 /* Private */,
      16,    0,  802,    2, 0x08 /* Private */,
      17,    0,  803,    2, 0x08 /* Private */,
      18,    0,  804,    2, 0x08 /* Private */,
      19,    0,  805,    2, 0x08 /* Private */,
      20,    0,  806,    2, 0x08 /* Private */,
      21,    0,  807,    2, 0x08 /* Private */,
      22,    0,  808,    2, 0x08 /* Private */,
      23,    1,  809,    2, 0x08 /* Private */,
      24,    0,  812,    2, 0x08 /* Private */,
      25,    0,  813,    2, 0x08 /* Private */,
      26,    0,  814,    2, 0x08 /* Private */,
      27,    0,  815,    2, 0x08 /* Private */,
      28,    0,  816,    2, 0x08 /* Private */,
      29,    0,  817,    2, 0x08 /* Private */,
      30,    0,  818,    2, 0x08 /* Private */,
      31,    0,  819,    2, 0x08 /* Private */,
      32,    0,  820,    2, 0x08 /* Private */,
      33,    0,  821,    2, 0x08 /* Private */,
      34,    0,  822,    2, 0x08 /* Private */,
      35,    0,  823,    2, 0x08 /* Private */,
      36,    0,  824,    2, 0x08 /* Private */,
      37,    0,  825,    2, 0x08 /* Private */,
      38,    0,  826,    2, 0x08 /* Private */,
      39,    0,  827,    2, 0x08 /* Private */,
      40,    0,  828,    2, 0x08 /* Private */,
      41,    0,  829,    2, 0x08 /* Private */,
      42,    0,  830,    2, 0x08 /* Private */,
      43,    0,  831,    2, 0x08 /* Private */,
      44,    0,  832,    2, 0x08 /* Private */,
      45,    0,  833,    2, 0x08 /* Private */,
      46,    0,  834,    2, 0x08 /* Private */,
      47,    0,  835,    2, 0x08 /* Private */,
      48,    0,  836,    2, 0x08 /* Private */,
      49,    0,  837,    2, 0x08 /* Private */,
      50,    0,  838,    2, 0x08 /* Private */,
      51,    0,  839,    2, 0x08 /* Private */,
      52,    1,  840,    2, 0x08 /* Private */,
      54,    1,  843,    2, 0x08 /* Private */,
      55,    0,  846,    2, 0x08 /* Private */,
      56,    0,  847,    2, 0x08 /* Private */,
      57,    0,  848,    2, 0x08 /* Private */,
      58,    0,  849,    2, 0x08 /* Private */,
      59,    1,  850,    2, 0x08 /* Private */,
      62,    1,  853,    2, 0x08 /* Private */,
      63,    1,  856,    2, 0x08 /* Private */,
      64,    1,  859,    2, 0x08 /* Private */,
      65,    0,  862,    2, 0x08 /* Private */,
      66,    0,  863,    2, 0x08 /* Private */,
      67,    0,  864,    2, 0x08 /* Private */,
      68,    0,  865,    2, 0x08 /* Private */,
      69,    0,  866,    2, 0x08 /* Private */,
      70,    1,  867,    2, 0x08 /* Private */,
      71,    0,  870,    2, 0x08 /* Private */,
      72,    0,  871,    2, 0x08 /* Private */,
      73,    0,  872,    2, 0x08 /* Private */,
      74,    0,  873,    2, 0x08 /* Private */,
      75,    0,  874,    2, 0x08 /* Private */,
      76,    0,  875,    2, 0x08 /* Private */,
      77,    1,  876,    2, 0x08 /* Private */,
      78,    1,  879,    2, 0x08 /* Private */,
      79,    1,  882,    2, 0x08 /* Private */,
      80,    1,  885,    2, 0x08 /* Private */,
      81,    1,  888,    2, 0x08 /* Private */,
      82,    0,  891,    2, 0x08 /* Private */,
      83,    0,  892,    2, 0x08 /* Private */,
      84,    0,  893,    2, 0x08 /* Private */,
      85,    0,  894,    2, 0x08 /* Private */,
      86,    0,  895,    2, 0x08 /* Private */,
      87,    0,  896,    2, 0x08 /* Private */,
      88,    0,  897,    2, 0x08 /* Private */,
      89,    0,  898,    2, 0x08 /* Private */,
      90,    0,  899,    2, 0x08 /* Private */,
      91,    0,  900,    2, 0x08 /* Private */,
      92,    0,  901,    2, 0x08 /* Private */,
      93,    0,  902,    2, 0x08 /* Private */,
      94,    0,  903,    2, 0x08 /* Private */,
      95,    0,  904,    2, 0x08 /* Private */,
      96,    0,  905,    2, 0x08 /* Private */,
      97,    0,  906,    2, 0x08 /* Private */,
      98,    0,  907,    2, 0x08 /* Private */,
      99,    0,  908,    2, 0x08 /* Private */,
     100,    0,  909,    2, 0x08 /* Private */,
     101,    0,  910,    2, 0x08 /* Private */,
     102,    0,  911,    2, 0x08 /* Private */,
     103,    1,  912,    2, 0x08 /* Private */,
     104,    0,  915,    2, 0x08 /* Private */,
     105,    0,  916,    2, 0x08 /* Private */,
     106,    0,  917,    2, 0x08 /* Private */,
     107,    0,  918,    2, 0x08 /* Private */,
     108,    1,  919,    2, 0x08 /* Private */,
     109,    1,  922,    2, 0x08 /* Private */,
     110,    1,  925,    2, 0x08 /* Private */,
     111,    0,  928,    2, 0x08 /* Private */,
     112,    0,  929,    2, 0x08 /* Private */,
     113,    0,  930,    2, 0x08 /* Private */,
     114,    1,  931,    2, 0x08 /* Private */,
     115,    0,  934,    2, 0x08 /* Private */,
     116,    0,  935,    2, 0x08 /* Private */,
     117,    0,  936,    2, 0x08 /* Private */,
     118,    0,  937,    2, 0x08 /* Private */,
     119,    0,  938,    2, 0x08 /* Private */,
     120,    1,  939,    2, 0x08 /* Private */,
     122,    1,  942,    2, 0x08 /* Private */,
     123,    1,  945,    2, 0x08 /* Private */,
     124,    1,  948,    2, 0x08 /* Private */,
     125,    0,  951,    2, 0x08 /* Private */,
     126,    0,  952,    2, 0x08 /* Private */,
     127,    1,  953,    2, 0x08 /* Private */,
     128,    0,  956,    2, 0x08 /* Private */,
     129,    0,  957,    2, 0x08 /* Private */,
     130,    1,  958,    2, 0x08 /* Private */,
     131,    0,  961,    2, 0x08 /* Private */,
     132,    0,  962,    2, 0x08 /* Private */,
     133,    0,  963,    2, 0x08 /* Private */,
     134,    0,  964,    2, 0x08 /* Private */,
     135,    0,  965,    2, 0x08 /* Private */,
     136,    0,  966,    2, 0x08 /* Private */,
     137,    0,  967,    2, 0x08 /* Private */,
     138,    0,  968,    2, 0x08 /* Private */,
     139,    0,  969,    2, 0x08 /* Private */,
     140,    0,  970,    2, 0x08 /* Private */,
     141,    1,  971,    2, 0x08 /* Private */,
     142,    0,  974,    2, 0x08 /* Private */,
     143,    0,  975,    2, 0x08 /* Private */,
     144,    1,  976,    2, 0x08 /* Private */,
     145,    0,  979,    2, 0x08 /* Private */,
     146,    3,  980,    2, 0x08 /* Private */,
     151,    0,  987,    2, 0x08 /* Private */,
     152,    0,  988,    2, 0x08 /* Private */,
     153,    0,  989,    2, 0x08 /* Private */,
     154,    0,  990,    2, 0x08 /* Private */,
     155,    0,  991,    2, 0x08 /* Private */,
     156,    1,  992,    2, 0x08 /* Private */,
     157,    0,  995,    2, 0x08 /* Private */,
     158,    0,  996,    2, 0x08 /* Private */,
     159,    0,  997,    2, 0x08 /* Private */,
     160,    0,  998,    2, 0x08 /* Private */,
     161,    0,  999,    2, 0x08 /* Private */,
     162,    0, 1000,    2, 0x08 /* Private */,
     163,    0, 1001,    2, 0x08 /* Private */,
     164,    0, 1002,    2, 0x08 /* Private */,
     165,    0, 1003,    2, 0x08 /* Private */,
     166,    0, 1004,    2, 0x08 /* Private */,
     167,    1, 1005,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    5,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   53,
    QMetaType::Void, QMetaType::Bool,   53,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 60,   61,
    QMetaType::Void, 0x80000000 | 60,   61,
    QMetaType::Void, 0x80000000 | 60,   61,
    QMetaType::Void, 0x80000000 | 60,   61,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   53,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 60,   61,
    QMetaType::Void, 0x80000000 | 60,   61,
    QMetaType::Void, 0x80000000 | 60,   61,
    QMetaType::Void, 0x80000000 | 60,   61,
    QMetaType::Void, 0x80000000 | 60,   61,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   53,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   53,
    QMetaType::Void, QMetaType::Bool,   53,
    QMetaType::Void, QMetaType::Bool,   53,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   53,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,  121,
    QMetaType::Void, QMetaType::Int,  121,
    QMetaType::Void, QMetaType::Int,  121,
    QMetaType::Void, QMetaType::Int,  121,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   53,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   53,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   53,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::QModelIndex, 0x80000000 | 149,  147,  148,  150,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   53,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,  168,

       0        // eod
};

void DJIonboardSDK::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DJIonboardSDK *_t = static_cast<DJIonboardSDK *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->GPRSDataSend((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->abortEmit((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->autoActivateSDK(); break;
        case 3: _t->autoActivateGPRS(); break;
        case 4: _t->on_tmr_autoSendStatus(); break;
        case 5: _t->on_btn_portRefresh_clicked(); break;
        case 6: _t->on_btn_portOpen_clicked(); break;
        case 7: _t->on_comboBox_portName_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->logSignalRecv((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->on_btn_coreSet_clicked(); break;
        case 10: _t->on_btn_coreActive_clicked(); break;
        case 11: _t->on_btn_coreVersion_clicked(); break;
        case 12: _t->on_btn_coreSetControl_clicked(); break;
        case 13: _t->on_btn_core_setSync_clicked(); break;
        case 14: _t->on_btn_coreRead_clicked(); break;
        case 15: _t->on_btn_mobile_clicked(); break;
        case 16: _t->on_tmr_Broadcast(); break;
        case 17: _t->on_cb_core_mechine_activated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->upDateTime(); break;
        case 19: _t->upDateCapacity(); break;
        case 20: _t->upDateFlightStatus(); break;
        case 21: _t->updateControlDevice(); break;
        case 22: _t->on_btn_VRC_resetAll_clicked(); break;
        case 23: _t->on_btn_VRC_resetLeft_clicked(); break;
        case 24: _t->on_btn_VRC_resetRight_clicked(); break;
        case 25: _t->on_btn_virtualRC_send_clicked(); break;
        case 26: _t->on_btn_virtualRC_init_clicked(); break;
        case 27: _t->on_btn_vrc_down_pressed(); break;
        case 28: _t->on_btn_vrc_up_pressed(); break;
        case 29: _t->on_tmr_VRC_autosend(); break;
        case 30: _t->on_btn_vrc_w_pressed(); break;
        case 31: _t->on_btn_vrc_A_pressed(); break;
        case 32: _t->on_btn_vrc_S_pressed(); break;
        case 33: _t->on_btn_vrc_D_pressed(); break;
        case 34: _t->on_btn_vrcRead_clicked(); break;
        case 35: _t->on_btn_vrc_left_pressed(); break;
        case 36: _t->on_btn_vrc_right_pressed(); break;
        case 37: _t->on_btn_cameraRead_clicked(); break;
        case 38: _t->on_btn_camera_0_clicked(); break;
        case 39: _t->on_btn_camera_8_pressed(); break;
        case 40: _t->on_btn_camera_2_pressed(); break;
        case 41: _t->on_btn_camera_7_pressed(); break;
        case 42: _t->on_btn_camera_9_pressed(); break;
        case 43: _t->on_btn_camera_4_pressed(); break;
        case 44: _t->on_btn_camera_6_pressed(); break;
        case 45: _t->on_tmr_Camera_autosend(); break;
        case 46: _t->on_cb_camera_send_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 47: _t->on_btn_cameraRecord_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 48: _t->on_btn_cameraShoot_clicked(); break;
        case 49: _t->on_btn_camera_send_clicked(); break;
        case 50: _t->on_btr_camera_angle_clicked(); break;
        case 51: _t->on_btr_camera_speed_clicked(); break;
        case 52: _t->on_btg_cameraAngle((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 53: _t->on_btg_cameraYaw((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 54: _t->on_btg_cameraRoll((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 55: _t->on_btg_cameraPitch((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 56: _t->resetCameraAngle(); break;
        case 57: _t->updateCameraFlag(); break;
        case 58: _t->on_btn_flight_frount_pressed(); break;
        case 59: _t->on_btn_flight_back_pressed(); break;
        case 60: _t->on_btn_flight_runTask_clicked(); break;
        case 61: _t->on_btn_flight_arm_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 62: _t->on_btn_flight_up_pressed(); break;
        case 63: _t->on_btn_flight_left_pressed(); break;
        case 64: _t->on_btn_flight_right_pressed(); break;
        case 65: _t->on_btn_flight_down_pressed(); break;
        case 66: _t->on_btn_flight_leftYaw_pressed(); break;
        case 67: _t->on_btn_flight_rightYaw_pressed(); break;
        case 68: _t->on_btg_flight_HL((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 69: _t->on_btg_flight_VL((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 70: _t->on_btg_flight_YL((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 71: _t->on_btg_flight_CL((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 72: _t->on_btg_flight_SM((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 73: _t->updateFlightFlag(); break;
        case 74: _t->updateFlightX(); break;
        case 75: _t->updateFlightY(); break;
        case 76: _t->updateFlightZ(); break;
        case 77: _t->updateFlightYaw(); break;
        case 78: _t->resetFlightData(); break;
        case 79: _t->flightSend(); break;
        case 80: _t->on_btn_FlightAcc_clicked(); break;
        case 81: _t->on_btn_FlightPal_clicked(); break;
        case 82: _t->on_btn_FlightMag_clicked(); break;
        case 83: _t->on_btn_FlightQua_clicked(); break;
        case 84: _t->on_btn_FlightVel_clicked(); break;
        case 85: _t->on_btn_FlightPos_clicked(); break;
        case 86: _t->on_lineEdit_flight_X_editingFinished(); break;
        case 87: _t->on_lineEdit_flight_Y_editingFinished(); break;
        case 88: _t->on_lineEdit_flight_Z_editingFinished(); break;
        case 89: _t->on_lineEdit_flight_Yaw_editingFinished(); break;
        case 90: _t->on_lineEdit_flight_X_returnPressed(); break;
        case 91: _t->on_lineEdit_flight_Y_returnPressed(); break;
        case 92: _t->on_lineEdit_flight_Z_returnPressed(); break;
        case 93: _t->on_lineEdit_flight_Yaw_returnPressed(); break;
        case 94: _t->on_cb_flight_autoSend_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 95: _t->on_btn_flight_dataReset_clicked(); break;
        case 96: _t->filght_autosend(); break;
        case 97: _t->on_btn_log_clean_clicked(); break;
        case 98: _t->on_btn_log_save_clicked(); break;
        case 99: _t->on_cb_mission_hp_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 100: _t->on_cb_mission_wp_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 101: _t->on_cb_mission_follow_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 102: _t->on_btn_hotPoint_start_clicked(); break;
        case 103: _t->on_btn_hotPoint_stop_clicked(); break;
        case 104: _t->on_btn_hotPoint_current_clicked(); break;
        case 105: _t->on_btn_hp_pause_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 106: _t->on_btn_hp_setPal_clicked(); break;
        case 107: _t->on_btn_hp_setRadius_clicked(); break;
        case 108: _t->on_btn_hp_setYaw_clicked(); break;
        case 109: _t->on_btn_hp_data_clicked(); break;
        case 110: _t->on_btn_follow_current_clicked(); break;
        case 111: _t->on_hs_follow_la_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 112: _t->on_hs_follow_lo_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 113: _t->on_hs_follow_al_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 114: _t->on_hs_follow_an_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 115: _t->on_btn_follow_update_clicked(); break;
        case 116: _t->on_btn_follow_start_clicked(); break;
        case 117: _t->on_btn_follow_pause_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 118: _t->on_btn_follow_stop_clicked(); break;
        case 119: _t->on_tmr_follow_send(); break;
        case 120: _t->on_cb_waypoint_point_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 121: _t->on_le_waypoint_number_editingFinished(); break;
        case 122: _t->on_btn_waypoint_action_clicked(); break;
        case 123: _t->on_btn_waypoint_reset_clicked(); break;
        case 124: _t->on_btn_waypoint_removeAction_clicked(); break;
        case 125: _t->on_btn_waypoint_viewPoint_clicked(); break;
        case 126: _t->on_btn_wp_ivset_clicked(); break;
        case 127: _t->on_btn_wp_ivRead_clicked(); break;
        case 128: _t->on_btn_waypoint_add_clicked(); break;
        case 129: _t->on_btn_waypoint_remove_clicked(); break;
        case 130: _t->on_btn_waypoint_init_clicked(); break;
        case 131: _t->on_btn_wp_pr_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 132: _t->on_le_wp_exec_editingFinished(); break;
        case 133: _t->on_btn_wp_loadAll_clicked(); break;
        case 134: _t->on_btn_wp_start_stop_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 135: _t->on_btn_wp_loadOne_clicked(); break;
        case 136: _t->on_waypoint_data_changed((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2])),(*reinterpret_cast< const QVector<int>(*)>(_a[3]))); break;
        case 137: _t->on_btn_gps_read_clicked(); break;
        case 138: _t->on_btn_rtk_read_clicked(); break;
        case 139: _t->on_tmr_GPRS_autosend(); break;
        case 140: _t->on_tmr_GPRS_autoread(); break;
        case 141: _t->plpMissionCheck(); break;
        case 142: _t->on_btn_webTool_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 143: _t->on_btn_AbortWaypoint_clicked(); break;
        case 144: _t->on_btn_plp_init_clicked(); break;
        case 145: _t->on_btn_plp_loadAll_clicked(); break;
        case 146: _t->on_btn_Abortplp_clicked(); break;
        case 147: _t->on_btn_GPRSportOpen_clicked(); break;
        case 148: _t->on_btn_GPRSportSend_clicked(); break;
        case 149: _t->on_btn_GPRSportRead_clicked(); break;
        case 150: _t->on_btn_GPRSportClear_clicked(); break;
        case 151: _t->on_btn_plp_start_stop_clicked(); break;
        case 152: _t->guidanceTest(); break;
        case 153: _t->on_lineEdit_GPRSsend_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 52:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractButton* >(); break;
            }
            break;
        case 53:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractButton* >(); break;
            }
            break;
        case 54:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractButton* >(); break;
            }
            break;
        case 55:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractButton* >(); break;
            }
            break;
        case 68:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractButton* >(); break;
            }
            break;
        case 69:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractButton* >(); break;
            }
            break;
        case 70:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractButton* >(); break;
            }
            break;
        case 71:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractButton* >(); break;
            }
            break;
        case 72:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractButton* >(); break;
            }
            break;
        case 136:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<int> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (DJIonboardSDK::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DJIonboardSDK::GPRSDataSend)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (DJIonboardSDK::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DJIonboardSDK::abortEmit)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject DJIonboardSDK::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_DJIonboardSDK.data,
      qt_meta_data_DJIonboardSDK,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *DJIonboardSDK::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DJIonboardSDK::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_DJIonboardSDK.stringdata0))
        return static_cast<void*>(const_cast< DJIonboardSDK*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int DJIonboardSDK::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 154)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 154;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 154)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 154;
    }
    return _id;
}

// SIGNAL 0
void DJIonboardSDK::GPRSDataSend(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DJIonboardSDK::abortEmit(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
