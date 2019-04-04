#include "SPI.h"
#include "Adafruit_GFX.h"
#include "WROVER_KIT_LCD.h"
#include "BluetoothSerial.h"

// hardware LEDs            
const int NUM_LEDS = 3;
const int LEDs[NUM_LEDS] =   {4, 0, 2};
int toggle[NUM_LEDS] =       {0, 0, 0};

//LCD globals
const uint16_t colors[NUM_LEDS] = {WROVER_DARKCYAN, WROVER_MAROON, WROVER_DARKGREEN};
const uint16_t background = WROVER_DARKGREY;
const uint16_t outline = WROVER_BLACK;
const uint16_t textcolor = WROVER_BLACK;
const int WIDTH = 320;
const int HEIGHT = 240;
int numChars = 0;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

WROVER_KIT_LCD tft;
BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32");
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(background); 
  tft.setTextSize(2);
  tft.setTextColor(textcolor);
  tft.fillRect(0, (HEIGHT/2), WIDTH, 10, outline);
  tft.setCursor(20, 160);
  tft.print("SERIAL BLUETOOTH VALUE:");
  for(int i = 0; i < NUM_LEDS; i++)
  {
    pinMode(LEDs[i], OUTPUT);
    digitalWrite(LEDs[i], HIGH);
    drawLED(i, HIGH);
  }
}

void loop(void) {
  int in;
  
  if (SerialBT.available()) {
    in = SerialBT.read();
    if(in != 13 && in != 10) handle(in);
    if(in == 10) 
    {
      delay(200);
      clrText();
    }
  }
  
  delay(20);
}

void handle(int in)
{
    static const int ascii2int = 49;
    printBLE(in);
    in-=ascii2int;
    digitalWrite(LEDs[in], toggle[in]);
    drawLED(in, toggle[in]); 
    //exor to flip toggle between 0 and 1
    toggle[in] ^= 1;
}

void drawLED(int in, int toggle)
{
  static const int w = 107;
  static const int h = HEIGHT / 2;
  static const int y = 0;
  int x = w*in;
  uint16_t color;
  if (toggle == 1) color = colors[in];
  else color = WROVER_LIGHTGREY;
  tft.fillRect(x, 0, w, h, color);
}

void printBLE(int in)
{
  static const int margin = 30;
  static const int y_pos = 200;
  String out = (String) in;
  int curserPos = margin+(margin*numChars);
  if(curserPos < (WIDTH-margin))
  {
    tft.setCursor(curserPos, y_pos);
    tft.print(out);
  }
  numChars++;
}

void clrText()
{
  static const int y_pos = 180;
  static const int height = 50;
  tft.fillRect(0, y_pos, WIDTH, height, background);
  numChars = 0; 
}
