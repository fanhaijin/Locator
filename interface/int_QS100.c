#include "int_QS100.h"

// 定义接收缓冲区数组和长度
uint16_t QS_Data_Size = 1024;
uint8_t QS_data[1024] = {0};
uint16_t QS_data_Length = 0;
uint8_t nsocr_val = 100; // 存 socket，链路号，连网成功返回的
uint8_t socket_Id = 100; // 存 socket_id，由创建TCP/SocKet创建的
uint8_t is_Flag = 0;
char output_buffer[1024];
static void int_qs_sendCmd(uint8_t *cmd)
{
    // 每次发送AT命令都会回复消息，所以清空缓冲区数据
    memset(QS_data, 0, QS_Data_Size);
    QS_data_Length = 0;
    // 1. 发送命令到QS100模组中
    HAL_UART_Transmit(&huart3, cmd, (uint16_t)strlen((const char *)cmd), 1000);
    // 2. 接收QS100返回的信息
    uint32_t time_Start = HAL_GetTick();
    // 等待有效数据长度不为0或者超时3s之后就打印接收到的数据
    while ((QS_data_Length == 0) && ((HAL_GetTick() - time_Start) < 3000))
    {
        // 为了让其他任务执行，这里每1s执行一次，在多并发的情况下执行其他任务
        HAL_Delay(1);
    };
    if (QS_data_Length > 0)
    {
        debug_printfln("data: %.*s", QS_data_Length, (char *)QS_data);
    }
    else
    {
        debug_printfln("QS100 Timeout!");
    }
}
/**
 * @brief 初始化QS100
 *
 */
void int_qs100_init(void)
{
    // 1. 上电后延时3s
    HAL_Delay(3000);
    // 2. 启动接收中断服务
    HAL_StatusTypeDef status;
    while (status != HAL_OK)
    {
        status = HAL_UARTEx_ReceiveToIdle_IT(&huart3, QS_data, QS_Data_Size);
    }
    // 3. 发送AT命令
    int_qs_sendCmd("AT\r\n");
}
// 1.请求上网连接
static HAL_StatusTypeDef int_qs100_getNetwork(void)
{
    int_qs_sendCmd("AT+CGATT?\r\n");
    if (strstr((char *)QS_data, "+CGATT"))
    {
        sscanf((char *)QS_data, "%*[^:]:%hhd", &nsocr_val);
        debug_printfln("nsocr_val = %d\r\n",nsocr_val);
        // 缓冲区大小清空
        QS_data_Length = 0;
        return HAL_OK;
    }
    return HAL_ERROR;
}
// 2.创建TCP连接
static HAL_StatusTypeDef int_qs100_createTcp(void)
{
    // 1. 创建TCP Socket
    char cmd[50];
    sprintf((char *)cmd, "AT+NSOCR=STREAM,6,0,%d\r\n", nsocr_val);
    int_qs_sendCmd((uint8_t *)cmd);
    // 2. 创建TCP Socket成功返回
    if (strstr((char *)QS_data, "OK"))
    {
        sscanf((char *)QS_data, "%*[^:]:%hhd", &socket_Id);
        debug_printfln("socket_Id = %d\r\n",socket_Id);
        // 缓冲区大小清空
        QS_data_Length = 0;
        return HAL_OK;
    }
    return HAL_ERROR;
}
// 3.连接TCP服务器
static HAL_StatusTypeDef int_qs100_connectTcp(uint8_t *ip, uint16_t port)
{

    char cmd[50];
    sprintf(cmd, "AT+NSOCO=%d,%s,%d\r\n", socket_Id, ip, port);
    int_qs_sendCmd((uint8_t *)cmd);
    if (strstr((char *)QS_data, "OK"))
    {
        return HAL_OK;
    }
    return HAL_ERROR;
}
// 4.发送TCP数据
static HAL_StatusTypeDef int_qs100_sendTcp(uint8_t *data, uint16_t data_len)
{
    // 把字符串转化为16进制
    for (int i = 0; i < data_len; i++)
    {
        sprintf(&output_buffer[i * 2], "%02X", data[i]);
    }
    char cmd[512];
    sprintf(cmd, "AT+NSOSD=%d,%d,%s\r\n", socket_Id, data_len, output_buffer);
    int_qs_sendCmd((uint8_t *)cmd);
    if (strstr((char *)QS_data, "OK"))
    {
        return HAL_OK;
    }
    return HAL_ERROR;
}
// 5.关闭Socket
static HAL_StatusTypeDef int_qs100_endSocket(void)
{
    char endSocket_Cmd[50];
    sprintf(endSocket_Cmd, "AT+NSOCL=%d\r\n", socket_Id);
    int_qs_sendCmd((uint8_t *)endSocket_Cmd);
    if (strstr((char *)QS_data, "OK"))
    {
        return HAL_OK;
    }
    return HAL_ERROR;
}
HAL_StatusTypeDef int_qs100_clientTcp(uint8_t *ip, uint16_t port, uint8_t *data, uint16_t data_len)
{
    // 1. 请求上网信息
    uint8_t cnt = 10;
    // 等待是否上网成功
    while (int_qs100_getNetwork() != HAL_OK && cnt--)
    {
        HAL_Delay(1000);
    };
    if (cnt == 0)
    {
        debug_printfln("QS100 No Network timeout!\r\n");
        return;
    }
    debug_printfln("QS100 Network OK!\r\n");
    // 2. 创建TCP连接
    if (int_qs100_createTcp() != HAL_OK)
    {
        debug_printfln("QS100 Create TCP timeout!\r\n");
        return;
    }
    debug_printfln("QS100 Create TCP OK!\r\n");
    // 3.连接到TCP服务器
    cnt = 10;
    while (int_qs100_connectTcp(ip, port) != HAL_OK && cnt--)
    {
        HAL_Delay(1000);
    };
    if (cnt == 0)
    {
        debug_printfln("QS100 Connect TCP timeout!\r\n");
        return;
    }
    debug_printfln("QS100 Connect TCP OK!\r\n");
    // 4. 发送TCP数据
    if (int_qs100_sendTcp(data, data_len) != HAL_OK)
    {
        debug_printfln("QS100 Send Data timeout!\r\n");
        return;
    }
    debug_printfln("QS100 Send Data OK!\r\n");
    // 5. 关闭TCP Socket
    cnt = 10;
    while (int_qs100_endSocket() != HAL_OK && cnt--)
    {
        HAL_Delay(1000);
    };
    return HAL_OK;
}
