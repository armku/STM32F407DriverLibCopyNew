#include "IAP.h"

//u8 APP_USART_RX_BUF[USART_REC_LEN] __attribute__ ((at(0X20001000)));//���ջ���,���USART_REC_LEN���ֽ�,��ʼ��ַΪ0X20001000.  
extern "C"
{
	void WFI_SET(void)
	{
		__ASM volatile("wfi");
	}
	//�ر������ж�
	void INTX_DISABLE(void)
	{
		__ASM volatile("cpsid i");
	}
	//���������ж�
	void INTX_ENABLE(void)
	{
		__ASM volatile("cpsie i");
	}
	//����ջ����ַ
	//addr:ջ����ַ
	__asm void MSR_MSP(u32 addr)
	{
		MSR MSP, r0 			//set Main Stack value
			BX r14
	}
};

void IAP_BootLoader::Put_APP(u8 *data, u8 length)
{
	u8 i;
	static u8 res;
	static u16 temp[1];
	static u8 flag = 0;
	static char Last_data;
	static u32 addr = FLASH_APP1_ADDR;
	Update_flag = true;
	for (i = 0; i < length; i++)
	{
		//		APP_USART_RX_BUF[RecvIndex++] = data[i];
		if (RecvIndex++ == 146){
			RecvIndex = RecvIndex;
		}
		res = data[i + 2];
		if (flag == 0){
			flag = 1;
			Last_data = res;
			temp[0] = 0;
		}
		else{
			flag = 0;
			temp[0] = (u16)res << 8;
			temp[0] += (u16)Last_data;
			STMFLASH_Write(addr, temp, 1);
			//			FLASH_ProgramHalfWord(addr,temp[0]);
			addr += 0x02;
		}
	}
}

extern "C"
{
	void temp(u32 appxaddr)
	{
		iapfun jump2app;
		if (((*(vu32*)appxaddr) & 0x2FFE0000) == 0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
		{
			jump2app = (iapfun)*(vu32*)(appxaddr + 4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
			MSR_MSP(*(vu32*)appxaddr);										//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
			jump2app();																		//��ת��APP.
		}
	}
}
void IAP_BootLoader::iap_load_app(u32 appxaddr)															//ִ��flash�����app����
{
	temp(appxaddr);
}

void IAP_BootLoader::iap_load_appsram(u32 appxaddr)													//ִ��sram�����app����
{
	/////////////////////////////////////////////////////
}

void IAP_BootLoader::iap_write_appbin(u32 appxaddr, u8 *appbuf, u32 appsize)	//��ָ����ַ��ʼ,д��bin
{
	u16 t;
	u16 iapbuf[1024];
	u16 i = 0;
	u16 temp;
	u32 fwaddr = appxaddr;//��ǰд��ĵ�ַ
	u8 *dfu = appbuf;
	for (t = 0; t < appsize; t += 2)
	{
		temp = (u16)dfu[1] << 8;
		temp += (u16)dfu[0];
		dfu += 2;//ƫ��2���ֽ�
		iapbuf[i++] = temp;
		if (i == 1024)
		{
			i = 0;
			STMFLASH_Write(fwaddr, iapbuf, 1024);
			fwaddr += 2048;//ƫ��2048  16=2*8.����Ҫ����2.
		}
	}
	if (i)
	{
		STMFLASH_Write(fwaddr, iapbuf, i);//������һЩ�����ֽ�д��ȥ.
	}
}

u16 IAP_BootLoader::STMFLASH_ReadHalfWord(u32 faddr)		  //��������  
{
	return *(vu16*)faddr;
}

void IAP_BootLoader::STMFLASH_WriteLenByte(u32 WriteAddr, u32 DataToWrite, u16 Len)	//ָ����ַ��ʼд��ָ�����ȵ�����
{

}

u32 IAP_BootLoader::STMFLASH_ReadLenByte(u32 ReadAddr, u16 Len)											//ָ����ַ��ʼ��ȡָ����������
{
	return 0;
}
#if STM32_FLASH_WREN	//���ʹ����д   
//������д��
//WriteAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��   
void STMFLASH_Write_NoCheck(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)
{
	u16 i;
	for (i = 0; i < NumToWrite; i++)
	{
		FLASH_ProgramHalfWord(WriteAddr, pBuffer[i]);
		WriteAddr += 2;//��ַ����2.
	}
}
#endif
void IAP_BootLoader::STMFLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)			//��ָ����ַ��ʼд��ָ�����ȵ�����
{
	u32 secpos;	   //������ַ
	u16 secoff;	   //������ƫ�Ƶ�ַ(16λ�ּ���)
	u16 secremain; //������ʣ���ַ(16λ�ּ���)	   
	u16 i;
	u32 offaddr;   //ȥ��0X08000000��ĵ�ַ
	if (WriteAddr < STM32_FLASH_BASE || (WriteAddr >= (STM32_FLASH_BASE + 1024 * STM32_FLASH_SIZE)))return;//�Ƿ���ַ
	FLASH_Unlock();						//����
	offaddr = WriteAddr - STM32_FLASH_BASE;			//ʵ��ƫ�Ƶ�ַ.
	secpos = offaddr / STM_SECTOR_SIZE;					//������ַ  0~127 for STM32F103RBT6
	secoff = (offaddr%STM_SECTOR_SIZE) / 2;			//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
	secremain = STM_SECTOR_SIZE / 2 - secoff;		//����ʣ��ռ��С   
	if (NumToWrite <= secremain)secremain = NumToWrite;//�����ڸ�������Χ
	while (1)
	{
		STMFLASH_Read(secpos*STM_SECTOR_SIZE + STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE / 2);//������������������
		for (i = 0; i < secremain; i++)//У������
		{
			if (STMFLASH_BUF[secoff + i] != 0XFFFF)break;//��Ҫ����  	  
		}
		if (i < secremain)//��Ҫ����
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE + STM32_FLASH_BASE);//�����������
			for (i = 0; i<secremain; i++)//����
			{
				STMFLASH_BUF[i + secoff] = pBuffer[i];
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE + STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE / 2);//д����������  
			//			printf("download flash\r\n");
		}
		else STMFLASH_Write_NoCheck(WriteAddr, pBuffer, secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if (NumToWrite == secremain)break;//д�������
		else//д��δ����
		{
			secpos++;				//������ַ��1
			secoff = 0;				//ƫ��λ��Ϊ0 	 
			pBuffer += secremain;  	//ָ��ƫ��
			WriteAddr += secremain * 2;	//д��ַƫ��	secremain = STM_SECTOR_SIZE / 2 - secoff;	�������һ��2
			NumToWrite -= secremain;	//�ֽ�(16λ)���ݼ�
			if (NumToWrite>(STM_SECTOR_SIZE / 2))secremain = STM_SECTOR_SIZE / 2;//��һ����������д����
			else secremain = NumToWrite;//��һ����������д����
		}
	};
	FLASH_Lock();//����
}

void IAP_BootLoader::STMFLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead)   			//��ָ����ַ��ʼ����ָ�����ȵ�����
{
	u16 i;
	for (i = 0; i < NumToRead; i++)
	{
		pBuffer[i] = STMFLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
		ReadAddr += 2;//ƫ��2���ֽ�.	
	}
}

