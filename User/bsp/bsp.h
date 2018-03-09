/*
*********************************************************************************************************
*
*	ģ������ : BSPģ��
*	�ļ����� : bsp.h
*	˵    �� : ���ǵײ�����ģ�����е�h�ļ��Ļ����ļ��� Ӧ�ó���ֻ�� #include bsp.h ���ɣ�
*			  ����Ҫ#include ÿ��ģ��� h �ļ�
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_H_
#define _BSP_H

#define STM32_V4
//#define STM32_X2

/* ����Ƿ����˿������ͺ� */
#if !defined (STM32_V4) && !defined (STM32_X2)
	#error "Please define the board model : STM32_X2 or STM32_V4"
#endif

/* ���� BSP �汾�� */
#define __STM32F1_BSP_VERSION		"1.1"

/* CPU����ʱִ�еĺ��� */
//#define CPU_IDLE()		bsp_Idle()

/* ����ȫ���жϵĺ� */
#define ENABLE_INT()	__set_PRIMASK(0)	/* ʹ��ȫ���ж� */
#define DISABLE_INT()	__set_PRIMASK(1)	/* ��ֹȫ���ж� */

/* ���������ڵ��Խ׶��Ŵ� */
#define BSP_Printf		printf
//#define BSP_Printf(...)
#define DEBUG 0
#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef TRUE
	#define TRUE  1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

//app need
#define NC 0//�̵�������--����
#define NO 1//�̵�������--����

#define DATA_LEN 1024

//��������״̬��������Ϣ
typedef struct
{
    uint8_t status;//���ߵ�link״̬ 
    uint16_t len;
    uint8_t buf[DATA_LEN+13];//1024�ֽ�����+13�ֽ�Э��
}NetData_T;

//����ʱ����Ϣ������ʱ�䣬����״̬�����ؿ�״̬���׿���״̬
typedef struct
{
    uint8_t remoteOpen;//Զ�̿��ţ���4λΪ1��a�ţ���4λΪ1��b��
    uint8_t firstCardStatus;//д1��ʾ�׿��Ѿ����ˣ�д0��ʾ����ȴ�
    uint16_t multipleCardStatus;//��ʾˢ����
    uint8_t time[5];//ʱ��
}RunInfo_T;

//�������ö��
enum NetCfg_Enum
{
    e_local_port,
    e_server_port,
    e_local_ip,
    e_server_ip,
    e_Link,
    e_NoLink
};

//���ŷ�ʽö�٣�4��
enum ReaderOrButton_Enum
{
    e_READER_A,//A��ͷ
    e_READER_B,//B��ͷ
    e_BUTTON_A,//A����
    e_BUTTON_B //B����
};

//ID����ö��,9��
enum ID_Enum
{
    e_firstCardID,
    
    e_superCardID,
    e_superPasswordID,
    
    e_threatCardID,
    e_threatPasswordID,
    
    e_keyBoardID,
    
    e_multipleCardID,

    e_generalCardID,
    e_fingerID
};

//���ֿ��ŷ�ʽ�Ϳ���/�ȴ�ʱ����ö�٣�10��
enum OpenCfg_Enum
{
    e_openTime,
    e_waitTime,
    e_interlock,
    e_firstCard,
    e_multipleCard,
    e_superCard,
    e_superPassword,
    e_threatCard,
    e_threatPassword,
    e_finger
};

#include "bsp_led.h"
#include "bsp_timer.h"

#include "bsp_cpu_flash.h"

#include "param.h"
#include "bsp_i2c_gpio.h"
#include "bsp_eeprom_24xx.h"

#include "bsp_spi_bus.h"
#include "bsp_spi_flash.h"
#include "key.h"
#include "bsp_dwt.h"

#include "bsp_flash.h"
#include "bsp_iap.h"

/* �ṩ������C�ļ����õĺ��� */
void bsp_Init(void);
void bsp_Idle(void);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
