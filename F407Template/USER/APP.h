#ifndef _APP_H
#define _APP_H

#include "Protocol.h"
#include "ADC.h"
#include "GPRS.h"
#include "GPIO.h"
#include "SHT2X.h"
#include "stdio.h"
#include "W25QXX.h"
#include "Monitor.h"
#include "DPPrint.h"
#include "GidLink.h"
#include "SoftwareI2C.h"
#include "Socket_esp8266.h"
#include "FIFOBuffer.h"

#define TRAIN_SIZE						50
#define NAMEJOB_SIZE					50

#define TAGDATA_BUFFER_SIZE 	50

#define SETDATAADD 						12000
#define TRAINADD  						13000
#define NAMEADD 							16000
#define TAGDATAADD 						22000

#define TAGMAXNUM							200
#define TRAINMAXNUM						200
#define PEOPLEMAXNUM					200

extern TaskManager tskmgr;
/***** USB CP2102 ******/
extern USART com1;
extern UidLink mUSBCom;
/***** MEASURE ******/
extern USART com2;
/***** WIFI_ESP8266 ******/
extern USART com3;
extern Socket_esp8266 mWifi;
/***** Monitor ******/
extern USART com4;
extern Monitor mMonitor;
/***** Printer ******/
extern USART com5;
extern DPPrint mPrint;
/***** GPRS ******/
extern USART com6;
extern GPRS mGPRS;
/***** 12V ENABLE ******/
extern GPIO Vol_EN;
/***** SHT20 ******/
extern SHT2X mSHT20;
/***** W25Q32 *****/
//extern SPI mSPI;
//extern W25QXX mW25QXX;
/*** VOLT MEASURE ***/
extern MADC mAdc;

class APP{

private:
	FIFOBuffer<TagStructer, TAGDATA_BUFFER_SIZE>  mTagDataQueue;  //TagDataStructer FIFOBuffer
	TraNum mTraNum[TRAIN_SIZE]; 						/*** �����б� ***/
	NameJob mNameJob[NAMEJOB_SIZE];					/*** ����-�����б� ***/
	SetDataStruct mSetData;			/*** ��ŷ��������Ϣ ***/
	TagData 		mTempData;
	TagStructer mTempDataStru;  /*** ��ŵ�ǰ���ڲ��������� ***/
	char mPageNum;
	TraNum mTempTra[4];

public:
//	DPPrint 			 &mPrint;
////	W25QXX  			 &mW25QXX;
//	GPRS 					 &mGPRS;
//	SHT2X 				 &mSHT20;
//	UidLink 			 &mMeaCom;
//	UidLink 			 &mUSBCom;
//	Monitor 			 &mMonitor;
//	Socket_esp8266 &mWifi;
//	MADC					 &mADC;

	u32 TagDataAdd;
	OperatorPeoPle ChPeoSel;
	MeggerChannel ChMeaSel;

public:
//	APP(DPPrint &print,GPRS &mGP,SHT2X &mSHT,UidLink &mMea,UidLink &mUSB,Monitor &mMonit,Socket_esp8266 &mSocket,MADC &mAD);
	APP();
	
	/**** ��ʼ�� ***/
	void SH2X_Init(void);
	void GPRS_Init(void);
	void Wifi_Init(void);

	void ReadSetData(void);
	void WriteSetData(void);
	void ReadNameJob(void);
	void WriteNameJob(void);
	char *FindName(char *JobNum);
	void ReadTrain(void);
	void WriteTrain(void);
	void ReadTagStructer(void);

	char * Measure(void);
	void ClearTempDataStru(void);

	void RefreshTrain(u8 PageNum); /**** ������ҳʱ ˢ�³����б� ****/
	void Update();
	void UpdateMonitor(void);
	void UpdateDataQueue(void);
	void CheckUSBFrame(void);
	void CheckStatus(void);   		 /**** ��� GPRS WIFI TCP ����״̬ ****/
};

#endif
