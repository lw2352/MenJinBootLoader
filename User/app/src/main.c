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

#define IAP_PAGE_SIZE 2048//Ϊ���ܹ�����F103 ���������2K������´�С
#define APP_START_ADDR (uint32_t)0x08004000//app����ӵ�16K��ʼ
#define DEFAULT_START_ADDR (uint32_t)0x08044000//Ĭ�ϳ���ӵ�16K+256K=��ʼ
/* ���� */
typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;

uint8_t updateDataBuf[IAP_PAGE_SIZE] ={0};

uint8_t g_MainStatus = 0;	/* ״̬�� */
static uint8_t status_0(void);
static uint8_t status_1(void);
static void status_2(void);
static void status_3(uint32_t address);
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
    uint8_t temp;
	/*
		ST�̼����е������ļ��Ѿ�ִ���� SystemInit() �������ú����� system_stm32f4xx.c �ļ�����Ҫ������
	����CPUϵͳ��ʱ�ӣ��ڲ�Flash����ʱ������FSMC�����ⲿSRAM
	*/
	bsp_Init();		/* Ӳ����ʼ�� */

    g_MainStatus = 0;	/* ��ʼ״̬Ϊ״̬0 */
	/* �������ѭ�� */
	while (1)
	{
        switch(g_MainStatus)
        {
            case 0:
                if(status_0() == 1)
                {
                    //����λ��������5�룬����и�λ����
                    resetParam();
                    sf_EraseChip();
                }
                bsp_LedOn(1);//ָʾ��λ�ɹ�
                g_MainStatus = 1;	/* ת�Ƶ�״̬1 */
                break;
            
            case 1:
                //����ʱ���ٶ�ȡ������־
                temp = status_1();
                if(temp == 1)
                {
                    g_MainStatus = 2;//��Ҫ����
                }
                else if(temp == 0)
                {
                    //������app��
                    status_3(APP_START_ADDR);
                }
                else if(temp == 0xFF)
                {
                    //������Ĭ����
                    status_3(DEFAULT_START_ADDR);
                }
                break;
            
            case 2:
                //��Ҫ����������spi���ݵ�mcu�ڲ�flash
                status_2();
                bsp_LedOn(2);//ָʾ���ݸ��Ƴɹ�
                g_MainStatus = 3;
                break;
            
            case 3:
                //��ת
                status_3(APP_START_ADDR);
                break;
            
            default:
                break;
        }
	}
}

//��⸴λ�����Ƿ���
static uint8_t status_0(void)
{
    if(READ_RESET_KEY == 0)//high default,0��ʾ�͵�ƽ
    {
        //����������ʾ
        bsp_LedOn(3);
        bsp_DelayMS(5000);
        if(READ_RESET_KEY == 0)//����5s
        {
            bsp_LedOff(3);
            return 1;
        }
    }
    return 0;
}

//����ʱ���ٶ�ȡ������־
static uint8_t status_1(void)
{
    uint8_t data;
    bsp_DelayMS(100);
    ee_ReadBytes(&data, 0, 1);
    return data;
}

//����spi���ݵ�mcu�ڲ�flash
static void status_2(void)
{
    uint8_t pageNum;
    for(pageNum=0; pageNum<128; pageNum++)//���256/2=128ҳ��0--127
    {
        sf_ReadBuffer(updateDataBuf, pageNum, IAP_PAGE_SIZE);//��spi����������
        bsp_WriteCpuFlash(APP_START_ADDR+pageNum*IAP_PAGE_SIZE, updateDataBuf, IAP_PAGE_SIZE);//����������д��mcu�ڲ�flash
    }
}

//��ת
static void status_3(uint32_t address)
{
    bsp_LedOn(3);//ָʾ׼����ת
    
    JumpAddress = *(__IO uint32_t*) (address+4);
    /* �û�Ӧ�õ�ַ */
    Jump_To_Application = (pFunction) JumpAddress;
    
    /* ��ʼ��MSP */
    __set_MSP(*(__IO uint32_t*) address);
    //printf("\n\rJump_To_Application~\n\r");
    Jump_To_Application();
}
/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
