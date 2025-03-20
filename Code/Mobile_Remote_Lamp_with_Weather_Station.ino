         ////////////////////////////////////////////////////  
        //   Mobile Remote Lamp with Weather Station      //
       //                                                //
      //          -------------------------             //
     //              Arduino Nano V3.0                 //           
    //               by Kutluhan Aktar                // 
   //                                                //
  ////////////////////////////////////////////////////

// Control your room lighting system and display weather information via specifically developed Android application and a uniquely designed PCB.
//
// You can find more information regarding the project from the link below:
// https://www.theamplituhedron.com/projects/Mobile-Remote-Lamp-with-Weather-Station/
//
// Connections
// Arduino Nano :           
//                                HC-06 Bluetooth Module
// D4  --------------------------- TX
// D5  --------------------------- RX
// 5V  --------------------------- 5V
// GND --------------------------- GND
//                                I2C LCD Screen (BUS)
// A4  --------------------------- SDA
// A5  --------------------------- SCL
// 5V  --------------------------- 5V
// GND --------------------------- GND
//                                DHT11 Temperature and Humidity Sensor
// D2  --------------------------- Signal
// 5V  --------------------------- 5V
// GND --------------------------- GND
//                                L298N DC Motor Driver
// A0  --------------------------- IN_1
// A1  --------------------------- IN_2
// A2  --------------------------- IN_3
// A3  --------------------------- IN_4
//                                2-Way Relay
// D12 --------------------------- IN_1
// D13 --------------------------- IN_2
// 5V  --------------------------- 5V
// GND --------------------------- GND
//                                RGB LED
// D9  --------------------------- R
// 5V  --------------------------- 5V
// D10 --------------------------- G
// D11 --------------------------- B
//                                5mm Yellow LED
// D8  --------------------------- +
//                                5mm Blue LED
// D7  --------------------------- +
//                                5mm Green LED
// D6  --------------------------- +
//                                5mm Red LED
// D3  --------------------------- +
//
// Spare Pins on the J1 Connector on the board:
// A6
// A7
// VIN
// 3V3
// GND


// Include the SoftwareSerial library to communicate with the HC-06 Bluetooth Module.
#include <SoftwareSerial.h>
// Initiate the bluetooth module. Connect the defined RX pin (4) to the TX pin on the bluetooth module.
SoftwareSerial Remote_Lamp(4, 5); // RX, TX

// Include DHT.h library.
#include "DHT.h"
// Define the dht object.
DHT dht;

// include LiquidCrystal_I2C and Wire libraries to run I2C module.
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define custom characters for the LCD.
uint8_t bell[8]  = {0x4, 0xe, 0xe, 0xe, 0x1f, 0x0, 0x4};
uint8_t duck[8]  = {0x0, 0xc, 0x1d, 0xf, 0xf, 0x6, 0x0};
uint8_t check[8] = {0x0, 0x1 ,0x3, 0x16, 0x1c, 0x8, 0x0};


// Define data holders.
float humidity, temperature, fahrenheit;

// Define the Weather_Screen boolean to activate the Weather Screen when requested by a command over the bluetooth module.
boolean Weather_Screen = false;

// Define the pins attached to LEDs.
#define RED 9
#define GREEN 10
#define BLUE 11
#define Yellow_LED 8
#define Blue_LED 7
#define Green_LED 6
#define Red_LED 3

// Define the pins attached to the L298N Motor Driver.
#define L_1 A0
#define L_2 A1
#define L_3 A2
#define L_4 A3

// Define the pins attached to the 2-Way Relay.
#define R_1 12
#define R_2 13


