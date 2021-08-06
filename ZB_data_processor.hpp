#ifndef ZB_DATA_PROCESSOR_HPP
#define ZB_DATA_PROCESSOR_HPP

#include<iostream>

extern void ZMQSyncServiceRecvCB(const std::string& recv_data, std::string& response_data, void* privateData);
void ZMQSyncServiceRecvCBForCommand(const std::string& recv_data, std::string& response_data, void* privateData);



class ZMQDataProcess
{
public:  
	
	
};


#endif







