#ifndef __INT_LORA_H__
#define __INT_LORA_H__

#include "ebyte_core.h"
void int_LoRa_init(void);

void int_LoRa_send_data(uint8_t *data, uint8_t len);
void inf_lora_rece_data(void);
#endif /* __INT_LORA_H__ */
