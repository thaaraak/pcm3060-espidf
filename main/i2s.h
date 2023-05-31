#ifndef PCM3060_I2S_H_
#define PCM3060_I2S_H_


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
#include "string.h"

esp_err_t i2s_mclk_gpio_select(i2s_port_t i2s_num, gpio_num_t gpio_num);
void i2s_init( int i2s_num, int bits, int sample_rate, int mclk, int bck, int ws, int dout, int din );

#endif
