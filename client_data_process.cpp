#include "client_data_process.hpp"
#include "ZbDB.hpp"
#include "jsonxx/json.hpp"
#include <iostream>
#include <sstream>

using namespace std;

void StoreDataInZbDB(char* inData)
{
    std::string data = inData;
    std::cout << "store-> " << data << std::endl;

    jsonxx::json jP = jsonxx::json::parse(data);
    std::string mac = jP["Address"].as_string();
     
    std::string stateType; 
    if(data.npos != data.find("\"StateType\"")){
        cout << "is stateType" << endl;
        stateType = jP["StateType"].as_string();
        cout << "stateType= " << stateType << endl;
    }else if(data.npos != data.find("\"EventType\"")){
        cout << "is EventType" << endl;
        stateType = jP["EventType"].as_string();
        cout << "EventType= " << stateType << endl;
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
    ZigbeeDB& myZb = ZigbeeDB::get_instance();
	myZb.InsertDevTable(mac,stateType,strValue);
    myZb.ShowDeviceMessage();
    myZb.SetDevLatestTime(mac);
}