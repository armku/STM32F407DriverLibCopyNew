#ifndef _IAP_H

#define _IAP_H
#include "stm32f4xx.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
//�û������Լ�����Ҫ����
#define STM32_FLASH_SIZE 64 	 					//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1              //ʹ��FLASHд��(0��������;1��ʹ��)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
//FLASH������ֵ

//////////////////////////////////////////////////////////////////////////////////	 
extern "C"{
	typedef  void(*iapfun)(void);				//����һ���������͵Ĳ���.
}

#define FLASH_APP1_ADDR		0x08008000  	//��һ��Ӧ�ó�����ʼ��ַ(�����FLASH)
//����0X08000000~0X08000FFF�Ŀռ�ΪIAPʹ��

#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //�ֽ�
#else 
#define STM_SECTOR_SIZE	2048
#endif

#define USART_REC_LEN  		10*1024 //�����������ֽ��� 15K 
extern u8 APP_USART_RX_BUF[USART_REC_LEN];

class IAP_BootLoader{

private:
	u16 STMFLASH_BUF[STM_SECTOR_SIZE / 2];//�����2K�ֽ�

public:
	u32 RecvIndex;
	bool Update_flag;
	IAP_BootLoader(u8 i)
	{
		Update_flag = false;
		RecvIndex = 0;
	}
	void Put_APP(u8 *data, u8 length);

	void iap_load_app(u32 appxaddr);															//ִ��flash�����app����
	void iap_load_appsram(u32 appxaddr);													//ִ��sram�����app����
	void iap_write_appbin(u32 appxaddr, u8 *appbuf, u32 appsize);	//��ָ����ַ��ʼ,д��bin

	u16 STMFLASH_ReadHalfWord(u32 faddr);		  //��������  
	void STMFLASH_WriteLenByte(u32 WriteAddr, u32 DataToWrite, u16 Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
	u32 STMFLASH_ReadLenByte(u32 ReadAddr, u16 Len);											//ָ����ַ��ʼ��ȡָ����������
	void STMFLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite);			//��ָ����ַ��ʼд��ָ�����ȵ�����
	void STMFLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead);   			//��ָ����ַ��ʼ����ָ�����ȵ�����

};
#endif
