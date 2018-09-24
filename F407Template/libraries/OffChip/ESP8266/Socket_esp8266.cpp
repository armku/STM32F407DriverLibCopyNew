#include "Socket_esp8266.h"

Socket_esp8266::Socket_esp8266(USART& usart)
	:esp8266(usart), mUsart(usart), mIsConnected(false)
{
	mTimeOut = 3;

}

bool Socket_esp8266::Init()
{
	return true;
}

bool Socket_esp8266::Connect(char* ipAddr, short port, Socket_Type socketType, Socket_Protocol socketProtocol)
{
	if (CreateConnectMutipleMode(ipAddr, port, socketType))
		mIsConnected = true;
	else
		mIsConnected = false;
	return mIsConnected;
}

bool Socket_esp8266::GetConnectStatus(void)
{
	return mIsConnected;
}

bool Socket_esp8266::Write(char* data, unsigned int num)
{
	return SendMultipleMode(data, num);
}

unsigned int Socket_esp8266::Read(char* data)
{
	float starttime = TaskManager::Time();
	if (!mIsConnected)
		return 0;
	unsigned char temp[13];
	unsigned short bufferSize = 0;
	unsigned short dataLength = 0;
	unsigned short count = 0;
	bool flag = false;
	//读出数据长度
	while (TaskManager::Time() - starttime < mTimeOut){
		bufferSize = mUsart.RxSize();
		if (bufferSize > 6)//数据帧格式：+IPD,长度:数据
		{
			for (count = 0; (count < bufferSize) && (count < ESP8266_RECEIVE_BUFFER_SIZE); ++count)
			{
				mUsart.GetBytes(temp, 1);
				if (temp[0] == '+')
				{
					if (!mIsEnableMUX)
						RecvFindAndFilter(":", ",", ":", (char*)temp, mTimeOut);
					else
					{
						if (RecvFind(":", mTimeOut))
						{
							char* index1 = strstr(mReceiveBuffer, ",");
							index1 = strstr(index1, ",");
							char* index2 = strstr(mReceiveBuffer, ":");

							if (index1&&index2)
							{
								index1 += strlen(",");
								*index2 = '\0';
								strcpy((char*)temp, index1);
								return true;
							}
						}
					}
					dataLength = atoi((char*)temp);
					flag = true;
					break;
				}
			}
		}
		if (flag)
			break;
	}
	count = 0;
	//读取数据
	while (TaskManager::Time() - starttime < mTimeOut){
		if (mUsart.RxSize()>0)
		{
			mUsart.GetBytes((unsigned char*)(data + count), 1);
			++count;
			if ((count == dataLength) || (count == ESP8266_RECEIVE_BUFFER_SIZE))
				break;
		}
	}
	return count;
}

unsigned int Socket_esp8266::Read(char* data, unsigned int num)
{
	float starttime = TaskManager::Time();
	if (!mIsConnected)
		return 0;
	unsigned int count = 0;
	while (TaskManager::Time() - starttime < mTimeOut&&count < num){
		if (mUsart.RxSize() > 0)
		{
			mUsart.GetBytes((unsigned char*)(data + count), 1);
			++count;
		}
	}
	return count;
}

bool Socket_esp8266::IsAlive()
{
	if (!mIsConnected)
		return false;
	if (GetStatus() == 3)
		return true;
	return false;
}

bool Socket_esp8266::Close()
{
	if (mIsConnected)
	{
		esp8266::Close();
	}
	return true;
}

void Socket_esp8266::SetTimeOut(float timetOut)
{
	mTimeOut = timetOut;
}

bool Socket_esp8266::SetMUX(bool isEnableMUX)
{
	mIsEnableMUX = isEnableMUX;
	return esp8266::SetMUX(isEnableMUX);
}
//****************** 应用于兆欧表
bool Socket_esp8266::SendTagData(char* data, u8 num)
{
	u8 count = 0, i = 0;
	SendFrame sendFrame;
	sendFrame.FrameHead = 0x68;
	sendFrame.Command = 0x00;
	sendFrame.DataLength = sizeof(TagData);
	for(i=0;i<sizeof(TagData);i++)
		sendFrame.Data[i] = data[i];
	sendFrame.CRC8 = CRC8Calculate((uint8_t *)&sendFrame.FrameHead, sizeof(TagData) + 3);
	sendFrame.FrameEnd = 0x16;
	while (!Write(&sendFrame.FrameHead, sizeof(TagData) + 5) && (count++ < 20));
	if(count > 19)
		return false;
	return true;
}
//****************** 应用于兆欧表
