#include "ZB_data_processor.hpp"
#include <unistd.h>
#include <iostream>
#include "jsonxx/json.hpp"
#include "ZbDB.hpp"
#include "common/Common.hpp"

using namespace std;
using namespace xq;

// static int g_server_conn;
extern int g_server_conn;
extern int g_server_conn_fd[2];

void ZMQSyncServiceRecvCB(const std::string& recv_data, std::string& response_data, void* privateData)
{
 
    std::cout << "recv: " << recv_data << std::endl;

    jsonxx::json jP = jsonxx::json::parse(recv_data);
    std::string strAction = jP["action"].as_string();
    std::string strMsgId = jP["msgId"].as_string();
    cout << "strMsgId=" << strMsgId << ", strAction=" << strAction << endl;
    std::string resp_value;

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
        cout << "reqMac------      "  << reqMac << endl;
        cout << "reqStateType--------     "  << reqStateType << endl;
        VALUE_T respValue =  myZb.GetValueByST(reqMac,reqStateType);
        cout << "************         " << respValue << endl;
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
        cout << "response_data````````  " << response_data  << endl;
        return;
    }
    std::string reqMac = jP["payload"]["mac"].as_string();
    static std::string respValue = " ";
    ZigbeeDB& rcvDB = ZigbeeDB::get_instance();
    rcvDB.ShowDeviceMessage();
    resp_value = rcvDB.GetValueByMac(reqMac);
    cout << "resp_value=" << resp_value << endl;
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
  std::cout << "recv: " << recv_data << std::endl;

    jsonxx::json jP = jsonxx::json::parse(recv_data);
    std::string strAction = jP["action"].as_string();
    std::string strMsgId = jP["msgId"].as_string();
    cout << "strMsgId=" << strMsgId << ", strAction=" << strAction << endl;
    std::string resp_value;

    if(strAction == "write")
    {
        //form package
        std::string reqMac = jP["payload"]["mac"].as_string();
        std::string reqCommand = jP["payload"]["command"].as_string();
        std::string reqStateType = jP["payload"]["stateType"].as_string();
        cout << "come to here" << endl;
        // char temp[] = reqCommand.c_str();
        cout << "sizeof(reqCommand.c_str())=" << reqCommand.length() << endl;
        cout << "reqCommand=" << reqCommand << endl;
        write(g_server_conn,reqCommand.c_str(),reqCommand.length());
        jsonxx::json jpld = {
            {"mac",reqMac},
            {"result",0}
        };
        resp_value = jpld.dump();
        // jsonxx::json jPC = jsonxx::json::parse(reqCommand);
        // string disireValue = jPC["Command"]["State"].as_string();
        // ZigbeeDB& rcvDB = ZigbeeDB::get_instance();
        // string now_value = rcvDB.GetValueByST(reqMac,reqStateType);
        // int setResult = -1;
        // if(now_value == disireValue)
        // {
        //     setResult = 0;
        //     cout << "setResult --->" << setResult << endl;
        // }
        jsonxx::json j = {
            {"msgId", strMsgId},
            {"ts",time(NULL)},
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
