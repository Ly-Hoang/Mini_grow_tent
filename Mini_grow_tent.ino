#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

//char UART;

const byte DS1307 = 0x68;
const byte NumberOfFields = 7;

byte Fan = 3;
byte Led = 4;
byte Pump = 2;
byte button = 11;

////////////////////////////////////////////////////////////////////////////////////////

bool flowering_state = 1;   // 0-veg state | 1-flowering_state

int off_fan_sec = 30; // fan will turn on 0s -> off_fan_sec(s)

int pump_on_duration = 25;  // when pump is on pump will work in pump_on_duration(s)

int on_pump_time1 = 7;
int on_pump_time2 = 17;

//////////////////////////////////////////////////////////////////////////////////////////

int on_led_time = 6;       
int off_led_time = 18;

int on_pump_sec = off_fan_sec + 1;
int off_pump_sec = on_pump_sec + pump_on_duration;

LiquidCrystal_I2C lcd(0x27,16,2); 

const int DHTPIN = 12;
const int DHTTYPE = DHT22;
DHT dht(DHTPIN, DHTTYPE);

byte customChar[] = {
  B00110,
  B01001,
  B01001,
  B00110,
  B00000,
  B00000,
  B00000,
  B00000
};

int second, minute, hour, day, wday, month, year;

int preMin;
boolean lcdStat;
boolean preButton;

void setup()
{
  pinMode(Fan, OUTPUT);
  pinMode(Led, OUTPUT);
  pinMode(Pump, OUTPUT);
  pinMode(button, INPUT_PULLUP);

  dht.begin();

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, customChar);
  lcd.home();
  Wire.begin();
  //setTime(18, 9, 0, 1, 14, 5, 23);
  Serial.begin(9600);
  readDS1307();
  preMin = minute;
  digitalWrite(13, 0);
  lcdStat = 1;
  preButton = digitalRead(button);
  if(flowering_state == 0)    // veg_state
  {
    int on_led_time = 3;       
    int off_led_time = 21;
  }
  if(flowering_state == 1) // flowering_state
  {
    int on_led_time = 6;       
    int off_led_time = 18;
  }
  
}

