#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include "stm32f4xx.h"

typedef enum{
	ASENDED = 0,
	ANOTSEND = 1,
	AWAITRESP = 2
} TagStatus;

typedef enum{
	PRINTED = 0,
	NOTPRINT = 1,
	WAITPRINT = 2
} PrintFlags;

typedef enum{
	SENDED = 0,
	NOTSEND = 1,
	WAITRESP = 2
} SendFlags;

typedef enum{
	SAVEED = 0,
	NOTSAVE = 1,
	WAITSAVE = 2
} SaveFlags;

typedef enum{
	USEGPRS = 0,
	USEWIFI = 1,
} USEWIFIORGPRS;

typedef enum MeggerChannel{ 
	CH_D600P = 0,
	CH_D600N = 1,
	CH_D110P = 2,
	CH_D110N = 3,
	CH_DPN = 5,
} MeggerChannel;

typedef enum CHTRAIN{ 
	CHTRAIN1 = 1,
	CHTRAIN2 = 2,
	CHTRAIN3 = 3,
	CHTRAIN4 = 4,
	CHTRAIN5 = 5,
} CHTRAIN;

typedef enum PeoPleEnum{ 
	PeoPleCh1 = 0,
	PeoPleCh2 = 1,
	PeoPleCh3 = 2,
} OperatorPeoPle;

//******** 打印数据*******************/
typedef struct TagData{
	char TrainNum[10];
	char DateTIME[20];
	char HUMIDTY[5];
	char CH_MEA[5][5];//MeasureCh data 5行*5列
	char PeoPle[3][10];//People 3行*10列
//	char CH_D600P[5];
//	char CH_D600N[5];
//	char CH_DPN[5];
//	char CH_D110P[5];
//	char CH_D110N[5];
//	char PeoPle1[10];
//	char PeoPle2[10];
//	char PeoPle3[10];
} TagData;

/******** BUTTON ENUM************/
typedef enum{
	NULLBTN			= 0,
	CH_D110PBTN = 1,
	CH_D110NBTN = 2,
	CH_D600PBTN = 3,
	CH_D600NBTN = 4,
	CH_DPNBTN 	= 5,
	PEOBTN1 		= 6,
	PEOBTN2 		= 7,
	PEOBTN3 		= 8,
	PEOSURE 		= 9,
	PRINTBTN 		= 10,
	TRAINSELECT	= 11,
	NEXTTRAINPAGE=12,
	LASTTRAINPAGE=13,
	TRAINSELECTED=14,
	CLEARBTN		= 15,
	GPRSSET 		= 16,
	GPRSUNSET		= 32,
	WIFISET 		= 17,
	WIFISSIDTEXT= 18,
	WIFIKEYTEXT = 19,
	IPADDRTEXT 	= 20,
	IPPORTTEXT 	= 21,
	CH_TRAIN1		= 22,
	CH_TRAIN2		= 23,
	CH_TRAIN3		= 24,
	CH_TRAIN4		= 25,
	PRINTSUREBTN= 26,
	MEASUREBTN  = 27,
	READDATETIME= 28,
	WIFISETTING = 29,
	GPRSSETTING = 30,
	CONFIGBTN   = 31,
} BtnType;

/****** 屏幕 *********/
typedef struct CommandStruct{
	BtnType mBtnType;
	char data[20];
} CommandData;

//****** 打印数据及状态链接*****************/
typedef struct TagDataStruct{
	TagData mTagData;
	char ALLStatus;
	char PrintFlag;
	char SendFlag;
	char SaveFlag;
} TagStructer;

//********** 兆欧表配置 *************/
typedef struct SetData{
	char USEWIFIORGPRS;
	char IPADDR[20];
	char IPPORT[10];
	char WIFISSID[30];
	char WIFIKEY[30];
	char PEOPLENUM;
	char TRAINNUM;
	char TAGNUM;
	//u16
} SetDataStruct;

//******* 工号-姓名链***************/
typedef struct Name{
	char NAME[20];
	char JOBNUM[20];
} NameJob;

//****** 车次 **************/
typedef struct TrainNumL{
	char TrainNumL[10];
} TraNum;

typedef union DataConvert
{
	int byInt;
	float byFloat;
	u8  byByte[4];
} dataConvert;

typedef struct SendFrame{
	char FrameHead;
	char Command;
	char DataLength;
	char Data[sizeof(TagData)];
	char CRC8;
	char FrameEnd;
}SendFrame;

#endif
