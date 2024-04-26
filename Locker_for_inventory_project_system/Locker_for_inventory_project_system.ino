#include <Wire.h>


//WIFI SECTION
#include <WiFiNINA.h>
char ssid[] = "LorimIpsum";
char wifi_password[] = "lockerHardware";


//LCD SECTION
//https://wiki.seeedstudio.com/Grove-LCD_RGB_Backlight/
#include "rgb_lcd.h"

const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

//SERVO MOTOR SECTION
//https://projecthub.arduino.cc/arduino_uno_guy/the-beginners-guide-to-micro-servos-ae2a30
#include <Servo.h>
Servo servo;

//KEYPAD SECTION 
//https://www.circuitbasics.com/how-to-set-up-a-keypad-on-an-arduino/
#include <Keypad.h>
const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

String password = "123";
String input_password ="";

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 

//DOOR SENSOR
//https://arduinogetstarted.com/tutorials/arduino-door-sensor


//PINS
rgb_lcd lcd;          //PIN I2C
const int DOOR_SENSOR_PIN = 13;
const int SENSOR_MOTOR_PIN = 10;
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 


void wifi_setup()
{
  while (!Serial) 
  {
        ; // Wait for serial port to connect
  }

    // Attempt to connect to WiFi network
  Serial.print("Attempting to connect to WiFi network: ");
  Serial.println(ssid);

  while (WiFi.begin(ssid, wifi_password) != WL_CONNECTED)
  {
        Serial.print(".");
        delay(5000);  // Retry every 5 seconds if connection fails
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() 
{
   Serial.begin(9600);

   wifi_setup();

  pinMode(13, INPUT_PULLUP);

  //door_sensor_setup();
  servo_motor_setup();
  pin_pad_setup();
  lcd_setup();
  delay(1000);
}


void door_sensor_setup(){pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP); }

void pin_pad_setup(){Serial.begin(9600);}

void lcd_setup()
{
    lcd.begin(16, 2);//Colums and rows to dedicated LED Screen
    lcd.setRGB(colorR, colorG, colorB);
}

void servo_motor_setup()
{
  servo.attach(SENSOR_MOTOR_PIN);
  servo.write(0);
}

bool get_booking()
{
  //TODO CALL TO BACKEND
  
  delay(300);
  bool booking = false;
  return booking;
}

void open_locker()
{
  //TODO finish implemeting
  servo.write(180);
}


void loop() 
{
  
    if (WiFi.status() != WL_CONNECTED) {
        lcd.print("WiFi connection lost. Reconnecting...");
        while (WiFi.begin(ssid, wifi_password) != WL_CONNECTED) {
            lcd.print(".");
            delay(5000);  // Retry every 5 seconds if connection fails
        }
        lcd.print("WiFi reconnected");
  }


/*
    servo.write(90);
    delay(2400);
    servo.write(0);
    delay(2400);


while(true)
{
  int doorState = digitalRead(13); // read state
  if (doorState == HIGH) {
    lcd.clear();
    lcd.print("The door is open");
    delay(100);
  } else {
    lcd.clear();
    lcd.print("The door is closed");
    delay(100);
  }
}
  
  bool wait_for_booking = true;
  while(wait_for_booking)
  {
    lcd.setCursor(0, 0);
    // print the number of seconds since reset:
    lcd.clear();
    lcd.print("Waiting for booking");
    wait_for_booking = get_booking();
    delay(300);
  }


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter password");
  lcd.setCursor(0,1);

  int counter_password_entries = 0;
  bool wait_for_user_input = true;

  while(wait_for_user_input)
  {
    char custom_key = customKeypad.getKey();
    bool password_correct = false;
    
    if (custom_key){

      if(custom_key == '*')
      {
        password_correct = (input_password == password);

        if(password_correct)
        {
          lcd.print("Password correct");
          delay(200);
          lcd.clear();

          open_locker();

          lcd.print("Get your item");
          delay(200);
          wait_for_user_input = false;
        }
        else if(!password_correct)
        {
          counter_password_entries += 1;

          lcd.print("password wrong");
          lcd.clear();
          lcd.print("try again ");
          lcd.print(String(counter_password_entries)); // Convert integer to string
          lcd.print("/3");
          lcd.setCursor(0,1);
          input_password =""; //reset password entry

          if(counter_password_entries == 3)
          {
            lcd.clear();
            lcd.print("Password wrong to many times");
            delay(300);
            wait_for_user_input = false;
          }
          
        }
      }
      if(custom_key != '*')
      {
      input_password += custom_key;
      lcd.print(custom_key);
      }
    }
  }
    delay(200); 
    */
}