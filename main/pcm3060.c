/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
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

#include "i2c.h"
#include "i2s.h"

#define SAMPLE_RATE     (44100)
#define I2S_NUM         (0)
#define WAVE_FREQ_HZ    (1000)
#define PI              (3.14159265)


#define I2S_MCLK_PIN	(GPIO_NUM_0)
#define I2S_BCK_IO      (GPIO_NUM_4)
#define I2S_WS_IO       (GPIO_NUM_16)
#define I2S_DO_IO       (GPIO_NUM_15)
#define I2S_DI_IO       (GPIO_NUM_5)


#define I2C_MASTER_SDA_IO 22
#define I2C_MASTER_SCL_IO 23

#define SAMPLE_PER_CYCLE (SAMPLE_RATE/WAVE_FREQ_HZ)

#define	BUF_SAMPLES		SAMPLE_PER_CYCLE * 2			// Size of DMA tx/rx buffer samples * left/right * 2 for 32 bit samples

// DMA Buffers
int32_t rxBuf[BUF_SAMPLES];
int32_t txBuf[BUF_SAMPLES];

int16_t rxBuf16[BUF_SAMPLES];
int16_t txBuf16[BUF_SAMPLES];

static int cnt = 0;

static void setup_sine_waves( int amplitude )
{
	double sin_float;

    for( int pos = 0; pos < BUF_SAMPLES; pos += 2 )
    {
        sin_float = amplitude * sin( pos/2 * 2 * PI / SAMPLE_PER_CYCLE);

        int32_t lval = sin_float;
        int32_t rval = sin_float;

        txBuf[pos] = lval << 8;
        txBuf[pos+1] = rval << 8;

    }

    /*
    if ( cnt++ == 10000 )
    {
    	cnt = 0;
    	for( int pos = 0; pos < BUF_SAMPLES; pos += 2 )
            printf( "%04x:%04x\n", txBuf[pos],txBuf[pos+1] );
    }
    */
}

static void setup_sine_waves16( int amplitude )
{
	double sin_float;


    for( int pos = 0; pos < BUF_SAMPLES; pos += 2 )
    {
        sin_float = amplitude * sin( pos/2 * 2 * PI / SAMPLE_PER_CYCLE);

        int16_t lval = sin_float;
        int16_t rval = sin_float;

        txBuf16[pos] = lval;
        txBuf16[pos+1] = rval;

    }

    /*
    if ( cnt++ == 10000 )
    {
    	cnt = 0;
    	for( int pos = 0; pos < BUF_SAMPLES; pos += 2 )
            printf( "%04d:%04d\n", txBuf16[pos],txBuf16[pos+1] );
    }
    */
}



#define RESET_GPIO 17

void do24bit();
void do16bit();

void app_main(void)
{
	do24bit();
	//do16bit();
}

void do24bit()
{
    printf("Initializing I2S - 24 bit\n");
    i2s_init( I2S_NUM, I2S_BITS_PER_SAMPLE_32BIT, SAMPLE_RATE, I2S_MCLK_PIN, I2S_BCK_IO, I2S_WS_IO, I2S_DO_IO, I2S_DI_IO );

    gpio_reset_pin(RESET_GPIO);
    gpio_set_direction(RESET_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(RESET_GPIO, 1);

    printf("Initializing I2C\n");
    i2c_master_init( 0, 23, 22 );

    printf("Resetting PCM - power ADC/DAC\n");
    i2c_write( 0x46, 64, 0b11000000 );

    // Set LRCLK1, BCK1, SCK1 for DAC
    // Set I2S Slave mode for DAC
    // Set 24 bit I2S for DAC
    i2c_write( 0x46, 67, 0b10000000);

    uint8_t val;
    val = i2c_read( 0x46, 64 );
    val = i2c_read( 0x46, 67 );

    size_t i2s_bytes_write = 0;

    int amplitude = 1000000;
    int start_dir = 500;
    int dir = start_dir;

    while (1)
    {
    	amplitude += dir;
    	if ( amplitude < 1000000 || amplitude > 3500000 )
    		dir *= -1;

    	setup_sine_waves( amplitude );


    	i2s_write(I2S_NUM, txBuf, BUF_SAMPLES*4, &i2s_bytes_write, -1);

    	//printf( "Bytes: %d\n", i2s_bytes_write );
    }
}


void do16bit()
{
    printf("Initializing I2S - 16 bit\n");
    i2s_init( I2S_NUM, I2S_BITS_PER_SAMPLE_16BIT, SAMPLE_RATE, I2S_MCLK_PIN, I2S_BCK_IO, I2S_WS_IO, I2S_DO_IO, I2S_DI_IO );

    gpio_reset_pin(RESET_GPIO);
    gpio_set_direction(RESET_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(RESET_GPIO, 1);

    printf("Initializing I2C\n");
    i2c_master_init( 0, 23, 22 );

    printf("Resetting PCM - power ADC/DAC\n");
    i2c_write( 0x46, 64, 0b11000000 );

    // Set LRCLK1, BCK1, SCK1 for DAC
    // Set I2S Slave mode for DAC
    // Set 16 bit I2S for DAC
    i2c_write( 0x46, 67, 0b10000011);

    uint8_t val;
    val = i2c_read( 0x46, 64 );
    val = i2c_read( 0x46, 67 );

    size_t i2s_bytes_write = 0;

    int amplitude = 20000;
    int start_dir = 100;
    int dir = start_dir;

    while (1)
    {
    	amplitude += dir;
    	if ( amplitude < 5000 || amplitude > 28000 )
    		dir *= -1;

    	setup_sine_waves16( amplitude );

    	i2s_write(I2S_NUM, txBuf16, BUF_SAMPLES*2, &i2s_bytes_write, -1);

    	//printf( "Bytes: %d\n", i2s_bytes_write );
    }
}
