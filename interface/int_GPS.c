#include "int_GPS.h"

// GPS的接收缓冲区数据
#define GPS_Rx_Data_Size 1024
static uint8_t GPS_Rx_Data[GPS_Rx_Data_Size];
static uint16_t GPS_Rx_Data_Length = 0;
/**
 * @brief 初始化
 *
 */
void int_GPS_Init(void)
{
    // 1. 上电（拉高 CS/ON_OFF 引脚）
    HAL_GPIO_WritePin(GPIOB, GPS_CS_Pin, GPIO_PIN_SET);
    HAL_Delay(200); // 等待芯片稳定
    // 2. 开启接收的中断服务成程序
    HAL_StatusTypeDef status;
    while (status != HAL_OK)
    {
        status = HAL_UARTEx_ReceiveToIdle_IT(&huart2, GPS_Rx_Data, GPS_Rx_Data_Size);
    }
    // 3. 定义命令（使用 char[] 避免类型转换麻烦）
    char cmd_rate[] = "$PCAS02,1000*2E\r\n";
    char cmd_mode[] = "$PCAS04,3*1A\r\n";
    // 4. 设置定位更新率（1Hz）
    int_GPS_WriteCmd((uint8_t *)cmd_rate, sizeof(cmd_rate) - 1);

    // 5. 设置北斗+GPS双模
    int_GPS_WriteCmd((uint8_t *)cmd_mode, sizeof(cmd_mode) - 1);
}

/**
 * @brief 读取卫星数据
 *
 * @param data
 * @param size
 */
void int_GPS_ReadData(uint8_t *data, uint16_t *size)
{
    // 1. 先清空data里面的数据
    memset(data, 0, strlen((char *)data));
    *size = 0;
    // 程序中错误优先处理
    if (GPS_Rx_Data_Length == 0)
    {
        return;
    }
    // 2. 数据拷贝
    // 拷贝数据到data
    memcpy(data, GPS_Rx_Data, GPS_Rx_Data_Length);
    *size = GPS_Rx_Data_Length;
    // 3. 清空接收缓冲区数据
    memset(GPS_Rx_Data, 0, GPS_Rx_Data_Length);
    GPS_Rx_Data_Length = 0;
}
/**
 * @brief 写入命令
 *
 * @param cmd
 * @param size
 */
void int_GPS_WriteCmd(uint8_t *cmd, uint8_t size)
{
    // 发送命令给GPS模组
    HAL_UART_Transmit(&huart2, cmd, size, 1000);
}
/**
 * @brief 处理GPS的中断服务程序的函数
 *
 * @param size
 */
void int_gps_Rx_Callback(uint16_t size)
{
    GPS_Rx_Data_Length = size;
    HAL_StatusTypeDef status;
    while (status != HAL_OK)
    {
        status = HAL_UARTEx_ReceiveToIdle_IT(&huart2, GPS_Rx_Data, GPS_Rx_Data_Size);
    }
}
