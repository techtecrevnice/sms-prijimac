
#include <LiquidCrystal.h>
#include <SPI.h>
#include <Ethernet.h>
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

//kod pro ethernet shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "dweet.io";    // name address for Google (using DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 1, 177);
IPAddress myDns(192, 168, 1, 1);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

bool printWebData = true;  // set to false for better speed measurement

void setup()
{ 
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("Techtec SMS");

      
    // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  
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

  
  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet

   

  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
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

void dweet(String text) {

  text.replace(" ", "%20");
  Serial.print("connecting to ");
  Serial.print(server);
  Serial.println("...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.print("connected to ");
    Serial.println(client.remoteIP());
    // Make a HTTP request:
    client.println("GET /dweet/quietly/for/techtec?zprava="+text+" HTTP/1.1");
    client.println("Host: dweet.io");
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }


  while(client.connected()) {
    int len = client.available();
    if (len > 0) {
      byte buffer[80];
      if (len > 80) len = 80;
      client.read(buffer, len);
      
      if (printWebData) {
        Serial.write(buffer, len); // show in the serial monitor (slows some boards)
      }
    }
  }

  Serial.println();
  Serial.println("disconnecting.");
  client.stop();
  
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
      lcd.clear();
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

      // kod pro displey
//      lcd.setCursor(0,0);
//      lcd.print(cislo);
//      lcd.setCursor(0,1);
//
//      if (text.length() <17) {
//        lcd.print(text);
//        
//      } else {
//        for (int i = 0; i < text.length() - 16; i++) {
//          lcd.print(text.substring(i, i + 16));
//          delay(1000);
//          lcd.scrollDisplayLeft();
//        }
//      }

      // kod pro odeslani na web
      dweet(text);
      
      blikej();
    }
    Serial.println(sms);
    //Serial.write(Serial1.read());//Forward what Software Serial received to Serial Port
  }
}
