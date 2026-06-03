#include "int_DS3553.h"
/**
 * @brief 初始化寄存器
 *
 */
void int_DS3553_Reg_Init(void)
{
    // HAL_StatusTypeDef status;
    // DS3553需要CSB=0才能访问
    DS2553_REG_CS_LOW;
    HAL_Delay(3); // 短暂延时确保信号稳定
    // 寄存器的配置
    uint8_t data = 0x18;
    HAL_I2C_Mem_Write(&hi2c1, DEV_ADDR << 1, DS3553_REG_USER_SET, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000);
    // 恢复CSB=1
    DS2553_REG_CS_HIGH;
    HAL_Delay(10);
}

/**
 * @brief 读取DS3553芯片ID
 * @param id: 存储芯片ID的指针
 * @return HAL_StatusTypeDef: HAL_OK表示成功，其他值表示失败
 */
HAL_StatusTypeDef int_DS3553_ReadChipId(uint8_t *id)
{
    HAL_StatusTypeDef status;
    // DS3553需要CSB=0才能访问
    DS2553_REG_CS_LOW;
    HAL_Delay(3); // 短暂延时确保信号稳定
    // 读取CHIP_ID寄存器(地址0x01)
    status = HAL_I2C_Mem_Read(&hi2c1, DEV_ADDR << 1, DS3553_REG_HCHIP_ID, I2C_MEMADD_SIZE_8BIT, id, 1, 1000);
    // 恢复CSB=1
    DS2553_REG_CS_HIGH;
    HAL_Delay(10);
    return status;
}

/**
 * @brief 通用读取寄存器函数
 * @param id: 存储读取数据的指针
 * @param innerAddr: 寄存器内部地址
 */
void int_DS3553_ReadId(uint8_t *id, uint8_t innerAddr)
{
    HAL_StatusTypeDef status;
    // DS3553需要CSB=0才能访问
    DS2553_REG_CS_LOW;
    HAL_Delay(3);
    // 使用7位设备地址，HAL库会自动处理读写位
    status = HAL_I2C_Mem_Read(&hi2c1, DEV_ADDR << 1, innerAddr, I2C_MEMADD_SIZE_8BIT, id, 1, 1000);
    // 恢复CSB=1
    DS2553_REG_CS_HIGH;
    HAL_Delay(10);
    // 如果读取失败，设置默认值
    if (status != HAL_OK)
    {
        *id = 0x00;
    }
}

uint32_t int_DS3553_getStep(void)
{
    // HAL_StatusTypeDef status;
    // DS3553需要CSB=0才能访问
    // #define DS3553_REG_STEP_CNT_L 0xC4
    // #define DS3553_REG_STEP_CNT_M 0xC5
    // #define DS3553_REG_STEP_CNT_H 0xC6
    uint32_t cnt = 0;
    static uint8_t byte1 = 0;
    static uint8_t byte2 = 0;
    static uint8_t byte3 = 0;
    DS2553_REG_CS_LOW;
    HAL_Delay(3);
    HAL_I2C_Mem_Read(&hi2c1, DEV_ADDR << 1, DS3553_REG_STEP_CNT_L, I2C_MEMADD_SIZE_8BIT, &byte1, 1, 1000);
    HAL_I2C_Mem_Read(&hi2c1, DEV_ADDR << 1, DS3553_REG_STEP_CNT_M, I2C_MEMADD_SIZE_8BIT, &byte2, 1, 1000);
    HAL_I2C_Mem_Read(&hi2c1, DEV_ADDR << 1, DS3553_REG_STEP_CNT_H, I2C_MEMADD_SIZE_8BIT, &byte3, 1, 1000);
    DS2553_REG_CS_HIGH;
    HAL_Delay(10);
    cnt |= (uint32_t)byte1;
    cnt |= (uint32_t)(byte2 << 8);
    cnt |= (uint32_t)(byte3 << 16);
    return cnt;
}
void int_DS3553_ReStop(void)
{
    DS2553_REG_CS_LOW;
    HAL_Delay(3); // 短暂延时确保信号稳定
    // 寄存器的配置
    uint8_t data = (0x18 | 0x04); // 清空计步器
    HAL_I2C_Mem_Write(&hi2c1, DEV_ADDR << 1, DS3553_REG_USER_SET, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000);
    // 恢复CSB=1
    DS2553_REG_CS_HIGH;
    HAL_Delay(10);
}
