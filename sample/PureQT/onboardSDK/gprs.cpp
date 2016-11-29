#include "gprs.h"
using namespace DJI::onboardSDK;

GPRSSendMessage::GPRSSendMessage(QtOnboardsdkPortDriver *GPRSdriver, QSerialPort *GPRSport, QTextBrowser *tb_GPRSDisplay)
{
    this->GPRSdriver=GPRSdriver;
    this->GPRSport=GPRSport;
    this->text=tb_GPRSDisplay;
    stopped=false;
}

void GPRSSendMessage::stop()
{
    stopped=true;
}

void GPRSSendMessage::run()
{
    while(!stopped)
    {
        GPRSDataSend();
        msleep(900);
    }
    stopped=false;
}

void GPRSSendMessage::GPRSDataSend()
{
    size_t GPRSDATA_len=0;
    int flag=0;
    QByteArray ba;
    QStringList GPRSDATA;
    QString head="Send: ";

    char* tmp;

    if((!GPRSSendBufData.isEmpty()))
    {
        if(GPRSSendBufData[0].contains("AT+CIPSEND",Qt::CaseSensitive))
        {
            GPRSDATA=GPRSSendBufData[0].split("|");
            if(GPRSDATA.length()>1)
            {
                ba=GPRSDATA[0].toLatin1();
                tmp=ba.data();
                GPRSDATA_len = GPRSdriver->charsend(tmp, GPRSDATA[0].length());
                msleep(1000);
                ba=GPRSDATA[1].toLatin1();
                tmp=ba.data();
                GPRSDATA_len = GPRSdriver->charsend(tmp, GPRSDATA[1].length());
                GPRSDATA.clear();
                flag=1;
            }
        }
        else if(GPRSSendBufData[0].contains("ATE0",Qt::CaseSensitive))
        {
            ba = GPRSSendBufData[0].toLatin1();
            tmp=ba.data();

            GPRSDATA_len = GPRSdriver->charsend(tmp, GPRSSendBufData[0].length());
            msleep(1000);
            flag=1;
        }
        else
        {
            ba = GPRSSendBufData[0].toLatin1();
            tmp=ba.data();

            GPRSDATA_len = GPRSdriver->charsend(tmp, GPRSSendBufData[0].length());
            flag=1;
        }
        //GPRSSendflag=1;
        GPRSSendBufData.removeFirst();
    }
    else if(!GPRSSendBufHD.isEmpty())
    {
        if(GPRSSendBufHD[0].contains("AT+CIPSEND",Qt::CaseSensitive))
        {
            GPRSDATA=GPRSSendBufHD[0].split("|");
            if(GPRSDATA.length()>1)
            {
                ba=GPRSDATA[0].toLatin1();
                tmp=ba.data();
                GPRSDATA_len = GPRSdriver->charsend(tmp, GPRSDATA[0].length());
                msleep(500);
                ba=GPRSDATA[1].toLatin1();
                tmp=ba.data();
                GPRSDATA_len = GPRSdriver->charsend(tmp, GPRSDATA[1].length());
                GPRSDATA.clear();
                flag=1;
            }
        }
        GPRSSendBufHD.removeFirst();
        //GPRSSendflag=0;
    }
    if(GPRSDATA_len==0&&flag==1)
    {
        text->append(head.append("error"));
    }
    else if(flag==1)
    {
        text->append(head.append(tmp));
    }
    flag=0;
}

void GPRSSendMessage::GPRSSignalRev(const QString &s)
{
    Message=s;
    if(!Message.isEmpty())
        Message+="\r";
    if(Message.contains("H=U=L=",Qt::CaseSensitive))
    {
        GPRSSendBufHD.append(Message);
        Message.clear();
    }
    else if(!Message.isEmpty())
    {
        GPRSSendBufData.append(Message);
        Message.clear();
    }
}

