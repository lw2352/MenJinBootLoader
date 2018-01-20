/*
*********************************************************************************************************
*
*	ģ������ : ���������
*	�ļ����� : main.c
*	��    �� : V1.0
*	˵    �� : CPU�ڲ�Flash��д����
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		V1.0    2015-08-30 armfly  �׷�
*
*	Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"				/* �ײ�Ӳ������ */

/* ���������������̷������� */
#define EXAMPLE_NAME	"V4-009_CPU�ڲ�Flash��д����"
#define EXAMPLE_DATE	"2015-08-30"
#define DEMO_VER		"1.0"

static void PrintfLogo(void);
static void PrintfHelp(void);

/* ���Ե�FLASH��ַ - PAGE SIZE = 2K�ֽ� */
#define TEST_ADDR		(0x08000000 + 0x10000 - 0x400)

/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: c�������
*	��    �Σ���
*	�� �� ֵ: �������(���账��)
*********************************************************************************************************
*/
int main(void)
{
	uint8_t fRefresh = 0;
	uint8_t read;
	uint16_t value = 0;
	uint32_t d32 = 0;
	
	/*
		ST�̼����е������ļ��Ѿ�ִ���� SystemInit() �������ú����� system_stm32f4xx.c �ļ�����Ҫ������
	����CPUϵͳ��ʱ�ӣ��ڲ�Flash����ʱ������FSMC�����ⲿSRAM
	*/
	bsp_Init();		/* Ӳ����ʼ�� */
	
	PrintfLogo();	/* ��ӡ�������ƺͰ汾����Ϣ */
	PrintfHelp();	/* ��ӡ������ʾ */
	
	fRefresh = 1;
	/* �������ѭ�� */
	while (1)
	{
		bsp_Idle();		/* CPU����ʱִ�еĺ������� bsp.c */
	
		
		
		if (comGetChar(COM1, &read))
		{
			switch (read)
			{
				case '1':
					value = 0x1111;
					bsp_WriteCpuFlash(TEST_ADDR, (uint8_t *)&value, 2);			
					fRefresh = 1;
					break;

				case '2':
					value = 0x2222;
					bsp_WriteCpuFlash(TEST_ADDR, (uint8_t *)&value, 2);			
					fRefresh = 1;
					break;

				case '3':
					d32 = 0x12345678;
					bsp_WriteCpuFlash(TEST_ADDR + 2, (uint8_t *)&d32, 4);		
					fRefresh = 1;
					break;

				case '4':
					value = 0xFFFF;
					bsp_WriteCpuFlash(TEST_ADDR, (uint8_t *)&value, 2);		
					fRefresh = 1;
					break;	

				case '5':
					fRefresh = 1;
					break;					
			}

		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: PrintfHelp
*	����˵��: ��ӡ������ʾ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void PrintfHelp(void)
{
	printf("������ʾ:\r\n");
	printf("1. 0-1��ַд��0x1111\r\n");
	printf("2. 0-1��ַд��0x1234\r\n");
	printf("3. 2-5��ַд��0x12345678\r\n");	
	printf("4. 0-1��ַд��0xFFFF\r\n");	
	printf("5. ��16�ֽ�����\r\n");	
}

/*
*********************************************************************************************************
*	�� �� ��: PrintfLogo
*	����˵��: ��ӡ�������ƺ����̷�������, ���ϴ����ߺ󣬴�PC���ĳ����ն�������Թ۲���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void PrintfLogo(void)
{
	printf("*************************************************************\n\r");
	printf("* ��������   : %s\r\n", EXAMPLE_NAME);	/* ��ӡ�������� */
	printf("* ���̰汾   : %s\r\n", DEMO_VER);		/* ��ӡ���̰汾 */
	printf("* ��������   : %s\r\n", EXAMPLE_DATE);	/* ��ӡ�������� */

	/* ��ӡST�̼���汾���궨���� stm32f4xx.h �ļ� */
	printf("* �̼���汾 : %d.%d.%d\r\n", __STM32F10X_STDPERIPH_VERSION_MAIN,
			__STM32F10X_STDPERIPH_VERSION_SUB1,__STM32F10X_STDPERIPH_VERSION_SUB2);

	/* ��ӡ CMSIS �汾. �궨���� core_cm4.h �ļ� */
	printf("* CMSIS�汾  : %X.%02X\r\n", __CM3_CMSIS_VERSION_MAIN, __CM3_CMSIS_VERSION_SUB);

	printf("* \n\r");	/* ��ӡһ�пո� */
	printf("* QQ    : 1295744630 \r\n");
	printf("* Email : armfly@qq.com \r\n");
	printf("* Copyright www.armfly.com ����������\r\n");
	printf("*************************************************************\n\r");
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
