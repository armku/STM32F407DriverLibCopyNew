#include "APP.h"

/* 从W25Q32中读取数据（W25Q32ADD 的值，以及配置信息）
		读取车次，工号-姓名链	*/
//APP::APP(DPPrint &print ,GPRS &mGP,SHT2X &mSHT,UidLink &mMea,UidLink &mUSB,Monitor &mMonit,Socket_esp8266 &mSocket,MADC &mAD)
//:mPrint(print), mGPRS(mGP),mSHT20(mSHT),mMeaCom(mMea),mUSBCom(mUSB),mMonitor(mMonit),mWifi(mSocket),mADC(mAD)
APP::APP()
{
	/******/
//	mSetData.PEOPLENUM = 7;
//	mSetData.TRAINNUM = 14;
//	mSetData.TAGNUM = 0;
//	mSetData.USEWIFIORGPRS = USEGPRS;
//	strcpy(mSetData.IPADDR, "192.168.191.1");
//	strcpy(mSetData.IPPORT, "8090");
//	strcpy(mSetData.WIFISSID, "InfiniteYuan");
//	strcpy(mSetData.WIFIKEY, "741852963");
//	WriteSetData();
//	strcpy(mTraNum[0].TrainNumL, "K1231");
//	strcpy(mTraNum[1].TrainNumL, "K2322");
//	strcpy(mTraNum[2].TrainNumL, "K2343");
//	strcpy(mTraNum[3].TrainNumL, "K2344");
//	strcpy(mTraNum[4].TrainNumL, "G2345");
//	strcpy(mTraNum[5].TrainNumL, "G2346");
//	strcpy(mTraNum[6].TrainNumL, "G2357");
//	strcpy(mTraNum[7].TrainNumL, "G2348");
//	strcpy(mTraNum[8].TrainNumL, "D2359");
//	strcpy(mTraNum[9].TrainNumL, "D2310");
//	strcpy(mTraNum[10].TrainNumL, "D2311");
//	strcpy(mTraNum[11].TrainNumL, "D2312");
//	strcpy(mTraNum[12].TrainNumL, "D2313");
//	strcpy(mTraNum[13].TrainNumL, "D2314");
//	WriteTrain();
//	strcpy(mNameJob[0].JOBNUM, "1111");
//	strcpy(mNameJob[0].NAME, "袁明富");
//	strcpy(mNameJob[1].JOBNUM, "2222");
//	strcpy(mNameJob[1].NAME, "但元宏");
//	strcpy(mNameJob[2].JOBNUM, "3333");
//	strcpy(mNameJob[2].NAME, "刘志有");
//	strcpy(mNameJob[3].JOBNUM, "4444");
//	strcpy(mNameJob[3].NAME, "李文兵");
//	strcpy(mNameJob[4].JOBNUM, "5555");
//	strcpy(mNameJob[4].NAME, "王万军");
//	strcpy(mNameJob[5].JOBNUM, "6666");
//	strcpy(mNameJob[5].NAME, "向聪");
//	strcpy(mNameJob[6].JOBNUM, "7777");
//	strcpy(mNameJob[6].NAME, "张雯雯");
//	WriteNameJob();
	
//	mMonitor.RedirectInit();
	
	ReadSetData();		/***从Flash中读取兆欧表配置信息 ***/
	ReadTrain();
	ReadNameJob();		/** 从Flash中读取工号-姓名数据 ***/
	ReadTagStructer();
	mPageNum = mSetData.TRAINNUM / 4;
	if (mSetData.TRAINNUM % 4 != 0)
		mPageNum++;
	mMonitor.ReadDateTime();
	mMonitor.SetBattery(100);
	mMonitor.SetWifiSignal(3);
	mMonitor.SetGprsSignal(3);
	
	TagDataAdd = TAGDATAADD + mSetData.TAGNUM * sizeof(TagStructer);  //****地址从末端开始
	
	GPRS_Init();
	Wifi_Init();
	SH2X_Init();
	
	mMonitor.RedirectMain();
}

