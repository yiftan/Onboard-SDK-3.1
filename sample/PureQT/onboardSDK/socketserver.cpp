#include "socketserver.h"
#pragma comment(lib,"ws2_32.lib")
socketServer::socketServer()
{
    stopped=false;
}

void socketServer::stop()
{
    stopped=true;
}

void socketServer::run()
{
    QProcess qp;
    qp.startDetached("F:/GIT_project/client/Debug/client.exe");
    WORD wVersionRequested;
    WSADATA wsaData;
    int ret, length;
    SOCKET sListen, sServer;
    struct sockaddr_in saServer, saClient;
    wVersionRequested = MAKEWORD(2, 2);
    ret = WSAStartup(wVersionRequested, &wsaData);
    sListen = socket(AF_INET, SOCK_STREAM, 0);
    if (sListen == INVALID_SOCKET)
    {
        WSACleanup();
    }
    saServer.sin_family = AF_INET;
    saServer.sin_port = htons(22222);
    saServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    ret = bind(sListen, (struct sockaddr *)&saServer, sizeof(saServer));
    if (ret == SOCKET_ERROR)
    {
        closesocket(sListen);
        WSACleanup();
    }
    ret = listen(sListen, 5);
    if (ret == SOCKET_ERROR)
    {
        closesocket(sListen);
    }
    length = sizeof(saClient);
    sServer = accept(sListen, (struct sockaddr *)&saClient, &length);
    if (sServer == INVALID_SOCKET)
    {
        closesocket(sListen);
        WSACleanup();
        getchar();
    }
    /*init*/

    char buf[50];
    QString mal;
    while (!stopped)
    {
        memset(buf, '0', sizeof(buf));
        ret = recv(sServer, buf, 50, 0);
        if (ret == SOCKET_ERROR)
        {
            break;
        }
        //cout << buf << endl;
        if(ret>0)
        {
            mal=QString(buf);
            emit emitMalfunction(mal);
            qDebug()<<buf;
        }
    }
    stopped=false;
}
