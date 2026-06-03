#ifndef __INT_GPS_H__
#define __INT_GPS_H__
#include "stm32f103xb.h"
#include "gpio.h"
#include"usart.h"
#include "string.h"
/**
 * @brief 初始化
 *
 */
void int_GPS_Init(void);

/**
 * @brief 读取卫星数据
 *
 * @param data
 * @param size
 */
void int_GPS_ReadData(uint8_t *data, uint16_t *size);
/**
 * @brief 写入命令
 *
 * @param cmd
 * @param size
 */
void int_GPS_WriteCmd(uint8_t *cmd, uint8_t size);
void int_gps_Rx_Callback(uint16_t size);
#endif /* __INT_GPS_H__ */
