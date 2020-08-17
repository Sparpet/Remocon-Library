
#define _CONTROLLER
#include <Remocon.h>
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
///////////////declare instances of objects and structs//////////////
RH_RF69 rf69(PB12,PA8);
Adafruit_SSD1306 display(OLED_RESET);
RECEIVER telemetry;
CONTROLLER inputs;
STM32ADC input(ADC1);

uint8_t len=sizeof(inputs);
uint8_t len_telemetry=sizeof(telemetry);
float f=0;
////////////////Part of DMA setup//////////////////
 uint8 pins[] = {pinLStick_X ,pinLStick_Y,pinRStick_X,pinRStick_Y,pinLeftSlider,pinCenterSlider,pinRightSlider,pinRightPot,pinLeftPot,pinConector};
 const int maxSamples = 10; //  10 channels 
 uint16_t dataPoints[maxSamples];
    

void package_all();

void setup() { 
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  pinMode(pinLStick_X,INPUT);
  pinMode(pinRStick_X,INPUT);
  pinMode(pinLStick_Y,INPUT);
  pinMode(pinRStick_Y,INPUT);
  pinMode(pinLeftSlider,INPUT);
  pinMode(pinRightSlider,INPUT);
  pinMode(pinCenterSlider,INPUT);
  pinMode(pinCenterSlider,INPUT);
  pinMode(pinRightPot,INPUT);
  pinMode(pinLeftPot,INPUT);
  pinMode(pinUp,INPUT);
  pinMode(pinDown,INPUT);
  pinMode(pinLeft,INPUT);
  pinMode(pinRight,INPUT);
  pinMode(pinA,INPUT);
  pinMode(pinB,INPUT);
  Serial.print("pass");
  //////////////initialize display////////////
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.drawBitmap(0, 0, splash, 128, 64, 1); //splash screen
  display.display();
  delay(300);
  display.clearDisplay();
  
  Serial.begin(9600);//not including this serial can cause stm32 to hang if you try to use Serial
  
  while (!rf69.init()){
   display.println("init failed");
   display.display();
  }


  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module),No encryption 
   if (!rf69.setFrequency(915.0)){
    display.println("setFrequency failed");
    display.display();
   }
  rf69.setTxPower(14, true);//required by RFM69HW

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);



///////////////DMA_SETUP/////////////////////


  input.calibrate();
  input.setSampleRate(ADC_SMPR_1_5);//set the Sample Rate
  input.setScanMode();              //set the ADC in Scan mode. 
  input.setPins(pins, 10);           //set how many and which pins to convert.
  input.setContinuous();            //set the ADC in continuous mode.
  input.setDMA(dataPoints, 10, (DMA_MINC_MODE | DMA_CIRC_MODE), NULL);
  input.startConversion();

  delay(500);
}


void loop()
{ 
  
  display.clearDisplay();
  package_all(); //load all inputs into struct

  if(!inputs.Up)
    display.fillCircle(20, 37, 2, WHITE);
  if(!inputs.Down)
    display.fillCircle(20, 48, 2, WHITE);
  if(!inputs.Left)
    display.fillCircle(14, 43, 2, WHITE);
  if(!inputs.Right)
    display.fillCircle(26, 43, 2, WHITE);

    
  ///////////draw bars
  display.fillRect(122,inputs.RightSlider,5,64 ,WHITE);
  display.fillRect(1,inputs.LeftSlider,5,64,WHITE);
  display.fillRect(7,59,inputs.CenterSlider,5,WHITE);
  
  ///////draw squares and a pixel corresponding to sticks
  display.drawRect(7,1,15,15,WHITE);
  display.drawRect(105,1,15,15,WHITE);
  display.drawPixel(14+map(inputs.LStick_X,0,4095,-6,6),8+map(inputs.LStick_Y,0,4095,6,-6), WHITE);
  display.drawPixel(112+map(inputs.RStick_X,0,4095,-6,6),8+map(inputs.RStick_Y,0,4095,6,-6), WHITE);

  ///////draw lines corresponding to the pot value  
  f=0;
  while(inputs.LeftPot>=f){
    display.drawLine(32,32,32+(cos(f)*7),32+(sin(f)*7),WHITE);
    f=f+0.1;
  }
  f=0;
  while(inputs.RightPot>=f){
    display.drawLine(96,32,96+(cos(f)*7),32+(sin(f)*7),WHITE);
    f=f+0.1;
  }

    /////send all inputs and wait to finish
    rf69.send((uint8_t*)&inputs, len);
    rf69.waitPacketSent();

    /////wait a little for telemetry data
    if (rf69.waitAvailableTimeout(1))
  {   
    if (rf69.recv((uint8_t*)&telemetry, &len_telemetry)){
      Serial.print(telemetry.internalTemp);
      Serial.print(",");
      Serial.println(telemetry.vdd);
    }
    else
    {
      Serial.println("recv failed");
    }
  }
  else
  {
    Serial.println("No reply, is rf69_server running?");
  }

  display.display();

}

void package_all(){
  inputs.LStick_X=dataPoints[0];
  inputs.LStick_Y=dataPoints[1];
  inputs.RStick_X=dataPoints[2];
  inputs.RStick_Y=dataPoints[3];
  
  inputs.LeftSlider=map(dataPoints[4],0,4095,63,1);
  inputs.CenterSlider=map(dataPoints[5],0,4095,1,114);
  inputs.RightSlider=map(dataPoints[6],0,4095,63,1);
  
  inputs.RightPot=mapfloat(dataPoints[7],0,4095,6.285,0);
  inputs.LeftPot=mapfloat(dataPoints[8],0,4095,0,6.285);
  inputs.Conector=dataPoints[9];

  inputs.Up=digitalRead(pinUp);
  inputs.Down=digitalRead(pinDown);
  inputs.Left=digitalRead(pinLeft);
  inputs.Right=digitalRead(pinRight);
  inputs.A=digitalRead(pinA);
  inputs.B=digitalRead(pinB);
}
