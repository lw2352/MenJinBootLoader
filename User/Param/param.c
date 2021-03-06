#include "bsp.h"
//地址宏定义
//第一页
#define ADDR_LOCAL_IP 1
#define ADDR_LOCAL_PORT 5
#define ADDR_SERVER_IP 7
#define ADDR_SERVER_PORT 11
#define ADDR_OPEN_TIME 13
#define ADDR_RELATION_READER_A 14
#define ADDR_RELATION_READER_B 15
#define ADDR_RELATION_BUTTON_A 16
#define ADDR_RELATION_BUTTON_B 17
#define ADDR_WAIT_TIME 18
#define NEXT_START_SECTOR_H 28
#define NEXT_START_SECTOR_L 29
#define NEXT_START_ADDR_H 30
#define NEXT_START_ADDR_L 31
//第二页
#define FIRST_CARD_ID       EE_PAGE_SIZE+0
#define IS_OPEN_FIRST_CARD  EE_PAGE_SIZE+3
#define SUPER_CARD_ID       EE_PAGE_SIZE+4
#define IS_OPEN_SUPER_CARD  EE_PAGE_SIZE+7
#define THREAT_CARD_ID      EE_PAGE_SIZE+8//后面存了3张卡
#define IS_OPEN_THREAT_CARD EE_PAGE_SIZE+17
#define IS_OPEN_INTERLOCK   EE_PAGE_SIZE+18
#define THREAT_PASSWORD_ID  EE_PAGE_SIZE+19
#define IS_OPEN_THREAT_PASSWORD EE_PAGE_SIZE+22
#define SUPER_PASSWORD_ID   EE_PAGE_SIZE+23
#define IS_OPEN_SUPER_PASSWORD EE_PAGE_SIZE+26
//第三页
#define IS_OPEN_MULTIPLE_CARD EE_PAGE_SIZE*2+0
#define MULTIPLE_CARD_ID      EE_PAGE_SIZE*2+1//后面存10张
//第四页及以后
#define IS_OPEN_FINGER      EE_PAGE_SIZE*3+0
#define KEY_BOARD_ID        EE_PAGE_SIZE*3+1
#define FINGER_ID           EE_PAGE_SIZE*3+4//后面存500个指纹,占1500字节
#define GENERAL_CARD_ID     EE_PAGE_SIZE*3+1504//后面存500个卡号
#define END_ADDR            EE_PAGE_SIZE*3+3004//总共3101（0--3100）个字节

Param_T g_tParam;

void updateRelation(Relation_T *relation, enum ReaderOrButton_Enum type);
void updateNetCfg(NetCfg_T *netCfg, enum NetCfg_Enum type);
void updateSystemCfg(SystemCfg_T *systemCfg, enum OpenCfg_Enum type);
void updateNextStartAddr(NextStartAddr_T *nextStartAddr);
void updateMultipleCardID(uint8_t *data, uint16_t len, enum ID_Enum type);//直接传数组地址+长度+flag
static uint8_t compareID(uint8_t *id, uint8_t *idLibrary, uint16_t len);