void setup(){

// You can change the default settings of the HC-06 Module by uncommenting the function below - Name: Remote Lamp, Password: 1234, Baud Rate: 9600.
// Serial.begin(9600); changeBluetoothSettings();

// Activate the bluetooth module.
Remote_Lamp.begin(9600);

// Initialize DHT11 Module on D2.
dht.setup(2);

// Initialize the LCD.
// You do not need to take any further action to define SDA and SCL pins in Wire library due to embedded SDA and SCL settings on Arduino Nano. 
lcd.begin();

// Create the predefined custom characters:
lcd.createChar(0, bell);
lcd.createChar(1, duck);
lcd.createChar(2, check);

// Home Screen.
lcd.clear();
lcd.home();
lcd.print("Remote Lamp V2.0");
lcd.setCursor(0, 1);
lcd.print("  ");
lcd.write(0);
lcd.print(" Waiting...");

// Set output pins.
pinMode(Yellow_LED, OUTPUT);
pinMode(Blue_LED, OUTPUT);
pinMode(Green_LED, OUTPUT);
pinMode(Red_LED, OUTPUT);
pinMode(L_1, OUTPUT);
pinMode(L_2, OUTPUT);
pinMode(L_3, OUTPUT);
pinMode(L_4, OUTPUT);
pinMode(R_1, OUTPUT);
pinMode(R_2, OUTPUT);


// Adjust the RGB color to white at the home screen initializing.
changeColor(255, 255, 255);

// Turn off relays at the home screen while initializing.
digitalWrite(R_1, HIGH);
digitalWrite(R_2, HIGH);

}

void loop(){

getDataFromDHT11();

// If the Weather Screen is requested by a command, print the weather information by the DHT11 Temperature and Humidity Sensor.
if(Weather_Screen){
    lcd.clear();
    lcd.print("Temp:" + String(temperature, 1) + "C " + String(fahrenheit, 1) + "F");
    lcd.setCursor(0, 1);
    lcd.print("Hum: " + String(humidity, 1) + "%");
}

// If the bluetooth module is receiving commands from the Android application:
if(Remote_Lamp.available()){
  char c = Remote_Lamp.read();
  // Commands:
  switch(c){
    // Return to the Home Screen:
    case 'h':
    lcd.clear();
    lcd.home();
    lcd.print("Remote Lamp V2.0");
    lcd.setCursor(0, 1);
    lcd.print("  ");
    lcd.write(0);
    lcd.print(" Waiting...");
    Weather_Screen = false;
    break;
    // Weather Screen Settings:
    case 't':
    Weather_Screen = true; // activate
    break;
    // LEDs settings:
    case '0':
    digitalWrite(Yellow_LED, LOW);
    digitalWrite(Blue_LED, LOW);
    digitalWrite(Green_LED, LOW);
    digitalWrite(Red_LED, LOW);
    lcd.clear();
    lcd.home();
    lcd.print("LEDs OFF => ");
    lcd.write(1);
    Weather_Screen = false;
    break;
    case '1':
    digitalWrite(Yellow_LED, HIGH);
    lcd.clear();
    lcd.home();
    lcd.print("Yellow LED => ");
    lcd.write(2);
    Weather_Screen = false;
    break;
    case '2':
    digitalWrite(Blue_LED, HIGH);
    lcd.clear();
    lcd.home();
    lcd.print("Blue LED => ");
    lcd.write(2);
    Weather_Screen = false;
    break;
    case '3':
    digitalWrite(Green_LED, HIGH);
    lcd.clear();
    lcd.home();
    lcd.print("Green LED => ");
    lcd.write(2);
    Weather_Screen = false;
    break;
    case '4':
    digitalWrite(Red_LED, HIGH);
    lcd.clear();
    lcd.home();
    lcd.print("Red LED => ");
    lcd.write(2);
    Weather_Screen = false;
    break;
    // Lamp settings (Relay):
    case '5':
    digitalWrite(R_1, LOW);
    lcd.clear();
    lcd.home();
    lcd.print("Lamp_1 => ON");
    Weather_Screen = false;
    break;
    case '6':
    digitalWrite(R_2, LOW);
    lcd.clear();
    lcd.home();
    lcd.print("Lamp_2 => ON");
    Weather_Screen = false;
    break;
    case '7':
    digitalWrite(R_1, HIGH);
    lcd.clear();
    lcd.home();
    lcd.print("Lamp_1 => OFF");
    Weather_Screen = false;
    break;
    case '8':
    digitalWrite(R_2, HIGH);
    lcd.clear();
    lcd.home();
    lcd.print("Lamp_2 => OFF");
    Weather_Screen = false;
    break;
    // Motor driver settings (L298N):
    case 'a':
    digitalWrite(L_1, LOW);
    digitalWrite(L_2, HIGH);
    lcd.clear();
    lcd.home();
    lcd.print("R_Wheel Turning!");
    lcd.setCursor(5, 1);
    lcd.write(1);lcd.write(1);lcd.write(1);lcd.write(1);lcd.write(1);lcd.write(1);
    Weather_Screen = false;
    break;
    case 's':
    digitalWrite(L_3, HIGH);
    digitalWrite(L_4, LOW);
    lcd.clear();
    lcd.home();
    lcd.print("L_Wheel Turning!");
    lcd.setCursor(5, 1);
    lcd.write(1);lcd.write(1);lcd.write(1);lcd.write(1);lcd.write(1);lcd.write(1);
    Weather_Screen = false;
    break;
    case 'd':
    digitalWrite(L_1, LOW);
    digitalWrite(L_2, LOW);
    digitalWrite(L_3, LOW);
    digitalWrite(L_4, LOW);
    lcd.clear();
    lcd.home();
    lcd.print("Wheels OFF!");
    lcd.setCursor(5, 1);
    lcd.write(1);lcd.write(1);lcd.write(1);lcd.write(1);lcd.write(1);lcd.write(1);
    Weather_Screen = false;
    break;
    // RGB LED settings:
    case 'r':
    changeColor(255, 0, 0);
    lcd.clear();
    lcd.home();
    lcd.print("RGB => Red");
    Weather_Screen = false;
    break;
    case 'g':
    changeColor(0, 255, 0);
    lcd.clear();
    lcd.home();
    lcd.print("RGB => Green");
    Weather_Screen = false;
    break;
    case 'b':
    changeColor(0, 0, 255);
    lcd.clear();
    lcd.home();
    lcd.print("RGB => Blue");
    Weather_Screen = false;
    break;
    case 'y':
    changeColor(255, 255, 0);
    lcd.clear();
    lcd.home();
    lcd.print("RGB => Yellow");
    Weather_Screen = false;
    break;
    case 'p':
    changeColor(255, 0, 255);
    lcd.clear();
    lcd.home();
    lcd.print("RGB => Purple");
    Weather_Screen = false;
    break;
    case 'c':
    changeColor(0, 255, 255);
    lcd.clear();
    lcd.home();
    lcd.print("RGB => Cyan");
    Weather_Screen = false;
    break;
    case 'w':
    changeColor(255, 255, 255);
    lcd.clear();
    lcd.home();
    lcd.print("RGB => White");
    Weather_Screen = false;
    break;
    case 'o':
    changeColor(0, 0, 0);
    lcd.clear();
    lcd.home();
    lcd.print("RGB => OFF");
    Weather_Screen = false;
    break;
    
  }
}

}

