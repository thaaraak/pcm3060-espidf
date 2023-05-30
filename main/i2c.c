#include "i2c.h"

int i2c_master_port;

uint8_t i2c_write_bulk( uint8_t i2c_bus_addr, uint8_t reg, uint8_t bytes, uint8_t *data)
{
	/*
    printf( "Writing [%02x]=", reg );
    for ( int i = 0 ; i < bytes ; i++ )
        printf( "%02x:", data[i] );
    printf( "\n");
*/

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, i2c_bus_addr << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write(cmd, &reg, 1, ACK_CHECK_EN);
    i2c_master_write(cmd, data, bytes, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    int ret = i2c_master_cmd_begin( i2c_master_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        return ret;
    }

    return 0;
}

uint8_t i2c_write( uint8_t i2c_bus_addr, uint8_t reg, uint8_t value)
{
    return i2c_write_bulk( i2c_bus_addr, reg, 1, &value );
}



uint8_t i2c_read( uint8_t i2c_bus_addr, uint8_t reg)
{
	   	uint8_t buffer[2];
	    //printf( "Addr: [%d] Reading register: [%d]\n", i2c_bus_addr, reg );

	    buffer[0] = reg;

	    int ret;
	    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	    // Write the register address to be read
	    i2c_master_start(cmd);
	    i2c_master_write_byte(cmd, i2c_bus_addr << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
	    i2c_master_write_byte(cmd, buffer[0], ACK_CHECK_EN);

	    // Read the data for the register from the slave
	    i2c_master_start(cmd);
	    i2c_master_write_byte(cmd, i2c_bus_addr << 1 | I2C_MASTER_READ, ACK_CHECK_EN);
	    i2c_master_read_byte(cmd, &buffer[0], NACK_VAL);
	    i2c_master_stop(cmd);

	    ret = i2c_master_cmd_begin( i2c_master_port, cmd, 1000 / portTICK_RATE_MS);
	    i2c_cmd_link_delete(cmd);

	    printf( "Read: [%02x]=[%02x]\n", reg, buffer[0] );

	    return (buffer[0]);
}

esp_err_t i2c_master_init( int id, int scl, int sda )
{
    i2c_master_port = id;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = sda;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = scl;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}
