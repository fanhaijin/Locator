#ifndef __INT_DS3553_H__
#define __INT_DS3553_H__

#include"i2c.h"
// 假设hi2c1在其他地方定义，这里声明为外部变量
extern I2C_HandleTypeDef hi2c1;

#define DEV_ADDR 0x27
#define WRITE_ADDR 0x4E
#define READ_ADDR 0x4F
#define DS3553_REG_USER_SET 0xC3 // 用户配置（读写，默认值 0x18）
#define DS3553_REG_STEP_CNT_L 0xC4
#define DS3553_REG_STEP_CNT_M 0xC5
#define DS3553_REG_STEP_CNT_H 0xC6
#define DS3553_REG_HCHIP_ID 0x01
#define DS2553_REG_CS_LOW (HAL_GPIO_WritePin(GPIOB, DS3553_CS_Pin, GPIO_PIN_RESET))
#define DS2553_REG_CS_HIGH (HAL_GPIO_WritePin(GPIOB, DS3553_CS_Pin, GPIO_PIN_SET))
void int_DS3553_Reg_Init(void);
void int_DS3553_ReadId(uint8_t *id, uint8_t innerAddr);
uint32_t int_DS3553_getStep(void);
HAL_StatusTypeDef int_DS3553_ReadChipId(uint8_t *id);
void int_DS3553_ReStop(void);
#endif /* __INT_DS3553_H__ */