//读取参数
void paramInit(void)
{
    uint8_t temp[300];//暂存从IIC读取的数据，可放100个ID
    uint8_t i;
    uint16_t len=300;
    //复制函数指针
    g_tParam.updateRelation = updateRelation;
    g_tParam.updateNetCfg = updateNetCfg;
    g_tParam.updateSystemCfg = updateSystemCfg;
    g_tParam.updateNextStartAddr = updateNextStartAddr;
    g_tParam.updateMultipleCardID = updateMultipleCardID;
    
    ee_ReadBytes(temp, 0, EE_PAGE_SIZE*3);
    
    //first page
    if(temp[ADDR_LOCAL_IP] == 0xFF)
    {
        //若IP没有配置过，则重置
        resetParam();
        //把普通卡号数组全置为0xFF,已在重置函数中完成
        //memset(g_tParam.multipleCardID.generalCardID, 0xFF, sizeof(g_tParam.multipleCardID.generalCardID));
        return;
    }

    memcpy(g_tParam.netCfg.local_ip,    &temp[ADDR_LOCAL_IP], sizeof(g_tParam.netCfg.local_ip));
    //memcpy(g_tParam.netCfg.local_port,  &temp[ADDR_LOCAL_PORT], sizeof(g_tParam.netCfg.local_port));
    g_tParam.netCfg.local_port = (temp[ADDR_LOCAL_PORT]<<8) + temp[ADDR_LOCAL_PORT+1];
    memcpy(g_tParam.netCfg.server_ip,   &temp[ADDR_SERVER_IP], sizeof(g_tParam.netCfg.server_ip));
    //memcpy(g_tParam.netCfg.server_port, &temp[ADDR_SERVER_PORT], sizeof(g_tParam.netCfg.server_port));
    g_tParam.netCfg.server_port = (temp[ADDR_SERVER_PORT]<<8) + temp[ADDR_SERVER_PORT+1];

    memcpy(&g_tParam.systemCfg.openTime, &temp[ADDR_OPEN_TIME], sizeof(g_tParam.systemCfg.openTime));
    memcpy(&g_tParam.systemCfg.waitTime, &temp[ADDR_WAIT_TIME], sizeof(g_tParam.systemCfg.waitTime));
    
    memcpy(&g_tParam.relation.relationA.reader_switcher, &temp[ADDR_RELATION_READER_A], sizeof(g_tParam.relation.relationA.reader_switcher));
    memcpy(&g_tParam.relation.relationB.reader_switcher, &temp[ADDR_RELATION_READER_B], sizeof(g_tParam.relation.relationB.reader_switcher));
    memcpy(&g_tParam.relation.relationA.button_switcher, &temp[ADDR_RELATION_BUTTON_A], sizeof(g_tParam.relation.relationA.reader_switcher));
    memcpy(&g_tParam.relation.relationB.button_switcher, &temp[ADDR_RELATION_BUTTON_B], sizeof(g_tParam.relation.relationB.reader_switcher));
    
    //i = sizeof(g_tParam.nextStartAddr.nextStartSector);
    memcpy(g_tParam.nextStartAddr.nextStartSector, &temp[NEXT_START_SECTOR_H], sizeof(g_tParam.nextStartAddr.nextStartSector));
    //i = sizeof(g_tParam.nextStartAddr.nextStartAddr);
    memcpy(g_tParam.nextStartAddr.nextStartAddr, &temp[NEXT_START_ADDR_H], sizeof(g_tParam.nextStartAddr.nextStartAddr));
    
    //第二页及以后
    //8个配置开关
    memcpy(&g_tParam.systemCfg.multipleOpenCfg[0], &temp[IS_OPEN_INTERLOCK], 1);
    memcpy(&g_tParam.systemCfg.multipleOpenCfg[1], &temp[IS_OPEN_FIRST_CARD], 1);
    memcpy(&g_tParam.systemCfg.multipleOpenCfg[2], &temp[IS_OPEN_MULTIPLE_CARD], 1);
    memcpy(&g_tParam.systemCfg.multipleOpenCfg[3], &temp[IS_OPEN_SUPER_CARD], 1);
    memcpy(&g_tParam.systemCfg.multipleOpenCfg[4], &temp[IS_OPEN_SUPER_PASSWORD], 1);
    memcpy(&g_tParam.systemCfg.multipleOpenCfg[5], &temp[IS_OPEN_THREAT_CARD], 1);
    memcpy(&g_tParam.systemCfg.multipleOpenCfg[6], &temp[IS_OPEN_THREAT_PASSWORD], 1);
    memcpy(&g_tParam.systemCfg.multipleOpenCfg[7], &temp[IS_OPEN_FINGER], 1);
    
    //last page
    //9个卡号数组
    memcpy(g_tParam.multipleCardID.firstCardID,     &temp[FIRST_CARD_ID], sizeof(g_tParam.multipleCardID.firstCardID));
    memcpy(g_tParam.multipleCardID.superCardID,     &temp[SUPER_CARD_ID], sizeof(g_tParam.multipleCardID.superCardID));
    memcpy(g_tParam.multipleCardID.threatCardID,    &temp[THREAT_CARD_ID], sizeof(g_tParam.multipleCardID.threatCardID));
    memcpy(g_tParam.multipleCardID.threatPasswordID,&temp[THREAT_PASSWORD_ID], sizeof(g_tParam.multipleCardID.threatPasswordID));
    memcpy(g_tParam.multipleCardID.superPasswordID, &temp[SUPER_PASSWORD_ID], sizeof(g_tParam.multipleCardID.superPasswordID));
    memcpy(g_tParam.multipleCardID.multipleCardID, &temp[MULTIPLE_CARD_ID], sizeof(g_tParam.multipleCardID.multipleCardID));
    memcpy(g_tParam.multipleCardID.keyBoardID, &temp[KEY_BOARD_ID], sizeof(g_tParam.multipleCardID.keyBoardID));
    //一次读300字节，有100个卡号，分5次读取
    for(i=0;i<5;i++)
    {
        ee_ReadBytes(temp, FINGER_ID+i*300, len);//指纹ID
        memcpy(&g_tParam.multipleCardID.fingerID[i*len], temp, sizeof(temp));
    }
    for(i=0;i<5;i++)
    {
        ee_ReadBytes(temp, GENERAL_CARD_ID+i*300, len);//普通卡ID
        memcpy(&g_tParam.multipleCardID.generalCardID[i*len], temp, sizeof(temp));
    }
       
}

