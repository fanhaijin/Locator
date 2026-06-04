#ifndef __INT_QS100_H__
#define __INT_QS100_H__
#include "string.h"
#include "gpio.h"
#include"usart.h"
#include"com_debug.h"

void int_qs100_init(void);
HAL_StatusTypeDef int_qs100_send_server(uint8_t *ip, uint16_t port, uint8_t *data, uint16_t data_len,uint8_t sequence);

#endif /* __INT_QS100_H__ */
