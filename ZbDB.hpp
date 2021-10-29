/*
 * @Descripttion: 
 * @version: V1.0
 * @Author: Zongwu Fang
 * @Date: 2021-10-19 17:26:59
 * @LastEditors: Zongwu Fang
 * @LastEditTime: 2021-10-25 11:44:11
 * @mail: gzhuflyer@foxmail.com
 */
#ifndef ZB_DB_HPP
#define ZB_DB_HPP

#include<iostream>
#include<map>
#include<list>
#include<vector>
#include<thread>
#include<unistd.h>
#include<time.h>

#define ONLINE      true
#define OFFLINE     false

typedef std::string                     VALUE_T;
typedef std::string                     ST_T;
typedef std::string                     MAC_T;

typedef std::map<ST_T,VALUE_T>          ST_VALUE_T;
typedef std::map<MAC_T ,ST_VALUE_T>     DEV_TABLE_T;

using STATUS_T = bool;
using ZB_TIME_T = time_t;
using DEV_ONTIME_TABLE_T = std::map<MAC_T,ZB_TIME_T>; 
using STATUS_TABLE_T = std::map<MAC_T,STATUS_T>;

class ZigbeeDB
{
public:
    ~ZigbeeDB(){
        // std::cout<<"close ZigbeeDB "<<std::endl;
        printf("close ZigbeeDB ");
    }
    ZigbeeDB(const ZigbeeDB&)=delete;
    ZigbeeDB& operator=(const ZigbeeDB&)=delete;

	void InsertDevTable(MAC_T mac,ST_T stateType,VALUE_T value);

    std::string GetValueByST(MAC_T mac,ST_T stateType);
    std::string GetValueByMac(MAC_T mac);

    STATUS_T QueryDevStatus(MAC_T mac);
    STATUS_T UpdateDevStatus();
    void SetDevLatestTime(MAC_T mac);

	void ShowDeviceMessage(void);

    static ZigbeeDB& get_instance(){
        static ZigbeeDB instance;
        return instance;
    }
private:

    DEV_TABLE_T m_zb_table;
    DEV_ONTIME_TABLE_T m_devOnlineTime;
    STATUS_TABLE_T m_devStatus;
    ZigbeeDB(){
        std::cout<<"open ZigbeeDB "<<std::endl;
    }
};


#endif