#ifndef __KEY_H
#define __KEY_H	 

#define KEY_PORT    GPIOA

#define KEY0_PIN     GPIO_Pin_11
#define READ_KEY0    GPIO_ReadInputDataBit(KEY_PORT,KEY0_PIN)//��ȡ����0

#define KEY1_PIN     GPIO_Pin_12
#define READ_KEY1    GPIO_ReadInputDataBit(KEY_PORT,KEY1_PIN)//��ȡ����1

#define RESET_PIN     GPIO_Pin_8
#define READ_RESET_KEY    GPIO_ReadInputDataBit(KEY_PORT,RESET_PIN)//��ȡ��λ����

void bsp_InitKey(void);//IO��ʼ��
void KEY_Scan(void);  	//����ɨ�躯��					    
#endif