void loop()
{


  
  readDS1307();

  int doam = dht.readHumidity();
  int doC = dht.readTemperature();
  int doF = dht.readTemperature(true);

  if (isnan(doam) || isnan(doC) || isnan(doF))
  {
    Serial.println("Failed to read from DHT");
  }
  else
  {
    String mess = String(doC) + "," + String(doam);
    Serial.println(mess);
  }

  

  
  if(hour >= on_led_time && hour < off_led_time)
  {
    digitalWrite(Led, HIGH);
    if(hour == on_pump_time1 && minute == 0 || digitalRead(button) == 0)
    {
      if(second > on_pump_sec && second < off_pump_sec || digitalRead(button) == 0)
      {
        digitalWrite(Pump, HIGH);
      }
      else
      {
        digitalWrite(Pump, LOW);
      }
    }

    if(hour == on_pump_time2 && minute == 0)
    {
      if(second > on_pump_sec && second < off_pump_sec)
      {
        digitalWrite(Pump, HIGH);
      }
      else
      {
        digitalWrite(Pump, LOW);
      }
    }

    
      if(preMin != minute)
    {
      digitalWrite(Fan, HIGH);
      preMin = minute;
      
    }
    if(second >= off_fan_sec)
      {
        digitalWrite(Fan, LOW);
      }
  }
  else
  {
      if(digitalRead(button) == 0)
    {
      digitalWrite(Pump, HIGH);
    }
      if(digitalRead(button) == 1)
    {
      digitalWrite(Pump, LOW);
    }
    digitalWrite(Led, LOW);
    if(minute % 2 == 0)
    {
      if(second < off_fan_sec)
      {
        digitalWrite(Fan, HIGH);
      }
      if(second >= off_fan_sec)
      {
        digitalWrite(Fan, LOW);
      }
    }
  }

 
  

//  if(preButton != digitalRead(button))
//  { 
//      if(digitalRead(button) == 0)
//      {
//        digitalWrite(Pump, HIGH);
////        lcdStat = !lcdStat;
////        lcd.clear();
////        setTime(19, 47,40, 5, 23, 3, 23);
//      }
//      digitalWrite(Pump, LOW);
//      preButton = digitalRead(button);
//  }
  
  if(lcdStat == 0)
  {
    lcd.setCursor(0,0);
    lcd.print("Humidity:");
    lcd.setCursor(0,1);
    lcd.print("Temp:");
    lcd.setCursor(15,0);
    lcd.print("%");
    lcd.setCursor(8,1);
    lcd.write(0);
    lcd.setCursor(9,1);
    lcd.print("C");
    lcd.setCursor(14,1);
    lcd.write(0);
    lcd.setCursor(15,1);
    lcd.print("F");
     if (isnan(doam) || isnan(doC) || isnan(doF))
    {
      //lcd.clear();
      return;
    }
    
    lcd.setCursor(10,0);
    lcd.print(doam);
    
    lcd.setCursor(6,1);
    lcd.print(doC);
  
    lcd.setCursor(12,1);
    lcd.print(doF);
    
  }

  if(lcdStat == 1)
  {
    if(day < 10)
    {
      lcd.setCursor(3,1);
      lcd.print("0");
      lcd.setCursor(4,1);
      lcd.print(day);
    }
    else
    {
      lcd.setCursor(3,1);
      lcd.print(day);
    }

    lcd.setCursor(5,1);
    lcd.print("/");

    if(month < 10)
    {
      lcd.setCursor(6,1);
      lcd.print("0");
      lcd.setCursor(7,1);
      lcd.print(month);
    }
    else
    {
       lcd.setCursor(6,1);
       lcd.print(month);
    }
    
    
    lcd.setCursor(8,1);
    lcd.print("/");
    
    lcd.setCursor(9,1);
    lcd.print(year);
    if(hour < 10)
    {
      lcd.setCursor(4,0);
      lcd.print("0");
      lcd.setCursor(5,0);
      lcd.print(hour);
    }
    else
    {
      lcd.setCursor(4,0);
      lcd.print(hour);
    }

    lcd.setCursor(6,0);
    lcd.print(":");

    
    if(minute < 10)
    {
      lcd.setCursor(7,0);
      lcd.print("0");
      lcd.setCursor(8,0);
      lcd.print(minute);
    }
    else
    {
      lcd.setCursor(7,0);
      lcd.print(minute);
    }


    lcd.setCursor(9,0);
    lcd.print(":");

    
    if(second < 10)
    {
      lcd.setCursor(10,0);
      lcd.print("0");
      lcd.setCursor(11,0);
      lcd.print(second);
    }
    else
    {
      lcd.setCursor(10,0);
      lcd.print(second);
    }
  }
  delay(200);
  //digitalClockDisplay();
}

void readDS1307()
{
  Wire.beginTransmission(DS1307);
  Wire.write((byte)0x00);
  Wire.endTransmission();
  Wire.requestFrom(DS1307, NumberOfFields);

  second = bcd2dec(Wire.read() & 0x7f);
  minute = bcd2dec(Wire.read() );
  hour = bcd2dec(Wire.read() & 0x3f);
  wday = bcd2dec(Wire.read() );
  day = bcd2dec(Wire.read() );
  month = bcd2dec(Wire.read() );
  year = bcd2dec(Wire.read() );
  year += 2000;
}
 int bcd2dec(byte num)
 {
  return ((num/16 * 10) + (num % 16));
 }
 int dec2bcd(byte num)
 {
  return ((num/10 * 16) + (num % 10));
 }
 void digitalClockDisplay()
 {
  Serial.print(hour);
  printDigits(minute);
  printDigits(second);
  Serial.print(" ");
  Serial.print(day);
  Serial.print(" ");
  Serial.print(month);
  Serial.print(" ");
  Serial.print(year);
  Serial.print(" ");
  Serial.println();
 }
void printDigits(int digits)
{
  Serial.print(":");
  if(digits < 10)
  {
    Serial.print('0');
  }
  Serial.print(digits);
}
void setTime (byte hr, byte mint, byte sec, byte wd, byte d, byte mth, byte yr)
{
  Wire.beginTransmission(DS1307);
  Wire.write(byte(0x00));
  Wire.write(dec2bcd(sec)); 
  Wire.write(dec2bcd(mint)); 
  Wire.write(dec2bcd(hr)); 
  Wire.write(dec2bcd(wd)); 
  Wire.write(dec2bcd(d)); 
  Wire.write(dec2bcd(mth)); 
  Wire.write(dec2bcd(yr)); 
  Wire.endTransmission();
}
void resetBoard()
{
  asm volatile ("jmp 0"); //Nhảy về dòng đầu tiên của chương trình
}
