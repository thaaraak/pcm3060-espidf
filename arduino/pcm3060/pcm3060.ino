#include <Wire.h>
#include "AudioTools.h"

typedef int32_t sound_t;                                   // sound will be represented as int16_t (with 2 bytes)
uint16_t sample_rate=44100;
uint8_t channels = 2;                                      // The stream will have 2 channels 
SineWaveGenerator<sound_t> sineWave(2000000000);                // subclass of SoundGenerator with max amplitude of 32000
//SquareWaveGenerator<sound_t> sineWave(2000000000);                // subclass of SoundGenerator with max amplitude of 32000
GeneratedSoundStream<sound_t> sound(sineWave);             // Stream generated from sine wave
I2SStream out; 
StreamCopy copier(out, sound);                             // copies sound into i2s

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


void setupPCM3060()
{
    uint8_t val;

    Serial.println( "Connected to PCM3060" );

    delay(500);
    writePCMRegister( 64, 0b11000000 );
    delay(500);
    writePCMRegister( 67, 0b10000000 );

     for ( int i = 64 ; i <= 73 ; i++ )
      val = readPCMRegister( i );   

}

#define RESET_PIN 17

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
  auto config = out.defaultConfig(RXTX_MODE);
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
  out.begin(config);

  sineWave.begin(channels, sample_rate, 2000);
  Serial.println("started...");
}

void loop() 
{
  copier.copy();
}


  