void APP::UpdateMonitor(void)/*** 屏幕交互与逻辑控制 ***/
{
	static u8 NowPage = 1;
	if (mMonitor.ReadCommand())
	{
		CommandData mCommand;
		mCommand = mMonitor.DecodeCommand();
		switch (mCommand.mBtnType)
		{
		case CH_D110PBTN://记录按键对应的测量项
			ChMeaSel = CH_D110P;
			break;
		case CH_D110NBTN:
			ChMeaSel = CH_D110N;
			break;
		case CH_D600PBTN:
			ChMeaSel = CH_D600P;
			break;
		case CH_D600NBTN:
			ChMeaSel = CH_D600N;
			break;
		case CH_DPNBTN:
			ChMeaSel = CH_DPN;
			break;
		case MEASUREBTN:
			char *meaData;
			mMonitor.RedirectMeasure();
			meaData = Measure();											//等待测量完成 GidLink
			strncpy(mTempDataStru.mTagData.CH_MEA[ChMeaSel], meaData, 5);
			mMonitor.RedirectMain();
			mMonitor.SetChMeasure(ChMeaSel, meaData);
			break;
		case PEOBTN1://记录第几个工号，在点击确认之后，存到指定的临时数据结构中
			ChPeoSel = PeoPleCh1;
			strncpy(mTempDataStru.mTagData.PeoPle[ChPeoSel], FindName(mCommand.data), 10);
			mMonitor.SetChPeoPle(PeoPleCh1, FindName(mCommand.data));
			break;
		case PEOBTN2:
			ChPeoSel = PeoPleCh2;
			strncpy(mTempDataStru.mTagData.PeoPle[ChPeoSel], FindName(mCommand.data), 10);
			mMonitor.SetChPeoPle(PeoPleCh2, FindName(mCommand.data));
			break;
		case PEOBTN3:
			ChPeoSel = PeoPleCh3;
			strncpy(mTempDataStru.mTagData.PeoPle[ChPeoSel], FindName(mCommand.data), 10);
			mMonitor.SetChPeoPle(PeoPleCh3, FindName(mCommand.data));
			break;
		case PEOSURE:
			strncpy(mTempDataStru.mTagData.PeoPle[ChPeoSel], FindName(mCommand.data), 10);//解析数据、填入
			break;
		case PRINTBTN:
			mMonitor.RedirectPrint();
			mMonitor.ReadDateTime();
			mMonitor.HidePrinting();
			mMonitor.EnableCancelBtn();
			mMonitor.SetPrintTrain(mTempDataStru.mTagData.TrainNum);
			mMonitor.SetPrintDate(mTempDataStru.mTagData.DateTIME);
			mMonitor.SetPrintHum(mTempDataStru.mTagData.HUMIDTY);
			mMonitor.SetPrintD600P(mTempDataStru.mTagData.CH_MEA[CH_D600P]);
			mMonitor.SetPrintD600N(mTempDataStru.mTagData.CH_MEA[CH_D600N]);
			mMonitor.SetPrintD110P(mTempDataStru.mTagData.CH_MEA[CH_D110P]);
			mMonitor.SetPrintD110N(mTempDataStru.mTagData.CH_MEA[CH_D110N]);
			mMonitor.SetPrintDPN(mTempDataStru.mTagData.CH_MEA[CH_DPN - 1]);
			mMonitor.SetPrintPeoPle(mTempDataStru.mTagData.PeoPle[PeoPleCh1], mTempDataStru.mTagData.PeoPle[PeoPleCh2], mTempDataStru.mTagData.PeoPle[PeoPleCh3]);
			break;
		case PRINTSUREBTN://把临时数据结构加入到队列头
			com1 << "print\r\n";
			mMonitor.ComePrinting();
			mMonitor.DisableCancelBtn();
			mTempDataStru.PrintFlag = NOTPRINT;
			mTempDataStru.SendFlag = NOTSEND;
			mTempDataStru.SaveFlag = NOTSAVE;
			sprintf(mTempDataStru.mTagData.HUMIDTY, "%0.0f", mSHT20.mSHT20data.HUMI_POLL);
			mTagDataQueue.Put(mTempDataStru);						//添加到队列中来
			UpdateDataQueue();											/***** 打印标签 ********/
			com1 << "printing\r\n";
			tskmgr.DelayMs(100);
			mMonitor.HidePrinting();
			mMonitor.EnableCancelBtn();
			mMonitor.RedirectMain();
			ClearTempDataStru();
		
			if((u8)mSetData.TAGNUM++ > TAGMAXNUM)		/*** 计数器 保存数据到Flash ***/
			{
				TagDataAdd = TAGDATAADD;
			}
			mSetData.TAGNUM = mSetData.TAGNUM % TAGMAXNUM;
			WriteSetData();
			com1 << "printed\r\n";
			break;
		case TRAINSELECT:	 //发送一页车次链到显示屏，供用户选择
			NowPage = 1;
			mMonitor.RedirectTrainSelect();
			mMonitor.SendResidueTrain(NowPage, mPageNum);
			RefreshTrain(NowPage);
			break;
		case NEXTTRAINPAGE://发送下一页车次链到显示屏，供用户选择
			NowPage++;
			if ((NowPage > 0) && (NowPage <= mPageNum))
				RefreshTrain(NowPage);
			else
				NowPage--;
			mMonitor.SendResidueTrain(NowPage, mPageNum);
			break;
		case LASTTRAINPAGE://发送上一页车次链到显示屏，供用户选择
			NowPage--;
			if ((NowPage > 0) && (NowPage <= mPageNum))
				RefreshTrain(NowPage);
			else
				NowPage++;
			mMonitor.SendResidueTrain(NowPage, mPageNum);
			break;
		case TRAINSELECTED:
			strncpy(mTempDataStru.mTagData.TrainNum, mTraNum[0].TrainNumL, 10);
			break;
		case CH_TRAIN1://存到临时数据结构中
			strncpy(mTempDataStru.mTagData.TrainNum, mTempTra[0].TrainNumL, 10);
			mMonitor.RedirectMain();
			mMonitor.SetMainTrain(mTempDataStru.mTagData.TrainNum);
			break;
		case CH_TRAIN2:
			strncpy(mTempDataStru.mTagData.TrainNum, mTempTra[1].TrainNumL, 10);
			mMonitor.RedirectMain();
			mMonitor.SetMainTrain(mTempDataStru.mTagData.TrainNum);
			break;
		case CH_TRAIN3:
			strncpy(mTempDataStru.mTagData.TrainNum, mTempTra[2].TrainNumL, 10);
			mMonitor.RedirectMain();
			mMonitor.SetMainTrain(mTempDataStru.mTagData.TrainNum);
			break;
		case CH_TRAIN4:
			strncpy(mTempDataStru.mTagData.TrainNum, mTempTra[3].TrainNumL, 10);
			mMonitor.RedirectMain();
			mMonitor.SetMainTrain(mTempDataStru.mTagData.TrainNum);
			break;
		case CLEARBTN:
			ClearTempDataStru();//清除临时数据
			break;
		case GPRSSET://GPRS设置
			mSetData.USEWIFIORGPRS = USEGPRS;
			mMonitor.RedirectMain();
			WriteSetData();
			break;
		case WIFISET:
			mSetData.USEWIFIORGPRS = USEWIFI;
			mMonitor.RedirectMain();
			WriteSetData();
			break;
		case WIFISSIDTEXT://把SSID 密码 IP 端口 存到W25Q32中
			strncpy(mSetData.WIFISSID, mCommand.data, 30);
			WriteSetData();
			break;
		case WIFIKEYTEXT:
			strncpy(mSetData.WIFIKEY, mCommand.data, 30);
			WriteSetData();
			break;
		case IPADDRTEXT:
			strncpy(mSetData.IPADDR, mCommand.data, 20);
			WriteSetData();
			break;
		case IPPORTTEXT:
			strncpy(mSetData.IPPORT, mCommand.data, 10);
			WriteSetData();
			break;
		case READDATETIME:
			strcpy(mTempDataStru.mTagData.DateTIME, mCommand.data);
			break;
		case WIFISETTING:
		case CONFIGBTN:
			mMonitor.SetWiFiSSID((char *)mSetData.WIFISSID);
			mMonitor.SetWiFiKEY((char *)mSetData.WIFIKEY);
			mMonitor.SetWiFiIP((char *)mSetData.IPADDR);
			mMonitor.SetWiFiIPPort((char *)mSetData.IPPORT);
			break;
		case GPRSSETTING:
			mMonitor.SetGPRSIP((char *)mSetData.IPADDR);
			mMonitor.SetGPRSIPPort((char *)mSetData.IPPORT);
			break;
		case NULLBTN:
//			Update();
//			UpdateDataQueue();
			break;
		default:
			break;
		}
	}
}