//复位参数, 需要重新设置w5500
void resetParam(void)
{
    uint16_t i;
    uint8_t buf[EE_PAGE_SIZE];
    uint8_t temp[2]; 
    uint8_t card[3]; 
#if DEBUG    
    uint8_t test[300];
#endif    
    for(i = 0; i < EE_PAGE_SIZE; i++)
	{
		buf[i] = 0xFF;
	}
      
    for(i = 1; i<97; i++)//只把第二页及以后填0xFF，97*32=3104>3100，可以填充完
    {
        ee_WriteBytes(buf, i*32, EE_PAGE_SIZE);
    }
 #if DEBUG   
    //test
    bsp_DelayMS(100);
    ee_ReadBytes(test, 0, 300);
 #endif
    //复位结构体成员的值
    //第一页的默认参数，不用0xFF填充
    g_tParam.netCfg.local_ip[0]=192;
    g_tParam.netCfg.local_ip[1]=168;
    g_tParam.netCfg.local_ip[2]=3;
    g_tParam.netCfg.local_ip[3]=100;
    g_tParam.netCfg.local_port = 6000;
    
    //server IP:255.255.255.255,broadcast
    g_tParam.netCfg.server_ip[0]=255;
    g_tParam.netCfg.server_ip[1]=255;
    g_tParam.netCfg.server_ip[2]=255;
    g_tParam.netCfg.server_ip[3]=255; 
    g_tParam.netCfg.server_port = 8085;
    
    g_tParam.systemCfg.openTime = 5;
    g_tParam.systemCfg.waitTime = 0;
    memset(g_tParam.systemCfg.multipleOpenCfg, 0, sizeof(g_tParam.systemCfg.multipleOpenCfg));
    
    g_tParam.relation.relationA.reader_switcher = 1;
    g_tParam.relation.relationB.reader_switcher = (1<<4);
    g_tParam.relation.relationA.button_switcher = 1;
    g_tParam.relation.relationB.button_switcher = (1<<4);
    
    g_tParam.nextStartAddr.nextStartSector[0] = 0;
    g_tParam.nextStartAddr.nextStartSector[1] = 64;
    g_tParam.nextStartAddr.nextStartAddr[0] = 0;
    g_tParam.nextStartAddr.nextStartAddr[1] = 0;
    
    //开始写入
    //net
    ee_WriteBytes(g_tParam.netCfg.local_ip, ADDR_LOCAL_IP, sizeof(g_tParam.netCfg.local_ip));
    temp[0] = g_tParam.netCfg.local_port >> 8;
    temp[1] = g_tParam.netCfg.local_port & 0xFF;
    ee_WriteBytes(temp, ADDR_LOCAL_PORT, sizeof(temp));
    ee_WriteBytes(g_tParam.netCfg.server_ip, ADDR_SERVER_IP, sizeof(g_tParam.netCfg.server_ip));
    temp[0] = g_tParam.netCfg.server_port >> 8;
    temp[1] = g_tParam.netCfg.server_port & 0xFF;
    ee_WriteBytes(temp, ADDR_SERVER_PORT, sizeof(temp));
    //配置开关
    ee_WriteOneBytes(g_tParam.systemCfg.openTime, ADDR_OPEN_TIME);
    ee_WriteOneBytes(g_tParam.systemCfg.waitTime, ADDR_WAIT_TIME);   
    ee_WriteOneBytes(g_tParam.systemCfg.multipleOpenCfg[0], IS_OPEN_INTERLOCK);
    ee_WriteOneBytes(g_tParam.systemCfg.multipleOpenCfg[1], IS_OPEN_FIRST_CARD);
    ee_WriteOneBytes(g_tParam.systemCfg.multipleOpenCfg[2], IS_OPEN_MULTIPLE_CARD);
    ee_WriteOneBytes(g_tParam.systemCfg.multipleOpenCfg[3], IS_OPEN_SUPER_CARD);
    ee_WriteOneBytes(g_tParam.systemCfg.multipleOpenCfg[4], IS_OPEN_SUPER_PASSWORD);
    ee_WriteOneBytes(g_tParam.systemCfg.multipleOpenCfg[5], IS_OPEN_THREAT_CARD);
    ee_WriteOneBytes(g_tParam.systemCfg.multipleOpenCfg[6], IS_OPEN_THREAT_PASSWORD);
    ee_WriteOneBytes(g_tParam.systemCfg.multipleOpenCfg[7], IS_OPEN_FINGER);
    //对应关系
    ee_WriteOneBytes(g_tParam.relation.relationA.reader_switcher, ADDR_RELATION_READER_A);
    ee_WriteOneBytes(g_tParam.relation.relationB.reader_switcher, ADDR_RELATION_READER_B);
    ee_WriteOneBytes(g_tParam.relation.relationA.button_switcher, ADDR_RELATION_BUTTON_A);
    ee_WriteOneBytes(g_tParam.relation.relationB.button_switcher, ADDR_RELATION_BUTTON_B);
    //下一次历史记录地址
    ee_WriteOneBytes(g_tParam.nextStartAddr.nextStartSector[0], NEXT_START_SECTOR_H);
    ee_WriteOneBytes(g_tParam.nextStartAddr.nextStartSector[1], NEXT_START_SECTOR_L);
    ee_WriteOneBytes(g_tParam.nextStartAddr.nextStartAddr[0], NEXT_START_ADDR_H);
    ee_WriteOneBytes(g_tParam.nextStartAddr.nextStartAddr[1], NEXT_START_ADDR_L);
    
    //i = sizeof(g_tParam.multipleCardID);
    memset(&g_tParam.multipleCardID, 0xFF, sizeof(g_tParam.multipleCardID));
    
    card[0]=0x79;
    card[1]=0xA3;
    card[2]=0x8E;
    memcpy(&g_tParam.multipleCardID.generalCardID[0], card, sizeof(card));
    
    card[0]=0x79;
    card[1]=0x21;
    card[2]=0x45;
    memcpy(&g_tParam.multipleCardID.generalCardID[3], card, sizeof(card));
    g_tParam.updateMultipleCardID(g_tParam.multipleCardID.generalCardID, 1500, e_generalCardID);
 #if DEBUG    
    //test
    bsp_DelayMS(100);
    ee_ReadBytes(test, 0, 300);
    #endif
    //重新设置网络
    //set_default(&g_tParam.netCfg);
}

