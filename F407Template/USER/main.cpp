#include "stm32f4xx.h"
#include "stdio.h"
#include "math.h"

/***** OnChip *****/
#include "USART.h"
#include "ADC.h"
#include "I2C.h"
#include "Timer.h"
#include "GPIO.h"
#include "SoftwareI2C.h"

/****** OffChip *****/
#include "DPPrint.h"
#include "W25QXX.h"
#include "GPRS.h"
#include "SHT2X.h"
#include "Socket_esp8266.h"
#include "Monitor.h"

/***** System *****/
#include "TaskManager.h"
#include "Configuration.h"
#include "APP.h"

/******* ToolBox *****/
#include "GidLink.h"

TaskManager tskmgr;
/***** USB CP2102 ******/
USART com1(USART1, 115200);
UidLink mUSBCom(com1);
/***** MEASURE ******/
USART com2(USART2, 115200);
/***** WIFI_ESP8266 ******/
USART com3(USART3, 115200, true);
Socket_esp8266 mWifi(com3);
//GPIO WIFI_EN(GPIOE, 0, GPIO_Mode_OUT, GPIO_OType_PP);
//GPIO WIFI_RST(GPIOE, 0, GPIO_Mode_OUT, GPIO_OType_PP);
/***** Monitor ******/
USART com4(UART4, 115200, true);
Monitor mMonitor(com4);
/***** Printer ******/
USART com5(UART5, 115200);
DPPrint mPrint(com5);
/***** GPRS ******/
USART com6(USART6, 115200);
GPRS mGPRS(com6);
/***** 12V ENABLE ******/
GPIO Vol_EN(GPIOE, 0, GPIO_Mode_OUT, GPIO_OType_PP);
/***** SHT20 ******/
SHT2X mSHT20;
/***** W25Q32 *****/
//SPI mSPI(SPI1, false, 0);
//W25QXX mW25QXX(mSPI, GPIOA, GPIO_Pin_4);
/*** VOLT MEASURE ***/
MADC mAdc(11,12);
/*** APP ***********/
//APP mApp(mPrint, mGPRS, mSHT20, mMeaCom, mUSBCom, mMonitor, mWifi, mAdc);
APP mApp;

void Print();
void MonitorTest();
void W25Qxx_Init();

int main(void)
{
//	float voltage = 0, power = 0;
	double curTime = 0, oldTime = 0;
	uint16_t loop500Hzcnt, loop100HzCnt, loop20HzCnt = 0, loop1per5Cnt = 0;

	com1 << "main come in\r\n";
	Vol_EN.SetLevel(1);
	com1 << "main come out\r\n";
//	mApp.GPRS_Init();
//	mApp.Wifi_Init();
//	mApp.SH2X_Init();
	
	while (1)
	{
		mApp.UpdateMonitor();
		curTime = tskmgr.Time();
		double deltaT = curTime - oldTime;
		if (deltaT >= 0.002)//500HZ 控制
		{
			oldTime = curTime;
			loop500Hzcnt++;
			if (loop500Hzcnt >= 5)//100HZ 控制
			{
				loop500Hzcnt = 0;
				loop100HzCnt++;
				if (loop100HzCnt >= 5)//20HZ
				{
					com1 << "kick\r\n";
					loop100HzCnt = 0;
					loop20HzCnt++;
					mAdc.GetBattle(11);			//更新电压
					if (loop20HzCnt >= 5)//4HZ
					{
						mApp.CheckStatus();
						loop20HzCnt = 0;
						loop1per5Cnt++;
						if(loop1per5Cnt >= 20)
						{
							loop1per5Cnt=0;
//							com1<<mWifi.GetCCSQ("InfiniteYuan");
							mApp.Update();
							mApp.UpdateDataQueue();
						}
					}
				}
			}
		}
	}
}

void W25Qxx_Init()
{
//	TraNum mtest[2];
//	TraNum test;
//	strcpy(mtest[0].TrainNumL,"K1234");
//	strcpy(mtest[1].TrainNumL,"K5678");
//	u8 TEXT_Buffer[] = { "Infinite STM32F4 SPI TEST" };
//	u32 FLASH_SIZE;
//	u8 datatemp[sizeof(TEXT_Buffer)];
//	FLASH_SIZE = 4 * 1024 * 1024;	//FLASH 大小为4M字节
//	SPI mSPI(SPI1, false, 0);
//	W25QXX mW25QXX(mSPI, GPIOA, GPIO_Pin_4);
//	mW25QXX.W25QXX_Write(TEXT_Buffer, FLASH_SIZE-1000, sizeof(TEXT_Buffer));		//从倒数第100个地址处开始,写入SIZE长度的数据 FLASH_SIZE - 100
//	mW25QXX.W25QXX_Read(datatemp, FLASH_SIZE-1000, sizeof(TEXT_Buffer));						//从倒数第100个地址处开始,读出SIZE个字节
//	com1 << datatemp;
}

