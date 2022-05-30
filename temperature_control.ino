/*
  LiquidCrystal Library

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 */

// include the library code:
#include <LiquidCrystal.h>
#include <dht.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

dht DHT;
int DHT11_PIN = 50;
int buzzerPin = 32;
int redLedPin = 41;
int greenLedPin = 39;
int yellowLedPin = 37;
int testerButtonPin = 13;

int criticalH = 90; //critical Humidity value
int highH = 70; // high Humidity value
int criticalT = 50; //critical temperature value
int highT = 45; // high temperature value

void siren ()
{
  for(int j=0;j<5;j++)
  {   
    for(int i=5; i<55; i++)
    {
      tone(buzzerPin, i * 10, 500);
      delay(10);
     }
     for(int i=55; i>5; i--)
     {
       tone(buzzerPin, i * 10, 500);
       delay(10);
      }
    }
}

void siren2 ()
{
  for(int i=0;i<4;i++)
  {
    for(int f=635;f<=912;f++)
    {
      tone(buzzerPin, f);
      delay(7);
    }

    for(int f=911;f>=634;f--)
    {
      tone(buzzerPin, f);
      delay(7);
    }
  }
}

void siren3 ()
{
  tone(buzzerPin,440,200);
  delay(100);
  noTone(buzzerPin);
  tone(buzzerPin,494,500);
  delay(300);
  noTone(buzzerPin);
  tone(buzzerPin,523,300);
  delay(300);
  noTone(buzzerPin);
}

void readBTData()
{
  String BT_data = "";//should be ####
  
  while (Serial1.available())
  {
    byte incomingByte = Serial1.read();
    delay(5);
    BT_data += char(incomingByte); 
  }

  //BT_data = <code (##)><num (##)>
  
  if (BT_data.length() == 6) //good length
  {
    if (BT_data.startsWith("ct"))
    {
      if (BT_data.substring(2).toInt() > 0 && BT_data.substring(2).toInt() < 100)
      {
        criticalH = BT_data.substring(2).toInt();
      }
    }
    if (BT_data.startsWith("hh"))
    {
      if (BT_data.substring(2,4).toInt() > 0 && BT_data.substring(2,4).toInt() < 100)
      { 
        highH = BT_data.substring(2,4).toInt();
      }
    }
    if (BT_data.startsWith("ct"))
    {
      if (BT_data.substring(2).toInt() > 0 && BT_data.substring(2).toInt() < 100)
      {
        criticalT = BT_data.substring(2).toInt();
      }
    }
    if (BT_data.startsWith("ht"))
    {
      if (BT_data.substring(2).toInt() > 0 && BT_data.substring(2).toInt() < 100)
      {
        highT = BT_data.substring(2).toInt();
      }
    }
  }
  else if (BT_data.length() > 0)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.println("Bad input data! ");
    delay(2000);
  }
}

void setup() 
{
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  Serial1.begin(9600);//Bluetooth coomunication

  Serial.begin(9600);

  pinMode(buzzerPin, OUTPUT);
  pinMode(redLedPin, OUTPUT); 
  pinMode(yellowLedPin, OUTPUT);  
  pinMode(greenLedPin, OUTPUT);  
  pinMode(testerButtonPin, INPUT_PULLUP);  

  digitalWrite(buzzerPin, HIGH); //OFF
  digitalWrite(redLedPin, HIGH);  //OFF
  digitalWrite(greenLedPin, HIGH);//OFF
  digitalWrite(yellowLedPin, HIGH);//OFF
  
}

void loop() 
{
  if (digitalRead(testerButtonPin) == LOW) //test mode
  {
    lcd.setCursor(0, 0);
    lcd.print("Test mode       ");
    lcd.setCursor(0, 1);
    lcd.print("Etai Aframian  ");
    
    digitalWrite(redLedPin, LOW);  //ON
    digitalWrite(greenLedPin, LOW);//ON
    digitalWrite(yellowLedPin, LOW);//ON

    Serial1.println("Test");

    // Send sound signal
    siren();
  }
  else
  {
    DHT.read11(DHT11_PIN);
    lcd.clear();
    lcd.setCursor(0, 0); // set the cursor to column 0, line 1
    lcd.print("Tempreture: ");
    lcd.print(int(DHT.temperature));
  
    lcd.setCursor(0, 1); // set the cursor to column 0, line 2
    lcd.print("Humidity: ");
    lcd.print(int(DHT.humidity));
    lcd.print("%");

   delay(1500);

    if (DHT.humidity > criticalH && DHT.temperature > criticalT) //Critical
    {
      digitalWrite(redLedPin, LOW);  //ON
      digitalWrite(yellowLedPin, HIGH);  //OFF
      digitalWrite(greenLedPin, HIGH);//OFF

      //Bluetooth comm
      Serial1.println("Alert! Critical temperature!");
      Serial1.println("Humidity: " + String(DHT.humidity) + "%");
      Serial1.println("Temperature: " + String(DHT.temperature) + "℃");
      
      // Send sound signal
      siren2();
    }
    else if (DHT.humidity > highH && DHT.temperature > highT) //High
    {
      siren3(); // Send 1.5KHz sound signal
      digitalWrite(yellowLedPin, LOW);//ON
      digitalWrite(redLedPin, HIGH);  //OFF
      digitalWrite(greenLedPin, HIGH);//OFF

      //Bluetooth comm
      Serial1.println("Alert! High temperature!");
      Serial1.println("Humidity: " + String(DHT.humidity) + "%");
      Serial1.println("Temperature: " + String(DHT.temperature) + "℃");
    }
    else
    {
      noTone(buzzerPin); // Stop sound signal
      //digitalWrite(buzzerPin, HIGH);
      digitalWrite(greenLedPin, LOW);//ON
      digitalWrite(yellowLedPin, HIGH);//OFF
      digitalWrite(redLedPin, HIGH);  //OFF
    }
  }

  readBTData();
}
