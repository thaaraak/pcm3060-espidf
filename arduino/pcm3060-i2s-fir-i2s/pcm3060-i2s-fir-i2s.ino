/**
 * @file streams-i2s-filter-i2s.ino
 * @brief Copy audio from I2S to I2S using an FIR filter
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

#include "AudioTools.h"
#include "Wire.h"
#include "FIRConverter.h"

float coeffs_60plus45[60] =
{
     0.004621015177093199,
     0.005803076057134736,
     0.008398848534014810,
     0.011070605501422921,
     0.012011544037449230,
     0.010212483927766067,
     0.006468230078269678,
     0.003276685410765598,
     0.003421402894455017,
     0.007976926394432645,
     0.015086517251198047,
     0.020602093602433549,
     0.020553118230563584,
     0.014069048704528873,
     0.004749834871128875,
    -0.000842860122019913,
     0.002859371099914513,
     0.016274028687678372,
     0.033160109747664451,
     0.043383049674853250,
     0.039014316921656009,
     0.020423745832596006,
    -0.001614427587024464,
    -0.009963364302439288,
     0.009994589273862787,
     0.061109944842178014,
     0.130195753590080343,
     0.191869213030050390,
     0.219369776452719017,
     0.197227986103014563,
     0.129280688421556683,
     0.037690449757477727,
    -0.046755270382402625,
    -0.098240151525143468,
    -0.106623328365524572,
    -0.080149105724243566,
    -0.039387167980812265,
    -0.006148067870983963,
     0.006632322465463789,
    -0.000859139466368104,
    -0.018244181532151575,
    -0.032471581746266860,
    -0.035274696600922327,
    -0.026638667871024192,
    -0.013113047257170066,
    -0.002815732303918569,
    -0.000505099571022141,
    -0.005489290956139294,
    -0.013054185024661442,
    -0.017950908774627395,
    -0.017543003217475647,
    -0.012834680480806963,
    -0.007166192548587818,
    -0.003783981742332182,
    -0.003943651197459626,
    -0.006576238628670079,
    -0.009410544263128996,
    -0.010581789452050234,
    -0.009650968000300929,
    -0.007537225770687841
};

float coeffs_60minus45[60] =
{
    -0.007537225770687170,
    -0.009650968000300499,
    -0.010581789452050476,
    -0.009410544263129985,
    -0.006576238628671330,
    -0.003943651197460286,
    -0.003783981742331553,
    -0.007166192548585905,
    -0.012834680480804665,
    -0.017543003217474328,
    -0.017950908774628013,
    -0.013054185024663774,
    -0.005489290956141837,
    -0.000505099571022898,
    -0.002815732303916365,
    -0.013113047257165571,
    -0.026638667871019869,
    -0.035274696600921071,
    -0.032471581746270073,
    -0.018244181532157757,
    -0.000859139466373159,
     0.006632322465464511,
    -0.006148067870975227,
    -0.039387167980797798,
    -0.080149105724229994,
    -0.106623328365520187,
    -0.098240151525153974,
    -0.046755270382427966,
     0.037690449757444351,
     0.129280688421526124,
     0.197227986102997105,
     0.219369776452719767,
     0.191869213030067154,
     0.130195753590105073,
     0.061109944842200607,
     0.009994589273875859,
    -0.009963364302437662,
    -0.001614427587030836,
     0.020423745832587811,
     0.039014316921651394,
     0.043383049674854284,
     0.033160109747669607,
     0.016274028687684093,
     0.002859371099917639,
    -0.000842860122020447,
     0.004749834871125916,
     0.014069048704525876,
     0.020553118230562491,
     0.020602093602434816,
     0.015086517251200629,
     0.007976926394434900,
     0.003421402894455769,
     0.003276685410764736,
     0.006468230078268022,
     0.010212483927764715,
     0.012011544037448875,
     0.011070605501423535,
     0.008398848534015839,
     0.005803076057135559,
     0.004621015177093499

};



uint16_t sample_rate=48000;
uint16_t channels = 2;
I2SStream in;
StreamCopy copier(in, in); 
FIRConverter<int32_t> *fir;

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

  fir = new FIRConverter<int32_t>( (float*)&coeffs_60minus45, (float*)&coeffs_60plus45, 60 );

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
  copier.copy( *fir );
}


  
