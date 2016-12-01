#include "guidancedriver.h"

/*  enum error_code:
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
*/
#pragma comment(lib,"../onboardSDK/DJI_guidance.lib")
GuidanceDriver guid;
double distance_front;
double distance_down;

int my_callback(int data_type, int data_len, char *content)
{
    guid.enter();
    if (e_obstacle_distance == data_type && NULL != content)
    {
        obstacle_distance *oa = (obstacle_distance*)content;
        distance_down = oa->distance[0] * 0.01;
        distance_front = oa->distance[1] * 0.01;
        //qDebug() << distance_front;
    }
    guid.set_event();
    guid.leave();
    return 0;
}

GuidanceDriver::GuidanceDriver()
{
    stopped=false;
    initGuiFlag=false;
    err_code=0;
    InitializeCriticalSection( &m_critical_section );
    SECURITY_ATTRIBUTES   sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    CreatePipe( &m_pipe_read, &m_pipe_write, &sa, 0 );
}

GuidanceDriver::~GuidanceDriver()
{
    CloseHandle( m_pipe_read );
    CloseHandle( m_pipe_write );
}

void GuidanceDriver::stop()
{
    stopped=true;
}

void GuidanceDriver::enter()
{
    EnterCriticalSection( &m_critical_section );
}

void GuidanceDriver::leave()
{
    LeaveCriticalSection( &m_critical_section );
}

int GuidanceDriver::set_event()
{
    char buffer[1] = {0};
    int count = 0;
    int ret = WriteFile( m_pipe_write, (LPWORD)buffer, 1, (LPDWORD)&count, NULL );
    return ret;
}

int GuidanceDriver::wait_event()
{
    char buffer[1] = {0};
    int count = 0;
    int ret = ReadFile( m_pipe_read, (LPWORD)buffer, 1, (LPDWORD)&count, NULL );
    return ret;
}

/*
emitErrorCode:
1: start successfully;
2: stopped;
3: error;
*/
void GuidanceDriver::run()
{
    stopped=false;
    while(!stopped)
    {
        if(!initGuiFlag)
        {
            err_code=reset_config();  // clear all data subscription
            if(err_code)
                break;
            err_code = init_transfer(); //wait for board ready and init transfer thread
            if(err_code)
                break;
            select_obstacle_distance();
            if(err_code)
                break;
            err_code = set_sdk_event_handler(my_callback);
            if(err_code)
                break;
            err_code = start_transfer();
            if(err_code)
                break;
            initGuiFlag=true;
            emit emitErrorCode(QString("1"));
        }
        else
        {
            guid.wait_event();
        }
        msleep(200);
    }
    if(!err_code)
    {
        err_code=stop_transfer();
    }
    if(!err_code)
        err_code=release_transfer();
    stopped=false;
    initGuiFlag=false;
    if(err_code)
    {
        emit emitErrorCode(QString("3"));
    }
    else
    {
        emit emitErrorCode(QString("2"));
    }
}
