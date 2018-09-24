#ifndef _SPI_H
#define _SPI_H


#include "stm32f4xx.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "FIFOBuffer.h"


/******************************************************************************************************/
/****configuration  ʹ��ǰ��������****/
#define SPI_TX_BUFFER_SIZE     64              //SPI BUFFER FIFO SIZE
#define SPI_RX_BUFFER_SIZE     64              //SPI BUFFER FIFO SIZE
#define SPI_DMA_TX_BUFFER_SIZE 20               //SPI DMA BUFFER SIZE

/*******************************************************************************************************/

typedef enum
{
	SPI_RXNE_IRQ,				//SPI���ջ������ǿ��ж�
	SPI_TXE_IRQ,				//SPI���ͻ��������ж�
	SPI_ERR_IRQ					//SPI�����ж�
}SPIIrqType;

typedef enum
{
	DMA_HT_IRQ,					//DMA��������ж�
	DMA_TC_IRQ,					//DMA��������ж�
	DMA_TE_IRQ					//DMA�����ж�
}DMAIrqType;

class SPI{

private:

	FIFOBuffer<u8, SPI_TX_BUFFER_SIZE>  SPIbufferTx;
	FIFOBuffer<u8, SPI_RX_BUFFER_SIZE>  SPIbufferRx;
	u8 bufferTxDma[SPI_DMA_TX_BUFFER_SIZE];
	u32  dmaChannel;				//dmaͨ��
	DMA_Stream_TypeDef*  mDMA_Streamx;//DMA data stream
	uint32_t dmaTCFlagChannel;								//dma��������жϱ�־λ
	uint32_t dmaGLFlagChannel;								//dmaȫ���жϱ�־λ
	uint32_t dmaTEFlagChannel;								//dma�����жϱ�־λ
	bool isBusySend;
	bool mUseDma;
	unsigned char mPrecision;
	/*-------SPI Pin Configuration------*/
	SPI_TypeDef*	 SPIx;
	u32            mGPIORcc;  				//GPIO Clock
	u16            mMISOPinSource;    //MISO gpio pinSource
	u16            mMOSIPinSource;    //MOSI gpio pinSource
	u16            mSCKPinSource;     //MOSI gpio pinSource
	u8             mGPIOAF;   				//GPIO AF

	void RCC_Configuration(void);

public:

	SPI(SPI_TypeDef* SPI, bool useDMA = false, u8 remap = 0, u8 Prioritygroup = 3, uint8_t preemprionPriority = 7, uint8_t subPriority = 1, u8 dmaPriority = 3);

	u8 SPI_RW(u8 dat);
	bool SendData(uint8_t *pbuffer, uint32_t size);
	bool GetReceivedData(u8* buffer, u16 number);

	void SetSpeed(u8 SPI_BaudRatePrescaler);

	u16 ReceiveBufferSize();
	u16 SendBufferSize();

	void ClearReceiveBuffer();
	void ClearSendBuffer();

	SPI_TypeDef* getSPI();
	bool UseDma();

	SPIIrqType SpiIrq();
	DMAIrqType DmaIrq();

	void SetCSNPin(u8 value);

};


#endif
