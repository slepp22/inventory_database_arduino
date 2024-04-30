#include <Wire.h>

//WIFI SECTION
#include <WiFiNINA.h>
char ssid[] = "LorimIpsum";
char wifi_password[] = "lockerHardware";
int status = WL_IDLE_STATUS; // WiFi status



WiFiClient client;
const char* server = "f-itplfo6nya-uc.a.run.app";
const int port = 443; // HTTPS port               // Port for HTTPS (443 for HTTPS)

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

String password = "";
String input_password ="";

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 

//CURRENTSENSOR SECTION
//https://www.aeq-web.com/arduino-current-sensor-power-electricity-meter/

int current_sensor_scaling_factor = 7;
int voltage = 220; //Voltage of Power Grid
int CURRENT_SENSOR_PIN = A0;

float current_sensor_value = 0;
float current = 0;



//DOOR SENSOR
//https://arduinogetstarted.com/tutorials/arduino-door-sensor


//PINS
rgb_lcd lcd;          //PIN I2C
const int DOOR_SENSOR_PIN = 13;
const int SENSOR_MOTOR_PIN = 10;
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 


String api_call(String endpoint) {
  if (client.connectSSL("f-itplfo6nya-uc.a.run.app", 443)) {
    Serial.println("Connected to server");

    // Send HTTP GET request with the specified endpoint
    client.print("GET ");
    client.print(endpoint);
    client.println(" HTTP/1.1");
    client.println("Host: f-itplfo6nya-uc.a.run.app");
    client.println("Connection: close");
    client.println();

    // Variables to capture PIN value
    bool pinFound = false;
    String pinValue = "";
    
    // Wait for response and capture PIN value
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        // Check for start of "pin" value
        if (!pinFound && isDigit(c)) {
          pinFound = true; // Start capturing pin value
          pinValue += c;
        } else if (pinFound && pinValue.length() < 4 && isDigit(c)) {
          // Continue capturing digits if pin value is not complete
          pinValue += c;
        } else if (pinFound && pinValue.length() == 4) {
          // Stop capturing once 4-digit pin value is complete
          break;
        }
      }
    }

    // Close connection
    client.stop();
    Serial.println("\nAPI call completed.");

    // Print captured PIN value
    if (pinValue.length() == 4) {
      Serial.print("PIN value: ");
      Serial.println(pinValue);
      return pinValue;
    } else {
      Serial.println("Failed to retrieve valid PIN value");
    }
  } else {
    Serial.println("Connection to server failed");
  }
  delay(5000); // Delay before making the next API call (adjust as needed)
}



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
 // door_sensor_setup();
  servo_motor_setup();
  pin_pad_setup();
  lcd_setup();
  close_locker();
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
  delay(300);
  bool booking = false;
  return booking;
}

void open_locker()
{
  servo.write(90);
}

void close_locker()
{
  servo.write(0);
}

void get_current_sensor_value()
{
  int i = 0;
  current_sensor_value = 0;
  int times_sensor_read = 2000;
  
  while(i < times_read_sensor){
    int tmp_sensor_value = analogRead(CURRENT_SENSOR_PIN);
    current_sensor_value = current_sensor_value + tmp_sensor_value;
    i++;
  }

  current = ((current_sensor_value/times_sensor_read)*current_sensor_scaling_factor)/100); 

  Serial.print("Current: ");
  Serial.print(current);
  Serial.print(" A Watts: ");
  Serial.print(current*voltage);
  Serial.print("\n");
}


void reconnect_wifi()
{
  Serial.print("WiFi connection lost. Reconnecting...");

  while (WiFi.begin(ssid, wifi_password) != WL_CONNECTED)
  {
   Serial.print(".");
   delay(5000);  // Retry every 5 seconds if connection fails
  }
}

void check_wifi_connection(){
  if (WiFi.status() != WL_CONNECTED){
       reconnect_wifi();
        Serial.print("WiFi reconnected");
  }
  delay(2400);
}

void loop() 
{ 
  bool wait_for_booking = true;
  while(wait_for_booking)
  {
    check_wifi_connection();

    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("Wait for booking");

    password = api_call("/pin");
    Serial.print(password);
    
    if(password != ""){
      wait_for_booking = false;
    }
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
          delay(300);
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
  
  
  bool door_is_unlocked = true;

  while(door_is_unlocked)
  {
    int doorState = digitalRead(13); // read state

    if (doorState == HIGH) {
      lcd.clear();
      lcd.print("Please close the door");
      delay(3000);
    } else {
      close_locker();
      door_is_unlocked = false;
      lcd.clear();
      lcd.print("Thank you!");
      delay(3000);
    }
  }
}