//更新参数--对应关系
void updateRelation(Relation_T *relation, enum ReaderOrButton_Enum type)
{
    switch(type)
    {
        case e_READER_A:
            ee_WriteOneBytes(relation->relationA.reader_switcher, ADDR_RELATION_READER_A);
            break;
        case e_READER_B:
            ee_WriteOneBytes(relation->relationB.reader_switcher, ADDR_RELATION_READER_B);
            break;
        case e_BUTTON_A:
            ee_WriteOneBytes(relation->relationA.button_switcher, ADDR_RELATION_BUTTON_A);
            break;
        case e_BUTTON_B:
            ee_WriteOneBytes(relation->relationB.button_switcher, ADDR_RELATION_BUTTON_B);
            break;
        default:
            break;
    }
}

//更新参数--网络配置
void updateNetCfg(NetCfg_T *netCfg, enum NetCfg_Enum type)
{
    uint8_t temp[2];
    switch(type)
    {
        case e_local_ip:
            ee_WriteBytes(netCfg->local_ip, ADDR_LOCAL_IP, sizeof(netCfg->local_ip));
            break;
        case e_local_port:
            temp[0] = netCfg->local_port >> 8;
            temp[1] = netCfg->local_port & 0xFF;
            ee_WriteBytes(temp, ADDR_LOCAL_PORT, sizeof(temp));
            break;
        case e_server_ip:
            ee_WriteBytes(netCfg->server_ip, ADDR_SERVER_IP, sizeof(netCfg->server_ip));
            break;
        case e_server_port:
            temp[0] = netCfg->server_port >> 8;
            temp[1] = netCfg->server_port & 0xFF;
            ee_WriteBytes(temp, ADDR_SERVER_PORT, sizeof(temp));
            break;
        default:
            break;
    }
}

