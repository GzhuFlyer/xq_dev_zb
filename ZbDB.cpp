// ***************************************************//

#include "ZbDB.hpp"
#include "syslog/XqLog.hpp"

using namespace std;



void ZigbeeDB::ShowDeviceMessage(void)
{
	DEV_TABLE_T::iterator tempMacIt,tempMacEndIt;
	tempMacIt = m_zb_table.begin();
	tempMacEndIt =m_zb_table.end();

	cout << "======show all device relative message========" << endl;
	XQ_LOG_INFO("======show all device relative message========");
	while(tempMacIt != tempMacEndIt)
	{
		ST_VALUE_T::iterator showStateType;

		showStateType = tempMacIt->second.begin();
		cout << "\t====================================" << endl; 
		cout << "\tMac=" << tempMacIt->first << endl;
		XQ_LOG_INFO("\t====================================");
		XQ_LOG_INFO("\tMac=%s", tempMacIt->first.c_str());
		while(showStateType != tempMacIt->second.end())
		{
			cout << "\tstateType=" << showStateType->first << "\t";
			cout << "\tValue=" << showStateType->second << endl;
			showStateType++;
		}
		cout << "\t====================================" << endl; 
		XQ_LOG_INFO("\t====================================");
		tempMacIt++;
	}

}

void ZigbeeDB::InsertDevTable(MAC_T mac,ST_T stateType,VALUE_T value)
{
    DEV_TABLE_T::iterator tableIt;
    ST_VALUE_T temp_st_value;
    ST_VALUE_T::iterator tempST_valueIt;

    tableIt = m_zb_table.find(mac);
	cout << "the mac is " << mac << endl; 
	XQ_LOG_INFO("the mac is %s ",mac.c_str());
	if(tableIt == m_zb_table.end())
    {
        cout << "no relative device store in m_zb_table ,start store " << endl;
		XQ_LOG_INFO("no relative device store in m_zb_table ,start store ");
        temp_st_value.insert(pair<ST_T,VALUE_T>(stateType,value));
        m_zb_table.insert(pair<MAC_T,ST_VALUE_T>(mac,temp_st_value));
    }else{
        tempST_valueIt = tableIt->second.find(stateType);
        if(tempST_valueIt != tableIt->second.end())
        {
            tableIt->second.erase(stateType);
        }
        tableIt->second.insert(pair<ST_T,VALUE_T>(stateType,value));
    }
}

std::string ZigbeeDB::GetValueByST(MAC_T mac,ST_T stateType){
	DEV_TABLE_T::iterator tempMacIt;
	tempMacIt = m_zb_table.find(mac);
	std::string retVal = "";

	if(tempMacIt != m_zb_table.end())
	{
		ST_VALUE_T::iterator showStateType;

		showStateType = tempMacIt->second.find(stateType);
		if(showStateType != tempMacIt->second.end())
		{
			if(showStateType->first == stateType)
			{
				retVal += showStateType->second;
				cout << "retVal=" << retVal << endl;
				XQ_LOG_INFO("retVal=%s",retVal);
				return retVal;
			}
			showStateType++;
		}
	}else
	return retVal;
}

std::string ZigbeeDB::GetValueByMac(MAC_T mac){
	DEV_TABLE_T::iterator tempMacIt;
	tempMacIt = m_zb_table.find(mac);
	std::string retVal = "";

	if(tempMacIt != m_zb_table.end())
	{
		ST_VALUE_T::iterator showStateType;
		std::string temp_retValue="";
		showStateType = tempMacIt->second.begin();
		while(showStateType != tempMacIt->second.end())
		{
			temp_retValue += "{\"stateType\":\"";
			temp_retValue += showStateType->first;
			temp_retValue += "\",";
			temp_retValue += "\"value\":";
			temp_retValue += showStateType->second;
			temp_retValue += "} ";
			showStateType++;
			if(showStateType != tempMacIt->second.end())
				temp_retValue += ',';
		}
		retVal = temp_retValue;
	}
	return retVal;
}

STATUS_T ZigbeeDB::QueryDevStatus(MAC_T mac)
{
	return m_devStatus.find(mac)->second;
}
STATUS_T ZigbeeDB::UpdateDevStatus()
{
	DEV_ONTIME_TABLE_T::iterator timeBegin, timeEnd;
	timeBegin = m_devOnlineTime.begin();
	timeEnd = m_devOnlineTime.end();
	while(timeBegin != timeEnd)
	{
		if( (time(NULL)-timeBegin->second) > 3)
		{
			m_devStatus.erase(timeBegin->first);
			m_devStatus.insert(pair<MAC_T,STATUS_T>(timeBegin->first,OFFLINE));
		}
		timeBegin++;
	}
	sleep(1);
}
void ZigbeeDB::SetDevLatestTime(MAC_T mac)
{
	DEV_ONTIME_TABLE_T::iterator timeIt;
	STATUS_TABLE_T::iterator statusIt;
	timeIt = m_devOnlineTime.find(mac);
	statusIt = m_devStatus.find(mac);
	if(m_devOnlineTime.end() != timeIt)
	{
		m_devOnlineTime.erase(mac);
	}
	if(m_devStatus.end() != statusIt)
	{
		m_devStatus.erase(mac);
	}
	m_devOnlineTime.insert(pair<MAC_T,ZB_TIME_T>(mac,time(NULL)));
	m_devStatus.insert(pair<MAC_T,STATUS_T>(mac,ONLINE));
}

