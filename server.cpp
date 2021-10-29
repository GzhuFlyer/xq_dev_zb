#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>

#include "server.hpp"
#include "client_data_process.hpp"
#include "zmq/SyncService.hpp"
#include "ZB_data_processor.hpp"
#include "config/ConfigInfo.hpp"
#include "config/ConfigParse.hpp"


using namespace std;
using namespace xq;

extern int    *g_p_map;
int g_server_conn;
extern int g_buff_fd[2];
extern unsigned int g_portArray[3];
extern string        g_svr_ip;

bool RunService(const std::string& serviceName);

int g_command_port = 5177;

static void do_service(int conn)
{
    cout << "server + conn = " << conn << endl;
    g_server_conn = conn;
    char recvbuf[1024];
    static int wait_ms;
    wait_ms = 0;
    bool flag = true;
    printf("start the SyncService srv,g_svr_ip=%s,g_command_port=%d\n",g_svr_ip.c_str(),g_command_port);
    XQ_LOG_INFO("start the SyncService srv,g_svr_ip=%s,g_command_port=%d\n",g_svr_ip.c_str(),g_command_port);
    SyncService srv(g_svr_ip, g_command_port, &ZMQSyncServiceRecvCBForCommand, nullptr);
    
    while(1)
    {
        for(; flag && wait_ms<8; wait_ms++)
        {
            sleep(1);
            int len = recv(conn,recvbuf,10,MSG_PEEK|MSG_DONTWAIT);
            printf("len=%d\n",len);
            XQ_LOG_INFO("len=%d\n",len);
            if(len > 9)
            {
                printf("come to break \n");
                flag = false;
                break;
            }
            printf("wait_ms...=%d\n",wait_ms);
            XQ_LOG_INFO("wait_ms...=%d\n",wait_ms);
        }
        printf("wait_ms=%d\n",wait_ms);
        if(wait_ms>7)
        {
            break;
        }
        memset(recvbuf, 0, sizeof(recvbuf));
        int ret = read(conn,recvbuf,sizeof(recvbuf));
        if(ret == 0)    //当返回0表示客户端关闭了
        {
            printf("client close\n");
        }else if(ret == -1)
        {
           ERR_EXIT("read");
        }
        
        printf("\n******************************************\n");
        XQ_LOG_INFO("\n******************************************\n");
        printf("\n\nrecvbuf--->%s\n\n",recvbuf);
        XQ_LOG_INFO("\n\nrecvbuf--->%s\n\n",recvbuf);
        printf("\n******************************************\n");
        XQ_LOG_INFO("\n******************************************\n");


        
        write(g_buff_fd[1],recvbuf,strlen(recvbuf)+1);
    } 
    printf("do_service break\n");  
    XQ_LOG_INFO("do_service break\n");
    std::string killPre = "kill -9 $(pidof ";
    std::string kill = killPre + "RexGatewaySDKDemoApp" + ")";
    XQ_LOG_INFO("#run cmd: %s ", kill.c_str());
    system(kill.c_str());
    close(conn);
    sleep(2);
}

static void handle_sigchld(int sig)
{
    while (waitpid(-1,NULL,WNOHANG) > 0);
}

void startTcpServer(void)
{
    signal(SIGCHLD,handle_sigchld); 
    int listenfd;
    if((listenfd = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0)
        ERR_EXIT("socket");
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = PF_INET;
    servaddr.sin_port = htons(5188);
    servaddr.sin_addr.s_addr = inet_addr("0.0.0.0");

    int on = 1;
    if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)) < 0)
        ERR_EXIT("setsockopt");
    if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0)
        ERR_EXIT("bind");
    if(listen(listenfd,SOMAXCONN) < 0)
        ERR_EXIT("listen");
    struct sockaddr_in peeraddr;
    socklen_t peerlen = sizeof(peeraddr);
    int conn;
    pid_t pid;
    RunService("RexGatewaySDKDemoApp");
    while(1)
    {
        if((conn = accept(listenfd,(struct sockaddr*)&peeraddr,&peerlen)) < 0)
            ERR_EXIT("accept");
        printf("ip=%s,port=%d\n",inet_ntoa(peeraddr.sin_addr),ntohs(peeraddr.sin_port));
        XQ_LOG_INFO("ip=%s,port=%d\n",inet_ntoa(peeraddr.sin_addr),ntohs(peeraddr.sin_port));
        pid = fork();   
        if(pid == -1)
            ERR_EXIT("fork");
        if(pid == 0)  
        {   
            close(listenfd);
            do_service(conn);
            printf("RunService start....");
            XQ_LOG_INFO("RunService start....");
            RunService("RexGatewaySDKDemoApp");
            sleep(1);
            exit(EXIT_SUCCESS);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
        }else
        {
            close(conn);
        }
    }
    
}       

bool RunService(const std::string& serviceName)
{
    if(1)
    {
        xq::ConfigInfo ConfigInfo;
        xq::ConfigInfo* m_pConfigInfo = &ConfigInfo;
        bool ret = xq::ConfigParse::Parse(ConfigInfo);

        std::string killPre = "kill -9 $(pidof ";
        std::string kill = killPre + serviceName + ")";
        XQ_LOG_INFO("#run cmd: %s ", kill.c_str());
        system(kill.c_str());
        std::string str = "nohup ";
        std::string strCMD = str + m_pConfigInfo->BinPath() + "/" + serviceName + ">/tmp/xq_rex_stdin.log 2>&1 &";
        XQ_LOG_INFO("#run cmd: %s ", strCMD.c_str());
        system(strCMD.c_str());

        if (-1 == ret)
        {
            XQ_LOG_ERR("call cmd: %s false, return %d ", strCMD.c_str(), ret);
            return false;
        }
        else
        {
            if (WIFEXITED(ret))
            {
                if (0 == WEXITSTATUS(ret))
                {
                    return true;
                }
                else
                {
                    XQ_LOG_ERR("call cmd: %s false, exit code: %d ", strCMD.c_str(), WEXITSTATUS(ret));
                    return false;
                }
            }
            else
            {
                XQ_LOG_ERR("call cmd: %s false, exit code: %d ", strCMD.c_str(), WEXITSTATUS(ret));
                return false;
            }
        }
    }
   
}