#ifndef __APP_COMMUNICATION_H__
#define __APP_COMMUNICATION_H__
#include "int_DS3553.h"
#include "int_GPS.h"
#include "int_QS100.h"
#include "cJSON.h"
#include "stdlib.h"
// 修正后的结构体
typedef struct
{
    // 纬度
    double lat;
    // 纬度方向 ('N' 或 'S')
    char latDir[2]; // 2字节：字符 + '\0'

    // 经度
    double lon;
    // 经度方向 ('E' 或 'W')
    char lonDir[2];

    // 规范时间格式 (YYYY-MM-DD HH:MM:SS)
    char datetime[20]; // 20字节足够存储时间字符串

    // 步数
    uint32_t step;

} UpdateData_Type;
void app_communication_init(void);
/**
 * @brief 处理数据发送给云端
 *
 */
void app_communication_send(uint8_t *ip, uint16_t port);
#endif /* __APP_COMMUNICATION_H__ */
