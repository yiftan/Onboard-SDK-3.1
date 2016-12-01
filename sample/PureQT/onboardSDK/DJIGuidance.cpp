

#include <QDebug>
#include "DJI_guidance.h"
#include "DJIGuidance.h"
//#pragma comment(lib,"../onboardSDK/DJI_guidance.lib")
DJIGuidance g;
int my_callback(int data_type, int data_len, char *content)
{
	g.enter();
	if (e_obstacle_distance == data_type && NULL != content)
	{
		obstacle_distance *oa = (obstacle_distance*)content;
		distance_down = oa->distance[0] * 0.01;
		//qDebug() << distance_down;
		distance_front = oa->distance[1] * 0.01;
		//qDebug() << distance_front;
	}
	g.set_event();
	g.leave();
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


void DJIGuidance::guidanceTest(){
	int err_code;
	if (!initGuiFlag){
		reset_config();  // clear all data subscription
		err_code = init_transfer(); //wait for board ready and init transfer thread
		qDebug() << err_code;
		if (err_code == 0){
			initGuiFlag = true;
		}

	}
	else{
		//  RETURN_IF_ERR( err_code );
		select_obstacle_distance();
		user_call_back ucb = my_callback;
		err_code = set_sdk_event_handler(ucb);
		if (err_code != 0){
			initGuiFlag = false;
		}
		//qDebug() << err_code;
		err_code = start_transfer();
		g.wait_event();
		err_code = stop_transfer();
		//	qDebug() << err_code;

		//     err_code = release_transfer();
		//	qDebug() << err_code;
	}
}

void DJIGuidance::run(){
	while (!stopped){
		guidanceTest();
		msleep(200);
	}
	stopped = false;

}
void DJIGuidance::stop(){
	stopped = true;
}
DJIGuidance::DJIGuidance(){
	stopped = false;
	initGuiFlag = false;
	InitializeCriticalSection(&m_critical_section);
	SECURITY_ATTRIBUTES   sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	CreatePipe(&m_pipe_read, &m_pipe_write, &sa, 0);
}

DJIGuidance::~DJIGuidance()
{
	CloseHandle(m_pipe_read);
	CloseHandle(m_pipe_write);
}
void  DJIGuidance::enter()
{
	EnterCriticalSection(&m_critical_section);
}

void  DJIGuidance::leave()
{
	LeaveCriticalSection(&m_critical_section);
}

int  DJIGuidance::set_event()
{
	char buffer[1] = { 0 };
	int count = 0;
	int ret = WriteFile(m_pipe_write, (LPWORD)buffer, 1, (LPDWORD)&count, NULL);
	return ret;
}

int  DJIGuidance::wait_event()
{
	char buffer[1] = { 0 };
	int count = 0;
	int ret = ReadFile(m_pipe_read, (LPWORD)buffer, 1, (LPDWORD)&count, NULL);
	return ret;
}