void APP::Update()
{
	char temp[20];
	mSHT20.Update();
	sprintf(temp, "%.0f", mSHT20.mSHT20data.TEMP_POLL);
	sprintf(mTempDataStru.mTagData.HUMIDTY, "%.0f", mSHT20.mSHT20data.HUMI_POLL);
	mMonitor.SetHumidity(mTempDataStru.mTagData.HUMIDTY);
	mMonitor.SetTemperature(temp);
	mMonitor.SetGprsSignal(mGPRS.GetCCSQ() / 6);
//	mMonitor.SetWifiSignal(4-(mWifi.GetCCSQ(mSetData.WIFISSID)-30)/10);
	mMonitor.SetBattery(mAdc.GetBattle(11));
}

void APP::CheckStatus(void)
{
	u8 count = 0;
	static bool isUSB = false;
	if(!mGPRS.GetConnectStatus())
	{
		mMonitor.SetWifiDataLogo(false);
		mGPRS.init();
		mGPRS.Close();
		while (!mGPRS.Connect((char*)"s169j08562.iask.in", 21843, Socket_Type_Stream, Socket_Protocol_IPV4) && (count++ < 4));
		count = 0;
	} else {
		mMonitor.SetWifiDataLogo(true);
	}
	if(!mWifi.GetConnectStatus())
	{
		mMonitor.SetGPRSDataLogo(false);
		while (!mWifi.Kick());//检查连接
		mWifi.SetEcho(false);//关闭回响
		mWifi.SetMode(esp8266::esp8266_MODE_STATION, esp8266::esp8266_PATTERN_NULL);//设置station+ap模式
		mWifi.SetMUX(false);//单链接模式
		mWifi.QuieAP();
		while (!mWifi.JoinAP(mSetData.WIFISSID, mSetData.WIFIKEY, esp8266::esp8266_PATTERN_NULL)&&(count++<4));//加入AP
		count = 0;
		mWifi.Close();
		while (!mWifi.Connect(mSetData.IPADDR, atoi(mSetData.IPPORT), Socket_Type_Stream, Socket_Protocol_IPV4)&&(count++<4));
		count = 0;
	} else {
		mMonitor.SetGPRSDataLogo(true);
	}
	isUSB = !isUSB;
	mMonitor.SetUSBLogo(isUSB);
}

