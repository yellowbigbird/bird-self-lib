#pragma once

#include <vector>
#include <string>
#include "comDll.h"


class CMessage
{
public:
	CMessage()
		:m_time(0)
	{
	}

	__time64_t		m_time;  //int64
	std::string		m_ip;
	std::string		m_data;  //std::vector<byte>
	std::vector<byte>		m_addressData;
};

