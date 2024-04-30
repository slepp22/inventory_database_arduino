#include <Wire.h>

//-------------------------------INTERNET RELATED--------------------------------
//-------------------------------------------------------------------------------
//WIFI SECTION
#include <WiFiNINA.h>
char ssid[] = "Vodafone-C56C";
char wifi_password[] = "efrExETcc96JfzHr";
int status = WL_IDLE_STATUS; // WiFi status

//-------------------------------------------------------------------------------
//API SECTION 
WiFiClient client;
const char* server = "f-itplfo6nya-uc.a.run.app";
const int port = 443; // HTTPS port               // Port for HTTPS (443 for HTTPS)


//-------------------------------------------------------------------------------
//WEB CLOUD SECTION
#include "ThingSpeak.h"
unsigned long smart_room_channel_number = 2530450; // ThingSpeak Channel Number
const char * write_API_KEY = "VLBJI3P9ZWGDS2GS"; //ThingSpeak Write API Key


//-------------------------------SENSORS-----------------------------------------
//-------------------------------------------------------------------------------
//LCD SECTION
//https://wiki.seeedstudio.com/Grove-LCD_RGB_Backlight/
#include "rgb_lcd.h"
rgb_lcd lcd;          //PIN I2C
const int colorR = 0;
const int colorG = 255;
const int colorB = 0;

//-------------------------------------------------------------------------------
//SERVO MOTOR SECTION
//https://projecthub.arduino.cc/arduino_uno_guy/the-beginners-guide-to-micro-servos-ae2a30
#include <Servo.h>
const int SENSOR_MOTOR_PIN = 10;
Servo servo;

//-------------------------------------------------------------------------------
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
byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

//-------------------------------------------------------------------------------
//CURRENT SENSOR SECTION
//https://www.aeq-web.com/arduino-current-sensor-power-electricity-meter/
int current_sensor_scaling_factor = 7;
int voltage = 220; //Voltage of Power Grid
int CURRENT_SENSOR_PIN = A0;
float current_sensor_value = 0;
float current = 0;

//-------------------------------------------------------------------------------
//DOOR SENSOR
//https://arduinogetstarted.com/tutorials/arduino-door-sensor
const int DOOR_SENSOR_PIN = 13;

//-------------------------------------------------------------------------------
//PASSWORD SECTION
String password = ""; // will be set trough API
String input_password =""; //will be set from user with pin-pad

// API endpoint URLs
const char* sourceAPIURL = "https://f-itplfo6nya-uc.a.run.app/bookings/newest";
const char* destinationAPIURL = "https://f-itplfo6nya-uc.a.run.app/bookings/";

#include <HttpClient.h>
#include <ArduinoJson.h>

String json_payload = 
  "{"
  "\"time_start\": \"2024-04-30T15:03:03.218Z\","
  "\"time_end\": \"2024-04-30T15:03:03.218Z\","
  "\"active\": false,"
  "\"price\": 0,"
  "\"device_id\": 9,"
  "\"user_id\": 4,"
  "\"rent_charge\": \"rent\","
  "\"pin\": 1234"
  "}";




void api_call_booking() {
     if (client.connectSSL("f-itplfo6nya-uc.a.run.app", 443)) {
    Serial.println("Connected to server");

    // Prepare the HTTP PUT request with the endpoint and host
    client.print("PUT /bookings/31 HTTP/1.1\r\n");
    client.print("Host: f-itplfo6nya-uc.a.run.app\r\n");
    client.print("Connection: close\r\n");
    client.print("Content-Type: application/json\r\n");
    client.print("Content-Length: ");
    client.print(json_payload.length());
    client.print("\r\n\r\n");

    // Send the JSON payload in the HTTP request body
    client.print(json_payload);

    
    // Print the HTTP request to Serial for debugging
    Serial.println("Sending HTTP PUT request:");
    Serial.print("PUT /bookings/31 HTTP/1.1\r\n");
    Serial.print("Host: f-itplfo6nya-uc.a.run.app\r\n");
    Serial.print("Connection: close\r\n");
    Serial.print("Content-Type: application/json\r\n");
    Serial.print("Content-Length: ");
    Serial.print(json_payload.length());
    Serial.print("\r\n\r\n");
    Serial.println(json_payload);

    // Wait for the response from the server
    while (client.connected() || client.available()) {
       String line = client.readStringUntil('\n');
       Serial.println("Response from server:");
       Serial.println(line);
      }

    // Close the connection
    client.stop();
    Serial.println("Connection closed");
  } else {
    Serial.println("Connection to server failed!");
  }

  delay(5000); // Delay before sending the next request
}


void setup() {
  Serial.begin(9600);
  wifi_setup();

  //WEBCLOUD SETUP
  ThingSpeak.begin(client);

  //SENSOR SETUP
  pinMode(13, INPUT_PULLUP);
  servo_motor_setup();
  pin_pad_setup();
  lcd_setup();
  close_locker();
  delay(1000);
}

void loop() { 
  while(true){
    //api_call_booking();
    float current_watt = get_current_sensor_value();
    delay(10000);
  }
  
  
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
          delay(3000);
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

void sendToWebCloud(float wattage_data) {

  ThingSpeak.setField(1, wattage_data);

  int response = ThingSpeak.writeFields(smart_room_channel_number, write_API_KEY);

    if (response == 200) {
    Serial.println("Data sent to ThingSpeak successfully");
  } else {
    Serial.print("Error sending data to ThingSpeak. HTTP error code: ");
    Serial.println(response);
  }
}


void wifi_setup(){
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

void door_sensor_setup(){pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP); }

void pin_pad_setup(){Serial.begin(9600);}

void lcd_setup(){
    lcd.begin(16, 2);//Colums and rows to dedicated LED Screen
    lcd.setRGB(colorR, colorG, colorB);
}

void servo_motor_setup(){
  servo.attach(SENSOR_MOTOR_PIN);
  servo.write(0);
}

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

bool get_booking(){
  delay(300);
  bool booking = false;
  return booking;
}

void open_locker(){
  servo.write(90);
}

void close_locker(){
  servo.write(0);
}

float get_current_sensor_value(){
  int i = 0;
  current_sensor_value = 0;
  int times_sensor_read = 2000;
  
  while(i < times_sensor_read){
    int tmp_sensor_value = analogRead(CURRENT_SENSOR_PIN);
    current_sensor_value = current_sensor_value + tmp_sensor_value;
    i++;
  }

  current = (((current_sensor_value/times_sensor_read)*current_sensor_scaling_factor)/100); 

  Serial.print("Current: ");
  Serial.print(current);
  Serial.print(" A Watts: ");
  Serial.print(current*voltage);
  Serial.print("\n");
  return current*voltage;
}

void reconnect_wifi(){
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
