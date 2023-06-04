/**
 * @file streams-i2s-filter-i2s.ino
 * @brief Copy audio from I2S to I2S using an FIR filter
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

#include "AudioTools.h"
#include "Wire.h"

uint16_t sample_rate=48000;
uint16_t channels = 2;
I2SStream in;
StreamCopy copier(in, in); 

TwoWire wire(0);

int i2c_bus_addr = 0x46;

uint8_t readPCMRegister(uint8_t addr)
{
  char buf[50];
  uint8_t reg_val = 0;

  wire.beginTransmission(i2c_bus_addr);
  wire.write(addr);
  wire.endTransmission();

  wire.requestFrom(i2c_bus_addr, (uint8_t)1, (uint8_t)false);

  while(wire.available())
  {
    reg_val = wire.read();
  }

  sprintf( buf, "Read: [%d] = %d", addr, reg_val );
  Serial.println( buf );

  return reg_val;
}


uint8_t writePCMRegister(uint8_t addr, uint8_t data)
{
  wire.beginTransmission(i2c_bus_addr);
  wire.write(addr);
  wire.write(data);
  return wire.endTransmission();
}

#define RESET_PIN 17

void setupPCM3060()
{
    uint8_t val;

    Serial.println( "Connected to PCM3060" );

    digitalWrite(RESET_PIN, LOW); 
    writePCMRegister( 64, 0b11110000 );
    delay(50);
    writePCMRegister( 64, 0b11000000 );
    digitalWrite(RESET_PIN, HIGH); 

    writePCMRegister( 67, 0b10000000 );
    writePCMRegister( 72, 0b00000000 );

    writePCMRegister( 70, 0b11010111 );
    writePCMRegister( 71, 0b11010111 );

    writePCMRegister( 65, 0b11111111 );
    writePCMRegister( 66, 0b11111111 );

}

void setup()
{
  char  buf[50];
  
  Serial.begin(115200);
  while (!Serial); 
  Serial.println("\nPCM3060 Test");

  pinMode(RESET_PIN, OUTPUT);    
  digitalWrite(RESET_PIN, HIGH); 
  
  wire.setPins(22, 23 );
  wire.begin();
  wire.beginTransmission(i2c_bus_addr);
  uint8_t error = wire.endTransmission();

  if ( error == 0 )
  {
    setupPCM3060();
  }
  else
  {
    sprintf( buf, "Error: %d connecting to PCM3060" );
    Serial.println( buf );
  }


    // start I2S
  Serial.println("starting I2S...");
  auto config = in.defaultConfig(RXTX_MODE);
  config.sample_rate = sample_rate;
  config.bits_per_sample = 32;
  config.i2s_format = I2S_STD_FORMAT;
  config.is_master = true;
  config.port_no = 0;
  config.pin_ws = 16;
  config.pin_bck = 4;
  config.pin_data = 15;
  config.pin_data_rx = 5;
  config.pin_mck = 0;
  config.use_apll = true;  
  in.begin(config);

  Serial.println("started...");
}

void loop() 
{
  copier.copy();
}


  
