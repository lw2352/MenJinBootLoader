#ifndef __KEY_H
#define __KEY_H	 

#define KEY_PORT    GPIOA

#define KEY0_PIN     GPIO_Pin_11
#define READ_KEY0    GPIO_ReadInputDataBit(KEY_PORT,KEY0_PIN)//读取按键0

#define KEY1_PIN     GPIO_Pin_12
#define READ_KEY1    GPIO_ReadInputDataBit(KEY_PORT,KEY1_PIN)//读取按键1

#define RESET_PIN     GPIO_Pin_8
#define READ_RESET_KEY    GPIO_ReadInputDataBit(KEY_PORT,RESET_PIN)//读取复位按键

void bsp_InitKey(void);//IO初始化
void KEY_Scan(void);  	//按键扫描函数					    
#endif