void getDataFromDHT11(){
  // Get weather information - temparature and humidity.
  humidity = dht.getHumidity();
  temperature = dht.getTemperature();
  fahrenheit = dht.toFahrenheit(temperature);
  // Wait until the module is ready.
  delay(2 * dht.getMinimumSamplingPeriod());
  
}

void changeColor(int red, int green, int blue){
  // Change the color of the RGB LED using the analogWrite() function.
  analogWrite(RED, 255 - red);
  analogWrite(GREEN, 255 - green);
  analogWrite(BLUE, 255 - blue);

}

void changeBluetoothSettings(){
  // Define the new settings.
  String Name = "Remote Lamp";
  int Password = 1234;
  String Uart = "9600,0,0";

  Remote_Lamp.print("AT+NAME"); // Change the name.
  Remote_Lamp.println(Name); 
  Serial.print("Name is changed: ");
  Serial.println(Name);
  delay(2000);
  Remote_Lamp.print("AT+PSWD"); // Change the password.
  Remote_Lamp.println(Password);
  Serial.print("Password is changed: ");
  Serial.println(Password);
  delay(2000);
  Remote_Lamp.print("AT+UART"); // Change the baud rate. If the bluetooth module is a HC-05, the default value of baud rate is 38400.
  Remote_Lamp.println(Uart);
  Serial.print("Baud rate is set: ");
  Serial.println(Uart);
  delay(2000);
  Serial.println("Task is completed."); // You can see in the terminal whether the task is completed correctly or not.

}