void APP::UpdateDataQueue(void)/*** 数据队列维护 ***/
{
	TagStructer mDealTempDataStru;
	u16 num = 0, TagDataQueueNum = mTagDataQueue.Size();
	u8 count = 0;
	for (num = 0; num < TagDataQueueNum; num++)
	{
		if (mTagDataQueue.Get(mDealTempDataStru))
		{
			if (mDealTempDataStru.PrintFlag == NOTPRINT)//未打印
			{
				/******			*******/
				mPrint.TagPrint(mDealTempDataStru.mTagData);
				mDealTempDataStru.PrintFlag = PRINTED;
				mDealTempDataStru.ALLStatus++;
			}
			if (mDealTempDataStru.SendFlag == NOTSEND)//未发送
			{
				if (mSetData.USEWIFIORGPRS == USEWIFI)
				{
					//WIFI链接
					//mWifi.Connect();
					count = 0;
					while (count++ < 20)
					{
						if (mWifi.SendTagData(mDealTempDataStru.mTagData.TrainNum, sizeof(TagData)))
						{
							mDealTempDataStru.SendFlag = SENDED;
							mDealTempDataStru.ALLStatus++;
							break;
						}
					}
				}
				else if (mSetData.USEWIFIORGPRS == USEGPRS)
				{
					//GPRS链接
					//mGPRS.Connect();
					count = 0;
					while (count++ < 20)
					{
						if (mGPRS.SendTagData(mDealTempDataStru.mTagData.TrainNum, sizeof(TagData)))
						{
							mDealTempDataStru.SendFlag = SENDED;
							mDealTempDataStru.ALLStatus++;
							break;
						}
					}
				}
			}
			if (mDealTempDataStru.SaveFlag == NOTSAVE)//未保存
			{
				/**			*/
				SPI mSPI(SPI1, false, 0);
				W25QXX mW25QXX(mSPI, GPIOA, GPIO_Pin_4);
				mW25QXX.W25QXX_Write((u8 *)mDealTempDataStru.mTagData.TrainNum, TagDataAdd, sizeof(TagStructer));  /** 记录状态，一并保存，开机再次发送， **/
				TagDataAdd += sizeof(TagStructer);
				mDealTempDataStru.SaveFlag = SAVEED;
				mDealTempDataStru.ALLStatus++;
			}
			if (mDealTempDataStru.ALLStatus == 3)//发送完成、打印完成、存储完成、可以出队列
			{
				;
//				TagStructer mTemp;
//				mTagDataQueue.Get(mTemp);//出队列
			}
			else
				mTagDataQueue.Put(mDealTempDataStru);
		}
	}
}