//更新参数--系统配置
void updateSystemCfg(SystemCfg_T *systemCfg, enum OpenCfg_Enum type)
{
    switch(type)
    {
        case e_openTime:
            ee_WriteOneBytes(systemCfg->openTime, ADDR_OPEN_TIME);
            break;
        case e_waitTime:
            ee_WriteOneBytes(systemCfg->waitTime, ADDR_WAIT_TIME);
            break;
        case e_interlock:
            ee_WriteOneBytes(systemCfg->multipleOpenCfg[0], IS_OPEN_INTERLOCK);
            break;
        case e_firstCard:
            ee_WriteOneBytes(systemCfg->multipleOpenCfg[1], IS_OPEN_FIRST_CARD);
            break;
        case e_multipleCard:
            ee_WriteOneBytes(systemCfg->multipleOpenCfg[2], IS_OPEN_MULTIPLE_CARD);
            break;
        case e_superCard:
            ee_WriteOneBytes(systemCfg->multipleOpenCfg[3], IS_OPEN_SUPER_CARD);
            break;
        case e_superPassword:
            ee_WriteOneBytes(systemCfg->multipleOpenCfg[4], IS_OPEN_SUPER_PASSWORD);
            break;
        case e_threatCard:
            ee_WriteOneBytes(systemCfg->multipleOpenCfg[5], IS_OPEN_THREAT_CARD);
            break;
        case e_threatPassword:
            ee_WriteOneBytes(systemCfg->multipleOpenCfg[6], IS_OPEN_THREAT_PASSWORD);
            break;
        case e_finger:
            ee_WriteOneBytes(systemCfg->multipleOpenCfg[7], IS_OPEN_FINGER);
            break;
        default:
            break;
    }
}

//更新参数--下次历史记录地址
void updateNextStartAddr(NextStartAddr_T *nextStartAddr)
{
    ee_WriteOneBytes(nextStartAddr->nextStartSector[0], NEXT_START_SECTOR_H);
    ee_WriteOneBytes(nextStartAddr->nextStartSector[1], NEXT_START_SECTOR_L);
    ee_WriteOneBytes(nextStartAddr->nextStartAddr[0], NEXT_START_ADDR_H);
    ee_WriteOneBytes(nextStartAddr->nextStartAddr[1], NEXT_START_ADDR_L);
}

