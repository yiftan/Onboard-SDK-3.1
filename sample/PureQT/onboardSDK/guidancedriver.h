#ifndef GUIDANCEDRIVER_H
#define GUIDANCEDRIVER_H

#include <Windows.h>
#include <WinBase.h>
#include <QDebug>
#include <QThread>
#include <QonboardSDK.h>
#include <DJI_guidance.h>


class GuidanceDriver;
extern GuidanceDriver guid;

class GuidanceDriver : public QThread
{
    Q_OBJECT
public:
    GuidanceDriver();
    ~GuidanceDriver();

    volatile bool stopped;
    void stop();
    void enter();
    void leave();
    int set_event();
    int wait_event();
private:
    bool initGuiFlag;
    CRITICAL_SECTION  m_critical_section;
    HANDLE      m_pipe_read;
    HANDLE      m_pipe_write;
protected:
    void run();
};

#endif // DJIGUIDANCE_H