void APP::CheckUSBFrame(void)
{
//	dataConvert mDataConvert;
	u16 i = 0;
	if(mUSBCom.CheckFrame())
	{
		switch (mUSBCom.rxFrame.fnCode)
		{
			case FC_READ_TRAIN:  /*** 发送所有列车信息 ***/
				for(i=0; i < mSetData.TRAINNUM; i++)
				{
					mUSBCom.txFrame.fnCode = FC_READ_TRAIN;
					mUSBCom.txFrame.data[0] = 1;
					mUSBCom.txFrame.data[1] = 1;
					mUSBCom.txFrame.data[2] = 1;
					mUSBCom.txFrame.isUpdated = true;
					mUSBCom.Send();
				}
				break;
			case FC_READ_PEOPLE:	/*** 发送所有人员-工号 ***/
				for(i=0; i < mSetData.TRAINNUM; i++)
				{
					mUSBCom.txFrame.fnCode = FC_READ_PEOPLE;
					mUSBCom.txFrame.data[0] = 1;
					mUSBCom.txFrame.data[1] = 1;
					mUSBCom.txFrame.data[2] = 1;
					mUSBCom.txFrame.isUpdated = true;
					mUSBCom.Send();
				}
				break;
			case FC_READ_TAGDATA:	/*** 发送所有标签数据 ***/
				for(i=0; i < mSetData.TRAINNUM; i++)
				{
					mUSBCom.txFrame.fnCode = FC_READ_TAGDATA;
					mUSBCom.txFrame.data[0] = 1;
					mUSBCom.txFrame.data[1] = 1;
					mUSBCom.txFrame.data[2] = 1;
					mUSBCom.txFrame.isUpdated = true;
					mUSBCom.Send();
				}
				break;
		}
	}
}

char * APP::Measure(void)/*** 测量 ***/
{
	u8 startCommand[]={0x01, 0x06, 0x00, 0x6F, 0x32, 0x10, 0xAD, 0x7B};
	u8 readResult[]={0x01 , 0x03 , 0x00 , 0x84 , 0x00 , 0x01 , 0xC4 , 0x23};
	u8 temp[20] = {0};
	char mMeaResult[10] = {0};
	double oldTime = 0; //curTime = 0,
	
	oldTime = tskmgr.Time();
	com2.SendBytes(startCommand, 8);
	while(!(com2.RxSize() > 7) && ((tskmgr.Time() - oldTime) < 2));
	if(com2.RxSize() > 7)
	{
		com2.GetBytes(temp, 8);
		if((temp[0] == startCommand[0])&&(temp[1] == startCommand[1])&&(temp[2] == startCommand[2])&&(temp[3] == startCommand[3])&&(temp[4] == startCommand[4])&&(temp[5] == startCommand[5]))
		{
			tskmgr.DelayS(3);
			com2.SendBytes(readResult, 8);
			while(!(com2.RxSize() > 6) && ((tskmgr.Time() - oldTime) < 2));
			if(com2.RxSize() > 6)
			{
				com2.GetBytes(temp, 7);
				sprintf(mMeaResult, "%.2f", (temp[3]*256+temp[4])/100.0);
			}
		}
	}
	return mMeaResult;
}

void APP::ReadSetData(void)/*** 读取配置数据到结构体中 ***/
{
	u32 ADD = SETDATAADD;
	SPI mSPI(SPI1, false, 0);
	W25QXX mW25QXX(mSPI, GPIOA, GPIO_Pin_4);
	mW25QXX.W25QXX_Read((u8 *)&mSetData.USEWIFIORGPRS, ADD, sizeof(SetDataStruct));
}

void APP::WriteSetData(void)/*** 写配置数据到Flash中 ***/
{
	u32 ADD = SETDATAADD;
	SPI mSPI(SPI1, false, 0);
	W25QXX mW25QXX(mSPI, GPIOA, GPIO_Pin_4);
	mW25QXX.W25QXX_Write((u8 *)&mSetData.USEWIFIORGPRS, ADD, sizeof(SetDataStruct));
}

