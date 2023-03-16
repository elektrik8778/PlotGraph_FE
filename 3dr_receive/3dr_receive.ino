#include <Adafruit_ADS1X15.h>
#include <Wire.h>
//#include <LiquidCrystal_I2C.h>
#include <Adafruit_MCP4725.h>
#include <SoftwareSerial.h>
Adafruit_ADS1115 ads;


// Set this value to 9, 8, 7, 6 or 5 to adjust the resolution
#define DAC_RESOLUTION    (9) //DAC resolution 12BIT: 0 to 4056

SoftwareSerial tlmSerial(10, 11); // RX, TX

//LiquidCrystal_I2C lcd(0x3f,16,2);
Adafruit_MCP4725 dac;



bool stream = false;
uint16_t i;
const PROGMEM uint16_t DACLookup_FullSine_5Bit[32] =
{
  2048, 2447, 2831, 3185, 3495, 3750, 3939, 4056,
  4095, 4056, 3939, 3750, 3495, 3185, 2831, 2447,
  2048, 1648, 1264,  910,  600,  345,  156,   39,
  0,   39,  156,  345,  600,  910, 1264, 1648
};

void setup(void)
{
  //  lcd.init();
  //  lcd.backlight();
  //  lcd.clear();
  //  lcd.setCursor(0,0);
  //  lcd.print("Voltage Arduino");
  //  lcd.setCursor(0,1);
  //  lcd.print("AIN0: ");
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  Serial.setTimeout(50);
  tlmSerial.begin(57600);
  tlmSerial.setTimeout(50);


  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }

  dac.begin(0x60);  //Start i2c communication with the DAC (slave address sometimes can be 0x60, 0x61 or 0x62)
  delay(10);
  dac.setVoltage(2048, false); //Set DAC voltage output ot 0V (MOSFET turned off)
  delay(10);
}

void loop(void)
{
  int16_t adc0;
  float volts0;

  adc0 = ads.readADC_SingleEnded(0);
  volts0 = ads.computeVolts(adc0);

  //  Serial.println(volts0);

  //  lcd.setCursor(6,1);
  //  lcd.print(String(volts0)+"V     ");


  dac.setVoltage(pgm_read_word(&(DACLookup_FullSine_5Bit[i])), false);

  if (stream)
  {
    digitalWrite(13, HIGH);
    tlmSerial.print("#D#");
    tlmSerial.print(int(volts0*100));
//    tlmSerial.print(567);
    tlmSerial.print("#");
    tlmSerial.print(678);
    tlmSerial.print("#");
    tlmSerial.print(345);
    tlmSerial.print("#");
    tlmSerial.print(123);
    tlmSerial.print("#");
    tlmSerial.print(789);
    tlmSerial.print("#");
    tlmSerial.print(15);
    tlmSerial.write("#\n");
  }
  else
  {
    digitalWrite(13, LOW);
  }



  if (tlmSerial.available())
  {
    //    String data = tlmSerial.readString();
    //    Serial.println(data);
    char data = tlmSerial.read();
    Serial.print(data);
    if (data == '?')
    {
      Serial.write(data);
      stream = false;
      tlmSerial.write("#!#5#\n");
    }
    if (data == 'A')
    {
      Serial.write(data);
      stream = true;

    }
    if (data == 'S')
    {
      Serial.write(data);
      stream = false;

    }

  }

  i++;
  if (i > 31)
  {
    i = 0;
  }
  delay(50);
}