//更新参数--卡号配置
void updateMultipleCardID(uint8_t *data, uint16_t len, enum ID_Enum type)
{
    uint8_t i = 0;
    uint8_t temp[300]={0};
    switch(type)
    {
        case e_firstCardID:
            ee_WriteBytes(data, FIRST_CARD_ID, len);
            break;
        case e_superCardID:
            ee_WriteBytes(data, SUPER_CARD_ID, len);
            break;
        case e_superPasswordID:
            ee_WriteBytes(data, SUPER_PASSWORD_ID, len);
            break;
        case e_threatCardID:
            ee_WriteBytes(data, THREAT_CARD_ID, len);
            break;
        case e_threatPasswordID:
            ee_WriteBytes(data, THREAT_PASSWORD_ID, len);
            break;
        case e_keyBoardID:
            ee_WriteBytes(data, KEY_BOARD_ID, len);
            break;
        case e_multipleCardID:
            ee_WriteBytes(data, MULTIPLE_CARD_ID, len);
            break;
        case e_generalCardID://len=300
            len=len/5;
            //一次写300字节，有100个卡号，分5次写入
            for(i=0;i<5;i++)
            {
                ee_WriteBytes(data, GENERAL_CARD_ID+i*300, len);
                data += 300;
            }
            //test
            bsp_DelayMS(100);
            ee_ReadBytes(temp, GENERAL_CARD_ID, 300);
            break;
        case e_fingerID://len=300
            len=len/5;
            //一次写300字节，有100个卡号，分5次写入
            for(i=0;i<5;i++)
            {
                ee_WriteBytes(data, GENERAL_CARD_ID+i*300, len);
                data += 300;
            }
            break;
        default:
            break;
    }
}

//搜索卡号所属类型，返回对应枚举值，0xFF表示不存在
//判断成功后立即返回卡号类型，不成功则继续判断
uint8_t searchID(uint8_t *id)
{
    uint16_t i;
    
    for(i = 0; i<sizeof(g_tParam.systemCfg.multipleOpenCfg); i++)
    {
        if(g_tParam.systemCfg.multipleOpenCfg[i]!=0)
        {
            switch(i)
            {
                case 1://首卡
                    if(compareID(id, g_tParam.multipleCardID.firstCardID, sizeof(g_tParam.multipleCardID.firstCardID)) == 1)
                    {
                        return e_firstCardID;
                    }
                    break;
                    
                case 2://多重卡
                    if(compareID(id, g_tParam.multipleCardID.multipleCardID, sizeof(g_tParam.multipleCardID.multipleCardID)) == 1)
                    {
                        return e_multipleCardID;
                    }
                    break;
                
                case 3://超级卡
                    if(compareID(id, g_tParam.multipleCardID.superCardID, sizeof(g_tParam.multipleCardID.superCardID)) == 1)
                    {
                        return e_superCardID;
                    }
                    break;
                
                case 4://超级密码
                    if(compareID(id, g_tParam.multipleCardID.superPasswordID, sizeof(g_tParam.multipleCardID.superPasswordID)) == 1)
                    {
                        return e_superPasswordID;
                    }
                    break;
                
                case 5://胁迫卡
                    if(compareID(id, g_tParam.multipleCardID.threatCardID, sizeof(g_tParam.multipleCardID.threatCardID)) == 1)
                    {
                        return e_threatCardID;
                    }
                    break;
                
                case 6://胁迫码
                    if(compareID(id, g_tParam.multipleCardID.threatPasswordID, sizeof(g_tParam.multipleCardID.threatPasswordID)) == 1)
                    {
                        return e_threatPasswordID;
                    }
                    break;
                
                case 7://指纹模块
                    if(compareID(id, g_tParam.multipleCardID.keyBoardID, sizeof(g_tParam.multipleCardID.keyBoardID)) == 1)
                    {
                        return e_keyBoardID;
                    }
                    if(compareID(id, g_tParam.multipleCardID.fingerID, sizeof(g_tParam.multipleCardID.fingerID)) == 1)
                    {
                        return e_threatCardID;
                    }                    
                    break;
                
                default:
                    break;
            }//end of switch
        }//end of if
    }//end of for
    
    //判断普通卡
    if(compareID(id, g_tParam.multipleCardID.generalCardID, sizeof(g_tParam.multipleCardID.generalCardID)) == 1)
    {
        return e_generalCardID;
    }
    //都不符合，返回0xFF
    return 0xFF;
}

//比较卡号
static uint8_t compareID(uint8_t *id, uint8_t *idLibrary, uint16_t len)
{
    uint16_t i;
    for(i=0; i<len; i++)
    {
        if(id[0] == idLibrary[i])
        {
            if(id[1] == idLibrary[i+1])
            {
                if(id[2] == idLibrary[i+2])
                {
                    return 1;
                }
            }
        }
    }//end of for
    return 0;
}




