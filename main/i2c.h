#ifndef PCM3060_I2C_H_
#define PCM3060_I2C_H_


#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "driver/i2s.h"
#include "soc/gpio_sig_map.h"
#include "math.h"


#define I2C_MASTER_NUM	0
#define ACK_CHECK_EN 0x1
#define ACK_CHECK_DIS 0x0
#define ACK_VAL I2C_MASTER_ACK
#define NACK_VAL I2C_MASTER_NACK

uint8_t i2c_write_bulk( uint8_t i2c_bus_addr, uint8_t reg, uint8_t bytes, uint8_t *data);
uint8_t i2c_write( uint8_t i2c_bus_addr, uint8_t reg, uint8_t value);
uint8_t i2c_read( uint8_t i2c_bus_addr, uint8_t reg);
esp_err_t i2c_master_init( int i2c_master_num, int scl, int sda );

#endif
