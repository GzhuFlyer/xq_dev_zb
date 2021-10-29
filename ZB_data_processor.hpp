/*
 * @Descripttion: 
 * @version: V1.0
 * @Author: Zongwu Fang
 * @Date: 2021-08-06 09:35:49
 * @LastEditors: Zongwu Fang
 * @LastEditTime: 2021-10-25 16:13:39
 * @mail: gzhuflyer@foxmail.com
 */
#ifndef ZB_DATA_PROCESSOR_HPP
#define ZB_DATA_PROCESSOR_HPP

#include<iostream>
#include <unistd.h>
#include <iostream>
#include "jsonxx/json.hpp"
#include "ZbDB.hpp"
#include "common/Common.hpp"
#include <string.h>

extern void ZMQSyncServiceRecvCB(const std::string& recv_data, std::string& response_data, void* privateData);
void ZMQSyncServiceRecvCBForCommand(const std::string& recv_data, std::string& response_data, void* privateData);



class ZMQDataProcess
{
public:  
	
	
};


#endif