void APP::ReadNameJob(void)/*** 读取工号-姓名数据	***/
{
	u8 i = 0;
	u32 NameAdd = NAMEADD;
	SPI mSPI(SPI1, false, 0);
	W25QXX mW25QXX(mSPI, GPIOA, GPIO_Pin_4);
	for (i = 0; i < mSetData.PEOPLENUM; i++)
	{
		mW25QXX.W25QXX_Read((u8 *)mNameJob[i].NAME, NameAdd, sizeof(NameJob));
		NameAdd += sizeof(NameJob);
	}
}

void APP::WriteNameJob(void)/*** 写工号-姓名数据	***/
{
	u8 i = 0;
	u32 NameAdd = NAMEADD;
	SPI mSPI(SPI1, false, 0);
	W25QXX mW25QXX(mSPI, GPIOA, GPIO_Pin_4);
	for (i = 0; i < mSetData.PEOPLENUM; i++)
	{
		mW25QXX.W25QXX_Write((u8 *)(mNameJob[i].NAME), NameAdd, sizeof(NameJob));
		NameAdd += sizeof(NameJob);
	}
}

void APP::ReadTrain(void)/*** 读取车次列表数据 ***/
{
	u8 i = 0;
	u32 TrainAdd = TRAINADD;
	SPI mSPI(SPI1, false, 0);
	W25QXX mW25QXX(mSPI, GPIOA, GPIO_Pin_4);
	for (i = 0; i < mSetData.TRAINNUM; i++)
	{
		mW25QXX.W25QXX_Read((u8 *)(mTraNum[i].TrainNumL), TrainAdd, sizeof(TraNum));		/*** 读取数据 ***/
		TrainAdd += sizeof(TraNum);
	}
}

void APP::WriteTrain(void)/*** 写车次列表数据 ***/
{
	u8 i = 0;
	u32 TrainAdd = TRAINADD;
	SPI mSPI(SPI1, false, 0);
	W25QXX mW25QXX(mSPI, GPIOA, GPIO_Pin_4);
	for (i = 0; i < mSetData.TRAINNUM; i++)
	{
		mW25QXX.W25QXX_Write((u8 *)(mTraNum[i].TrainNumL), TrainAdd, sizeof(TraNum));		/*** 读取数据 ***/
		TrainAdd += sizeof(TraNum);
	}
}

void APP::ReadTagStructer(void)
{
	u8 i = 0;
	u32 TagDataAdd = TAGDATAADD;
	TagStructer mTemp;
	SPI mSPI(SPI1, false, 0);
	W25QXX mW25QXX(mSPI, GPIOA, GPIO_Pin_4);
	for (i = 0; i < mSetData.TAGNUM; i++)
	{
		mW25QXX.W25QXX_Read((u8 *)(mTemp.mTagData.TrainNum), TagDataAdd, sizeof(TagStructer));		/*** 读取数据 ***/
		TagDataAdd += sizeof(TagStructer);
		mTagDataQueue.Put(mTemp);
	}
}

char * APP::FindName(char *JobNum)/*** 在工号-姓名中查找工号对应的姓名 ***/
{
	u8 i = 0;
	for (i = 0; i < mSetData.PEOPLENUM; i++)
	{
		if (strcmp(mNameJob[i].JOBNUM, JobNum) == 0)
		{
			return mNameJob[i].NAME;
		}
	}
	return ((char *)" ");
}

void APP::RefreshTrain(u8 PageNum)
{
	u8 i = 0;
	if (PageNum != mPageNum)
	{
		for (i = 0; i < 4; i++)
			strcpy(mTempTra[i].TrainNumL, mTraNum[(PageNum * 4 - 4 + i)].TrainNumL);
	}
	else
	{
		for (i = 0; i < (mSetData.TRAINNUM % 4); i++)
		{
			strcpy(mTempTra[i].TrainNumL, mTraNum[(PageNum * 4 - 4 + i)].TrainNumL);
		}
		for (i = 0; i < (4 - (mSetData.TRAINNUM % 4)); i++)
		{
			strcpy(mTempTra[(i + (mSetData.TRAINNUM % 4))].TrainNumL, " ");
		}
	}
	for (i = 0; i < 4; i++)
	{
		mMonitor.SetChTrain(i + 1, mTempTra[i].TrainNumL);
	}
}

