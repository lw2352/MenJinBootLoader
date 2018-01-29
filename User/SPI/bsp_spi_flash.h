/*
*********************************************************************************************************
*
*	ģ������ : SPI�ӿڴ���FLASH ��дģ��
*	�ļ����� : bsp_spi_flash.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_SPI_FLASH_H
#define _BSP_SPI_FLASH_H

#define SF_MAX_PAGE_SIZE	(4 * 1024)

/* ���崮��Flash ID */
enum
{
	SST25VF016B_ID = 0xBF2541,
	MX25L1606E_ID  = 0xC22015,
	W25Q64_ID    = 0xEF4017,
	W25Q128_ID   = 0xEF4018
};

//�ṹ����޸ģ�page��СΪ256�ֽڣ�sector��СΪ4K�ֽ�
typedef struct
{
	uint32_t ChipID;		/* оƬID */
	char ChipName[16];		/* оƬ�ͺ��ַ�������Ҫ������ʾ */
	uint32_t TotalSize;		/* ������ */
	uint16_t PageSize;		/* ҳ���С */
}SFLASH_T;

void sf_EraseChip(void);
//menjin need
extern SFLASH_T g_tSF;
void bsp_InitSFlash(void);
void sf_ReadBuffer(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize);
void sf_EraseSector(uint32_t _uiSectorAddr);
void sf_exWrite8Bytes(uint8_t* _pBuf, uint32_t _uiWriteAddr);//8�ֽ�
void sf_exWriteOnePage(uint8_t* _pBuf, uint32_t _uiWriteAddr);//256�ֽ�
void sf_PageWrite(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize);
void storeRecord(uint8_t *id, uint8_t reader);//�����ڲ��Լ���ȡʱ���д���ַ

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/