void MonitorTest()
{
//	mMonitor.RedirectMain();
//	mMonitor.SetChPeoPle(PeoPleCh1, (char *)"袁明富\0");
//	mMonitor.SetChPeoPle(PeoPleCh2, (char *)"但元宏\0");
//	mMonitor.SetChPeoPle(PeoPleCh3, (char *)"刘志有\0");
//	mMonitor.SetChMeasure(CH_D600P, (char *)"121\0");
//	mMonitor.SetChMeasure(CH_D600N, (char *)"121\0");
//	mMonitor.SetChMeasure(CH_D110P, (char *)"121\0");
//	mMonitor.SetChMeasure(CH_D110N, (char *)"121\0");
//	mMonitor.SetChMeasure(CH_DPN, (char *)"121\0");
//	mMonitor.SetChTrain(CHTRAIN1, (char *)"G1234\0");
//	mMonitor.SetChTrain(CHTRAIN2, (char *)"G2212\0");
//	mMonitor.SetChTrain(CHTRAIN3, (char *)"G3421\0");
//	mMonitor.SetChTrain(CHTRAIN4, (char *)"G2332\0");
//	mMonitor.SetMainTrain((char *)"K1234\0");
//	mMonitor.SetTemperature((char *)"26\0");
//	mMonitor.SetHumidity((char *)"26\0");
//	mMonitor.SetWifiSignal(3);
//	mMonitor.SetGprsSignal(3);
//	mMonitor.SetBattery(0);
}

void Print()
{
//	mPrint.InitializePrint();
//	mPrint.PrintPageStart(0, 0, 384, 640, 0);
//	mPrint.rectangleDraw(0, 0, 384, 576, 5, 1);

//	mPrint.lineDraw(96, 96, 384, 96, 5, 1);
//	mPrint.lineDraw(0, 192, 384, 192, 5, 1);
//	mPrint.lineDraw(0, 288, 384, 288, 5, 1);
//	mPrint.lineDraw(96, 384, 384, 384, 5, 1);
//	mPrint.lineDraw(96, 480, 384, 480, 5, 1);
//	mPrint.lineDraw(96, 0, 96, 576, 5, 1);
//	mPrint.lineDraw(192, 0, 192, 576, 5, 1);
//	mPrint.lineDraw(288, 0, 288, 576, 5, 1);

//	mPrint.TextDraw(357, 5, 32, 8465, (char *)" 车 次\0");
//	mPrint.TextDraw(357, 117, 32, 8465, (char *)"K9432\0");
//	mPrint.TextDraw(357, 197, 32, 8465, (char *)" 时 间");
//	mPrint.TextDraw(357, 309, 32, 8465, (char *)"12-28");
//	mPrint.TextDraw(357, 389, 32, 8465, (char *)" 湿 度");
//	mPrint.TextDraw(357, 501, 32, 8465, (char *)"100%");

//	mPrint.TextDraw(272, 21, 32, 4369, (char *)"D600+");
//	mPrint.TextDraw(272, 117, 32, 4369, (char *)"D600-");
//	mPrint.TextDraw(272, 218, 32, 4369, (char *)"D+/");
//	mPrint.TextDraw(272, 309, 32, 4369, (char *)"D110+");
//	mPrint.TextDraw(272, 405, 32, 4369, (char *)"D110-");

//	mPrint.TextDraw(240, 32, 32, 4369, (char *)"/地");
//	mPrint.TextDraw(240, 128, 32, 4369, (char *)"/地");
//	mPrint.TextDraw(240, 224, 32, 4369, (char *)"D-");
//	mPrint.TextDraw(240, 320, 32, 4369, (char *)"/地");
//	mPrint.TextDraw(240, 416, 32, 4369, (char *)"/地");

//	mPrint.TextDraw(168, 26, 32, 8465, (char *)"66.6");
//	mPrint.TextDraw(168, 122, 32, 8465, (char *)"66.6");
//	mPrint.TextDraw(168, 218, 32, 8465, (char *)"66.6");
//	mPrint.TextDraw(168, 314, 32, 8465, (char *)"66.6");
//	mPrint.TextDraw(168, 410, 32, 8465, (char *)"66.6");
//	mPrint.TextDraw(168, 506, 32, 8465, (char *)"66.6");

//	mPrint.TextDraw(80, 5, 32, 8465, (char *)" 单 位");
//	mPrint.TextDraw(80, 101, 32, 8465, (char *)"(兆欧)");
//	mPrint.TextDraw(80, 202, 32, 8465, (char *)"测试人");
//	mPrint.TextDraw(80, 298, 32, 8465, (char *)"刘志有、袁明富、王万军");

//	mPrint.PrintPageEnd();
//	mPrint.PagePrint(0x01);
}
