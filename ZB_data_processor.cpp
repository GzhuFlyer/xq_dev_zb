#include "ZB_data_processor.hpp"
#include "syslog/XqLog.hpp"

using namespace std;
using namespace xq;

extern int g_server_conn;
extern int g_server_conn_fd[2];
extern int    *g_p_map;

void ZMQSyncServiceRecvCB(const std::string& recv_data, std::string& response_data, void* privateData)
{
 
    jsonxx::json jP = jsonxx::json::parse(recv_data);
    if (jP["msgId"] == nullptr || !jP["msgId"].is_string() ||
        jP["type"] == nullptr || !jP["type"].is_string() ||
        jP["action"] == nullptr || !jP["action"].is_string())
    {
       return;
    }
    std::string strAction = jP["action"].as_string();
    std::string strMsgId = jP["msgId"].as_string();
    std::string resp_value;

    if (strAction == "heartbeat")
    {
        jsonxx::json respJson = {
        {"msgId", strMsgId},
        {"type", "resp"},
        {"action", strAction},
        {"resultCode", 0}};
        response_data = respJson.dump();
        return;
    }

    if(strAction == "getHB")
    {
        ZigbeeDB& myZb = ZigbeeDB::get_instance();
        std::string reqMac = jP["payload"]["mac"].as_string();
        STATUS_T devStatus = myZb.QueryDevStatus(reqMac);
        std::string strDevStatus = to_string(devStatus);
        jsonxx::json jpld = {
            {"mac",reqMac},
            {"status",strDevStatus}
        };
        resp_value = jpld.dump();
        jsonxx::json j = {
            {"msgId", strMsgId},
            {"ts",time(NULL)},
            {"src","xq_svr_comm_zigbee"},
            {"dst","xq_svr_dev_mng"},
            {"type", "resp"},
            {"action", strAction},
            {"resultCode",0},
            {"payload",jpld}
        };
        std::string respStr = j.dump();
        response_data = respStr;
        return;
    }
    if(strAction == "CkSetAttr")
    {
        ZigbeeDB& myZb = ZigbeeDB::get_instance();
        std::string reqMac = jP["payload"]["mac"].as_string();
        string reqStateType = jP["payload"]["StateType"].as_string();
        VALUE_T respValue =  myZb.GetValueByST(reqMac,reqStateType);
        jsonxx::json jpld = {
            {"mac",reqMac},
            {"value",respValue}
        };
        resp_value = jpld.dump();
        jsonxx::json j = {
            {"msgId", strMsgId},
            {"ts",xq::GetTS()},
            {"src","xq_svr_comm_zigbee"},
            {"dst","xq_svr_dev_mng"},
            {"type", "resp"},
            {"action", strAction},
            {"resultCode",0},
            {"payload",jpld}
        };
        std::string respStr = j.dump();
        response_data = respStr;
        return;
    }

    std::string reqMac = jP["payload"]["mac"].as_string();
    static std::string respValue = " ";
    ZigbeeDB& rcvDB = ZigbeeDB::get_instance();
    // rcvDB.ShowDeviceMessage();
    resp_value = rcvDB.GetValueByMac(reqMac);
    jsonxx::json j = {
            {"msgId", strMsgId},
            {"ts",xq::GetTS()},
            {"src","xq_svr_comm_zigbee"},
            {"dst","xq_svr_dev_mng"},
            {"type", "resp"},
            {"action", strAction},
            {"resultCode",0},
            {"payload",resp_value}
    };
    std::string respStr = j.dump();
    response_data = respStr;
}

void ZMQSyncServiceRecvCBForCommand(const std::string& recv_data, std::string& response_data, void* privateData)
{
   
    cout << "func:" << __func__ << ", line:" << __LINE__ << endl;
    jsonxx::json jP = jsonxx::json::parse(recv_data);

    if (jP["msgId"] == nullptr || !jP["msgId"].is_string() ||
        jP["type"] == nullptr || !jP["type"].is_string() ||
        jP["action"] == nullptr || !jP["action"].is_string())
    {
       return;
    }

    std::string strAction = jP["action"].as_string();
    std::string strMsgId = jP["msgId"].as_string();
    std::string resp_value;
    std::cout << strAction << endl;
    if(strAction == "write")
    {
        std::string reqMac = jP["payload"]["mac"].as_string();
        std::string reqCommand = jP["payload"]["command"].as_string();
        std::string reqStateType = jP["payload"]["stateType"].as_string();
        cout << "func:" << __func__ << ", line:" << __LINE__ << endl;
        cout << "g_server_conn=" << g_server_conn << endl;
        XQ_LOG_INFO("g_server_conn=%d",g_server_conn);
        write(g_server_conn,reqCommand.c_str(),reqCommand.length());
        jsonxx::json jpld = {
            {"mac",reqMac},
            {"result",0}
        };
        resp_value = jpld.dump();
        jsonxx::json j = {
            {"msgId", strMsgId},
            {"ts",xq::GetTS()},
            {"src","xq_svr_comm_zigbee"},
            {"dst","xq_svr_dev_mng"},
            {"type", "resp"},
            {"action", strAction},
            {"result",0},
            {"payload",jpld}
        };
        std::string respStr = j.dump();
        response_data = respStr;
        return;
    }

}
