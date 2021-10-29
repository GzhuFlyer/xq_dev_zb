/*
 * @Descripttion: 
 * @version: V1.0
 * @Author: Zongwu Fang
 * @Date: 2021-10-19 17:26:59
 * @LastEditors: Zongwu Fang
 * @LastEditTime: 2021-10-25 16:40:21
 * @mail: gzhuflyer@foxmail.com
 */
#include "client_data_process.hpp"
#include "ZbDB.hpp"
#include "jsonxx/json.hpp"
#include <iostream>
#include <sstream>
#include "syslog/XqLog.hpp"


using namespace std;

void StoreDataInZbDB(char* inData)
{
    std::string data = inData;
    std::cout << "store-> " << data << std::endl;
    XQ_LOG_INFO("store->");

    jsonxx::json jP = jsonxx::json::parse(data);
    std::string mac = jP["Address"].as_string();
     
    std::string stateType; 
    XQ_LOG_INFO("is stateType=%s",stateType.c_str());
    if(data.npos != data.find("\"StateType\"")){
        cout << "is stateType" << endl;
        stateType = jP["StateType"].as_string();
        cout << "stateType= " << stateType << endl;
    }else if(data.npos != data.find("\"EventType\"")){
        cout << "is EventType" << endl;
        stateType = jP["EventType"].as_string();
        cout << "EventType= " << stateType << endl;
        cout << "stateType= " << stateType << endl;
    }
    //add patch.
    //tell same stateType in one device ,according EndpointId.
    std::string EndpointId ;
    if(jP["EndpointId"].is_string())
    EndpointId = jP["EndpointId"].as_string();
    if(EndpointId != "1")
    {
        stateType += "+";
        stateType += EndpointId;
    }

    std::stringstream ss;
    if(data.npos != data.find("\"State\"")){
        cout << "is State" << endl;
        auto value = jP["State"].as_object();
        cout << "value= " << value << endl;
        ss << value;
    }else if(data.npos != data.find("\"Event\"")){
        cout << "is Event" << endl;
        auto value = jP["Event"].as_object();
        cout << "value= " << value << endl;
        ss << value;
    }

    string strValue;
    string out;
    while(ss>>out)
    {
        strValue += out.c_str();
    }
    cout << "strValue=" << strValue << endl;
    XQ_LOG_INFO("strValue=%s",strValue.c_str());
    ZigbeeDB& myZb = ZigbeeDB::get_instance();
	myZb.InsertDevTable(mac,stateType,strValue);
    myZb.ShowDeviceMessage();
    myZb.SetDevLatestTime(mac);
}