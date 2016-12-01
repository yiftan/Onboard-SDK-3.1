#ifndef DJIGUIDANCE_H
#define DJIGUIDANCE_H
//#include "powerlinepatrol.h"
#include "QonboardSDK.h"
#include <Windows.h>
#include <WinBase.h>
#include "DJI_guidance.h"
class DJIGuidance : public QThread
{
	Q_OBJECT

public:
	DJIGuidance();
	~DJIGuidance();
	void  enter();
	void  leave();
	int   set_event();
	int   wait_event();
	void   stop();

protected:
	void run();
private:
	volatile bool stopped;
	bool initGuiFlag;
	CRITICAL_SECTION  m_critical_section;
	HANDLE      m_pipe_read;
	HANDLE      m_pipe_write;
void guidanceTest();
};
#endif 