void APP::ClearTempDataStru(void)/*** 清除测量数据 ***/
{
	char data[10] = { 0 };
	mTempDataStru.ALLStatus = 0;
	mTempDataStru.PrintFlag = 0;
	mTempDataStru.SaveFlag = 0;
	mTempDataStru.SendFlag = 0;
	//	strncpy(mTempDataStru.mTagData.TrainNum, data, 10);
	//	strncpy(mTempDataStru.mTagData.DateTIME, data, 10);
	//	strncpy(mTempDataStru.mTagData.HUMIDTY, data, 5);
	strncpy(mTempDataStru.mTagData.CH_MEA[CH_D600P], data, 5);
	strncpy(mTempDataStru.mTagData.CH_MEA[CH_D600N], data, 5);
	strncpy(mTempDataStru.mTagData.CH_MEA[CH_D110P], data, 5);
	strncpy(mTempDataStru.mTagData.CH_MEA[CH_D110N], data, 5);
	strncpy(mTempDataStru.mTagData.CH_MEA[CH_DPN - 1], data, 5);
	//	strncpy(mTempDataStru.mTagData.PeoPle[PeoPleCh1], data, 10);
	//	strncpy(mTempDataStru.mTagData.PeoPle[PeoPleCh2], data, 10);
	//	strncpy(mTempDataStru.mTagData.PeoPle[PeoPleCh3], data, 10);
	//	strncpy(mTempDataStru.mTagData.CH_D600P, data, 5);
	//	strncpy(mTempDataStru.mTagData.CH_D600N, data, 5);
	//	strncpy(mTempDataStru.mTagData.CH_DPN, data, 5);
	//	strncpy(mTempDataStru.mTagData.CH_D110P, data, 5);
	//	strncpy(mTempDataStru.mTagData.CH_D110N, data, 5);
	//	strncpy(mTempDataStru.mTagData.PeoPle1, data, 10);
	//	strncpy(mTempDataStru.mTagData.PeoPle2, data, 10);
	//	strncpy(mTempDataStru.mTagData.PeoPle3, data, 10);
}

void APP::GPRS_Init(void)
{
	u8 count = 0;
	mGPRS.init();
	com1 << "GPRS init complete" << "\r\n";
	mGPRS.Close();
	while (!mGPRS.Connect((char*)"s169j08562.iask.in", 21843, Socket_Type_Stream, Socket_Protocol_IPV4) && (count++ < 4));
	count = 0;
	while (!mGPRS.Write((char*)"GPRSTEST", 8) && (count++ < 4));
	com1 << "test\r\n";
}

void APP::Wifi_Init(void)	//WIFI initialize
{
	u8 count = 0;
//	char mApSetName[20] = "InfiniteYuan520";
//	char mApSetPasswd[18] = "741852963";
//	char mApJoinName[15] = "InfiniteYuan";
//	char mApJoinPasswd[18] = "741852963";
	while (!mWifi.Kick());//检查连接
	com1 << "wifi is healthy\n";
	mWifi.SetEcho(false);//关闭回响
	mWifi.SetMode(esp8266::esp8266_MODE_STATION, esp8266::esp8266_PATTERN_NULL);//设置station+ap模式
	mWifi.SetMUX(false);//单链接模式
//	mWifi.SetApParam(mApSetName, mApSetPasswd, 1, esp8266::esp8266_ENCRYPTION_WPA2_PSK, esp8266::esp8266_PATTERN_NULL);
	mWifi.QuieAP();
	com1 << mSetData.WIFISSID << "\r\n";
	while (!mWifi.JoinAP(mSetData.WIFISSID, mSetData.WIFIKEY, esp8266::esp8266_PATTERN_NULL)&&(count++<4));//加入AP
	com1 << mSetData.WIFIKEY << "\r\n";
	count = 0;
	mWifi.Close();
	while (!mWifi.Connect(mSetData.IPADDR, atoi(mSetData.IPPORT), Socket_Type_Stream, Socket_Protocol_IPV4)&&(count++<4));
	com1 << "wifi init complete\n";
}

void APP::SH2X_Init()
{
	//com1 << "SHT20 come in" << "\r\n";
	mSHT20.SHT2x_Init();
//	com1 << "SHT20 come out" << "\r\n";
}
