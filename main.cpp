#include <iostream>
#include <unistd.h>
#include "server.hpp"
#include "ZbDB.hpp"
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <sys/types.h>
#include <sys/wait.h>

#include "jsonxx/json.hpp"
#include "zmq/SyncClient.hpp"
#include "zmq/SyncService.hpp"
#include "zmq/PubSub.hpp"
#include "config/ConfigParse.hpp"
#include "service_regist/ServiceRegist.hpp"
#include "threadpool/ThreadPool.hpp"
#include "lua/LuaCaller.hpp"
#include "common/Common.hpp"


#include "client_data_process.hpp"
#include "ZB_data_processor.hpp"



using namespace std;
using namespace xq;

#define SUCCESS  true
#define FAIL     false
#define BUF_SIZE 1024

int*          g_p_map;
int           g_buff_fd[2];
string        g_svr_ip;
string        g_svr_name = "xq_svr_comm_zigbee";

unsigned int g_portArray[3];

static void updateAllDevStatus();
static bool getIpPort();
void UploadGetZmqMsg(const std::string &strRecvMsg, std::string &responseData, void *privateData);


static void handle_sigchld(int sig)
{

    while (waitpid(-1,NULL,WNOHANG) > 0);
}

int main()
{
    signal(SIGCHLD,handle_sigchld); 

    if(FAIL == getIpPort())
    {
        ERR_EXIT("getIpPort");
    }else
    {
        printf("Regist local returnback info: %s : %d : %d : %d ", g_svr_ip.c_str(), g_portArray[0], g_portArray[1], g_portArray[2]);
        XQ_LOG_INFO("Regist local returnback info: %s : %d : %d : %d ", g_svr_ip.c_str(), g_portArray[0], g_portArray[1], g_portArray[2]);
    }
    

    if(-1 == pipe(g_buff_fd))
    {
        ERR_EXIT("pipe");
    }
    
    pid_t pid;
    pid = fork();   
    if(pid == -1)
        ERR_EXIT("fork");
    if(pid == 0) {
            close(g_buff_fd[0]);
            startTcpServer();
    }else{
        SyncService srv(g_svr_ip, g_portArray[0], &ZMQSyncServiceRecvCB, nullptr);
        thread th1(updateAllDevStatus);
        th1.detach();
        close(g_buff_fd[1]);
        while(1){
            char tempbuf[BUF_SIZE];
            char *temp_p_map = tempbuf;
            memset(tempbuf,0,sizeof(tempbuf));
            ssize_t s = read(g_buff_fd[0],tempbuf,sizeof(tempbuf));
            for( int i=0,j=0;;)
            {
                static int k=0;
                if(temp_p_map[i]=='{')  j++;
                if(temp_p_map[i]=='}')  j--;
                if(j==0)
                {
                    char tempBuf[1024];
                    memset(tempBuf,0,sizeof(tempBuf));
                    memcpy(tempBuf,&temp_p_map[k],i-k+1);
                    // std::cout << "tempBuf=" << tempBuf << endl;
                    std::cout << "temp_p_map["<<k<<"]="<<temp_p_map[k]<<std::endl;
                    if(0 != &temp_p_map[k])
                        StoreDataInZbDB(tempBuf);
                    k=i+1;          
                }
                i++;
                if(temp_p_map[i]==0 || temp_p_map[i]=='\0')
                {
                    k=0;
                    break;
                }
            }
        }
    }
    xq::SyncService* m_pSyncService = new xq::SyncService(g_svr_ip, g_portArray[0], UploadGetZmqMsg, (void *)nullptr);
}

static void updateAllDevStatus()
{
    ZigbeeDB& myZb = ZigbeeDB::get_instance();
    while (1)
    {
        myZb.UpdateDevStatus();
    }
}
bool isJsonBodyLegal(jsonxx::json jObj)
{
    if (jObj["msgId"] != nullptr && jObj["msgId"].is_string() &&
        jObj["type"] != nullptr && jObj["type"].is_string() &&
        jObj["action"] != nullptr && jObj["action"].is_string())
    {
        return true;
    }
    return false;
}
void UploadGetZmqMsg(const std::string &strRecvMsg, std::string &responseData, void *privateData)
{
    if (!privateData)
    {
        XQ_LOG_ERR("privateData is null!", "");
        return;
    }
    std::string strMsgId;
    std::string strAction;
    XQ_LOG_DEBUG("收到上行服务请求的信息内容: %s ", strRecvMsg.c_str());
    printf("come to here\n");
    static uint64_t recv_count=0; recv_count++;
    printf("recv_count:%ld  ------%s\n",recv_count,strRecvMsg.c_str());
    jsonxx::json jsonParese = jsonxx::json::parse(strRecvMsg);
    if (isJsonBodyLegal(jsonParese))
    {
        strMsgId = jsonParese["msgId"].as_string();
        strAction = jsonParese["action"].as_string();
        jsonxx::json respJson = {
            {"msgId", strMsgId},
            {"type", "resp"},
            {"action", strAction},
            {"resultCode", 0}};
        if (strAction == "heartbeat")
        {
            responseData = respJson.dump();
        }
    }
    else
    {
        XQ_LOG_ERR("Parse recv json fail!", "");
    }
}

static bool getIpPort()
{
    ServiceRegist *m_sServiceReg;
    m_sServiceReg = new ServiceRegist();
    for(int tryTimes=0; tryTimes < 30 ; tryTimes++)
    {
        if(FAIL == m_sServiceReg->Init())
                continue;
        if(FAIL == m_sServiceReg->Regist(g_svr_name, g_svr_ip, g_portArray[0], g_portArray[1], g_portArray[2])) 
                continue;
        if(SUCCESS ==  m_sServiceReg->BindNotify(g_svr_name))  
                return SUCCESS; 
    }
    return FAIL;
}
