
#include <RH_RF69.h>
#include <Servo.h>
// Singleton instance of the radio driver
struct CONTROLLER{
  uint16_t LStick_X;
  uint16_t LStick_Y;
  uint16_t RStick_X;
  uint16_t RStick_Y;
  uint16_t LeftSlider;
  uint16_t CenterSlider;
  uint16_t RightSlider;
  uint16_t RightPot;
  uint16_t LeftPot;
  uint16_t Conector;
  bool LeftButton;
  bool RightButton;
  bool Up;
  bool Down;
  bool Left;
  bool Right;
  bool A;
  bool B;
};

struct RECEIVER{
  float internalTemp;
  float vdd;
};



void setup_temperature_sensor() {
  adc_reg_map *regs = ADC1->regs;

// 3. Set the TSVREFE bit in the ADC control register 2 (ADC_CR2) to wake up the
//    temperature sensor from power down mode.  Do this first 'cause according to
//    the Datasheet section 5.3.21 it takes from 4 to 10 uS to power up the sensor.

  regs->CR2 |= ADC_CR2_TSVREFE;

// 2. Select a sample time of 17.1 μs
// set channel 16 sample time to 239.5 cycles
// 239.5 cycles of the ADC clock (72MHz/6=12MHz) is over 17.1us (about 20us), but no smaller
// sample time exceeds 17.1us.

  regs->SMPR1 |=  (0b111 << 18);// set channel 16, the temp. sensor
  regs->SMPR1 |=  (0b111 << 21); //VDD
    adc_calibrate(ADC1);
}

Servo miservo;
RH_RF69 rf69(PB12,PA8);
CONTROLLER inputs;
RECEIVER telemetry;

uint8_t len = sizeof(inputs);
uint8_t len_tele=sizeof(telemetry);

void setup() 
{ 
  pinMode(PB1,OUTPUT);
  pinMode(PC13,OUTPUT); 
  Serial.begin(9600);

  while (!rf69.init())
  Serial.println("init failed");

  miservo.attach(PB1);
    
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(915.0))
    Serial.println("setFrequency failed");

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rf69.setTxPower(14, true);

  // The encryption key has to be the same as the one in the client
 uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);

  Serial.println("ready");
  setup_temperature_sensor();
  delay(100);
}

void loop()
{ 
  telemetry.vdd = 1.20*4096.0/adc_read(ADC1, 17);
  telemetry.internalTemp=(1.43-(telemetry.vdd/4096.0*adc_read(ADC1,16)))/0.0043+25.0; //"1.43 and 0.0043" parameters come from F103 datasheet - ch. 5.9.13
 if (rf69.available())
 {
  

    if (rf69.recv((uint8_t*)&inputs, &len))
    {
      Serial.println(inputs.B);
      miservo.write(map(inputs.CenterSlider,0,4095,0,180));
      digitalWrite(PC13,inputs.B);
      rf69.send((uint8_t*)&telemetry, len_tele);
      rf69.waitPacketSent();
    }
    
  }
  
}
