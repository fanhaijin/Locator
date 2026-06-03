#include "app_communication.h"

static UpdateData_Type updateData_Type;
static uint8_t gps_data[1024] = {0};
static uint16_t gps_size = 0;
void app_communication_init(void)
{
    int_DS3553_Reg_Init();
    int_GPS_Init();
    int_qs100_init();
}
static double NMEA_To_Decimal(double *number)
{
    float temp = *number;
    *number = (int)(*number / 100);
    temp = temp - (*number) * 100;
    return (temp / 60) + *number;
}
/**
 * @brief 处理数据发送给云端
 *
 */
void app_communication_send(uint8_t *ip, uint16_t port)
{
    // 1. 读取GPS中的数据
    uint8_t date[7];
    uint8_t time[7];
    while (1)
    {
        int_GPS_ReadData(gps_data, &gps_size);
        // $GNRMC,154919.000,A,3028.09110,N,11423.29505,E,3.44,156.11,060525,,,A,V*00
        strcpy((char *)gps_data, "$GNRMC,154919.000,A,3028.09110,N,11423.29505,E,3.44,156.11,060525,,,A,V*00");
        gps_size = strlen((char *)gps_data);
        if (gps_size > 0)
        {
            char gps_flag = '0';
            // sscanf 的用法
            // %*[^,] 匹配任意字符，直到遇到逗号，这里是遇到指定字符停止
            sscanf((char *)gps_data, "$GNRMC,%*[^,],%c", &gps_flag);
            debug_printfln("gps_flag: %c\r\n", gps_flag);
            if (gps_flag == 'A')
            {
                debug_printfln("gps OK\r\n");
                // 解析gps中的数据
                // 在读取字符串时要读取指定长度的字符串
                sscanf((char *)gps_data, "$GNRMC,%10s,A,%lf,%1s,%lf,%1s,%*[^,],%*[^,],%6s,", time, &updateData_Type.lat, &updateData_Type.latDir, &updateData_Type.lon, &updateData_Type.lonDir, date);
                break;
            }
            else
            {
                debug_printfln("gps error\r\n");
                HAL_Delay(1000);
            }
        }
    }
    // 对字符串进行转换
    sprintf((char *)updateData_Type.datetime, "20%c%c-%c%c-%c%c %c%c:%c%c:%c%c", date[4], date[5], date[2], date[3], date[0], date[1], time[0], time[1], time[2], time[3], time[4], time[5]);
    // 将经纬度进行转化
    updateData_Type.lat = NMEA_To_Decimal(&updateData_Type.lat);
    updateData_Type.lon = NMEA_To_Decimal(&updateData_Type.lon);
    // 2. 读取计步数据
    updateData_Type.step = int_DS3553_getStep();
    // 3. 将数据转化为JSON数据
    // 创建JSON对象
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "datetime", updateData_Type.datetime);
    cJSON_AddNumberToObject(root, "lat", updateData_Type.lat);
    cJSON_AddNumberToObject(root, "lon", updateData_Type.lon);
    cJSON_AddStringToObject(root, "latDir", updateData_Type.latDir);
    cJSON_AddStringToObject(root, "lonDir", updateData_Type.lonDir);
    cJSON_AddNumberToObject(root, "step", updateData_Type.step);
    // 将cJSON对象转化为字符串
    char *str = cJSON_PrintUnformatted(root);
    debug_printfln("str : %s\r\n", str);
    // 释放JSON对象
    cJSON_Delete(root);
    // 4. 发送数据通过NB-IOT给云端
    int_qs100_clientTcp(ip, port, (uint8_t *)str, strlen(str),1);
    // 释放内存空间
    free(str);
}
