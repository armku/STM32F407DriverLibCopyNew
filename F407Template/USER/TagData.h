#ifndef __TAGDATA_H
#define __TAGDATA_H

#include "Protocol.h"
#include "stm32f4xx.h"

class TagDataQue{

	private:
		const u8 maxsize;
		TagData *mTagData;
	
		u8 indexHead;
		u8 indexPrintEnd;
		u8 indexSendEnd;
		u8 indexSaveEnd;
	
		u8 printCount;
		u8 saveCount;
		u8 sendCount;
	
		u8 overFlowCount;
	
	public:
		TagDataQue(u8 maxLength = 50):maxsize(maxLength){
		indexHead=0;
		indexPrintEnd=0;
		indexSendEnd=0;
		indexSaveEnd=0;
		printCount=0;
		saveCount=0;
		sendCount=0;
		overFlowCount=0;
		}
		
		bool AddTag(TagData data){
			if(printCount >= maxsize||sendCount >= maxsize||saveCount >= maxsize){
				overFlowCount++;
				return false;
			}
			indexHead++;
			if(indexHead >= maxsize){
				indexHead = 0;
			}
			
		}
};


#endif
