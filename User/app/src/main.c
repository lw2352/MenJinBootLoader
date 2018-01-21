/*
*********************************************************************************************************
*
*	模块名称 : 主程序入口
*	文件名称 : main.c
*	版    本 : V1.0
*	说    明 : CPU内部Flash读写例子
*	修改记录 :
*		版本号  日期       作者    说明
*		V1.0    2015-08-30 armfly  首发
*
*	Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"				/* 底层硬件驱动 */

#define IAP_PAGE_SIZE 2048//为了能够兼容F103 所以提出了2K这个更新大小
#define APP_START_ADDR (uint32_t)0x08004000//app程序从第16K开始
#define DEFAULT_START_ADDR (uint32_t)0x08044000//默认程序从第16K+256K=开始
/* 变量 */
typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;

uint8_t updateDataBuf[IAP_PAGE_SIZE] ={0};

uint8_t g_MainStatus = 0;	/* 状态机 */
static uint8_t status_0(void);
static uint8_t status_1(void);
static void status_2(void);
static void status_3(uint32_t address);
/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: c程序入口
*	形    参：无
*	返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
int main(void)
{
    uint8_t temp;
	/*
		ST固件库中的启动文件已经执行了 SystemInit() 函数，该函数在 system_stm32f4xx.c 文件，主要功能是
	配置CPU系统的时钟，内部Flash访问时序，配置FSMC用于外部SRAM
	*/
	bsp_Init();		/* 硬件初始化 */

    g_MainStatus = 0;	/* 初始状态为状态0 */
	/* 主程序大循环 */
	while (1)
	{
        switch(g_MainStatus)
        {
            case 0:
                if(status_0() == 1)
                {
                    //若复位按键按下5秒，则进行复位操作
                    resetParam();
                    sf_EraseChip();
                }
                bsp_LedOn(1);//指示复位成功
                g_MainStatus = 1;	/* 转移到状态1 */
                break;
            
            case 1:
                //先延时，再读取升级标志
                temp = status_1();
                if(temp == 1)
                {
                    g_MainStatus = 2;//需要升级
                }
                else if(temp == 0)
                {
                    //启动到app区
                    status_3(APP_START_ADDR);
                }
                else if(temp == 0xFF)
                {
                    //启动到默认区
                    status_3(DEFAULT_START_ADDR);
                }
                break;
            
            case 2:
                //需要升级，复制spi数据到mcu内部flash
                status_2();
                bsp_LedOn(2);//指示数据复制成功
                g_MainStatus = 3;
                break;
            
            case 3:
                //跳转
                status_3(APP_START_ADDR);
                break;
            
            default:
                break;
        }
	}
}

//检测复位按键是否按下
static uint8_t status_0(void)
{
    if(READ_RESET_KEY == 0)//high default,0表示低电平
    {
        //加上亮灯提示
        bsp_LedOn(3);
        bsp_DelayMS(5000);
        if(READ_RESET_KEY == 0)//按下5s
        {
            bsp_LedOff(3);
            return 1;
        }
    }
    return 0;
}

//先延时，再读取升级标志
static uint8_t status_1(void)
{
    uint8_t data;
    bsp_DelayMS(100);
    ee_ReadBytes(&data, 0, 1);
    return data;
}

//复制spi数据到mcu内部flash
static void status_2(void)
{
    uint8_t pageNum;
    for(pageNum=0; pageNum<128; pageNum++)//最多256/2=128页，0--127
    {
        sf_ReadBuffer(updateDataBuf, pageNum, IAP_PAGE_SIZE);//读spi的升级数据
        bsp_WriteCpuFlash(APP_START_ADDR+pageNum*IAP_PAGE_SIZE, updateDataBuf, IAP_PAGE_SIZE);//把升级数据写到mcu内部flash
    }
}

//跳转
static void status_3(uint32_t address)
{
    bsp_LedOn(3);//指示准备跳转
    
    JumpAddress = *(__IO uint32_t*) (address+4);
    /* 用户应用地址 */
    Jump_To_Application = (pFunction) JumpAddress;
    
    /* 初始化MSP */
    __set_MSP(*(__IO uint32_t*) address);
    //printf("\n\rJump_To_Application~\n\r");
    Jump_To_Application();
}
/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
