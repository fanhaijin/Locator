#include "int_LoRa.h"
extern void Ebyte_E220x_SetLoraSyncWord(uint16e_t syncWord);
void int_LoRa_init(void)
{
    // 1. 初始化LoRa
    Ebyte_RF.Init();
    // 2. 设置编号，指定某个设备收发
    Ebyte_E220x_SetLoraSyncWord(0xA5);
}

void int_LoRa_send_data(uint8_t *data, uint8_t len)
{
    Ebyte_RF.Send(data, len, 0);
}
/**
 * @brief 接收数据
 */
void inf_lora_rece_data(void)
{
    Ebyte_RF.StartPollTask();
}
