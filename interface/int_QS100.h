#ifndef __INT_QS100_H__
#define __INT_QS100_H__
#include "string.h"
#include "gpio.h"
#include"usart.h"
#include"com_debug.h"

void int_qs100_init(void);
HAL_StatusTypeDef int_qs100_clientTcp(uint8_t *ip, uint16_t port, uint8_t *data, uint16_t data_len);

// 字符串转十六进制函数
// input: 输入的普通字符串
// output: 输出的十六进制字符串缓冲区（需要足够大，至少 input_len * 2 + 1）
// input_len: 输入字符串的长度
// 返回值: 转换后的十六进制字符串长度
uint16_t str_to_hex(const char *input, char *output, uint16_t input_len);

#endif /* __INT_QS100_H__ */
