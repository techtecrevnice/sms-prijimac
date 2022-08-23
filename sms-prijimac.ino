
#include <LiquidCrystal.h>
//LCD pin to Arduino
const int pin_RS = 8; 
const int pin_EN = 9; 
const int pin_d4 = 4; 
const int pin_d5 = 5; 
const int pin_d6 = 6; 
const int pin_d7 = 7; 
const int pin_BL = 10; 
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);

//Create software serial object to communicate with SIM800L
String sms;

void setup()
{ 
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("Techtec SMS");
      
  delay(3000);
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  
  //Begin serial communication with Arduino and SIM800L
  Serial1.begin(9600);

  Serial.println("Initializing...");
  delay(1000);

  Serial1.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  Serial1.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  Serial1.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  updateSerial();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); 
}

void blikej()
{
  
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(100);     
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(100);     
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(100);   
  
}

void loop()
{
  updateSerial();
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    Serial1.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(Serial1.available()) 
  {
    sms = Serial1.readString();
    sms.trim();
    // parsovani SMS
    if (sms.startsWith("+CMT:"))
    {
      int zacatek = sms.indexOf("\"");
      int konec = sms.indexOf("\"", zacatek + 1);
      String cislo = sms.substring(zacatek+1,konec);
       
      zacatek = sms.indexOf("\"", konec+4);
      konec = sms.indexOf("\"", zacatek + 1);
      String cas = sms.substring(zacatek+1,konec);

      zacatek = sms.indexOf("\n");
      String text = sms.substring(zacatek+1);

      //Serial.println("Prisla SMS");
      //Serial.println(cislo);
      //Serial.println(cas);
      //Serial.println(text);
      lcd.setCursor(0,0);
      lcd.print(cislo);
      lcd.setCursor(0,1);
      lcd.print(text);
       
      blikej();
    }
    Serial.println(sms);
    //Serial.write(Serial1.read());//Forward what Software Serial received to Serial Port
  }
}
