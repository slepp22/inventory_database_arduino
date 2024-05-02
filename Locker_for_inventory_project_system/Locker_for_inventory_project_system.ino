#include <Wire.h>

//-------------------------------INTERNET RELATED--------------------------------
//-------------------------------------------------------------------------------
//WIFI SECTION
#include <WiFiNINA.h>
char ssid[] = "locker";
char wifi_password[] = "locker1234";
int status = WL_IDLE_STATUS;  // WiFi status

//-------------------------------------------------------------------------------
//API SECTION
WiFiClient client;
const char* server = "f-itplfo6nya-uc.a.run.app";
const int port = 443;  // HTTPS port               // Port for HTTPS (443 for HTTPS)


//-------------------------------------------------------------------------------
//THINGSPEAK SECTION
#include "ThingSpeak.h"
unsigned long smart_room_channel_number = 2530450;  // ThingSpeak Channel Number
const char* write_API_KEY = "VLBJI3P9ZWGDS2GS";     //ThingSpeak Write API Key


//-------------------------------SENSORS-----------------------------------------
//-------------------------------------------------------------------------------
//LCD SECTION
//https://wiki.seeedstudio.com/Grove-LCD_RGB_Backlight/
#include "rgb_lcd.h"
rgb_lcd lcd;  //PIN I2C
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
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 9, 8, 7, 6 };
byte colPins[COLS] = { 5, 4, 3, 2 };
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//-------------------------------------------------------------------------------
//CURRENT SENSOR SECTION
//https://www.aeq-web.com/arduino-current-sensor-power-electricity-meter/
int current_sensor_scaling_factor = 7;
int voltage = 220;  //Voltage of Power Grid
int CURRENT_SENSOR_PIN = A1;
float current_sensor_value = 0;
float current = 0;

//-------------------------------------------------------------------------------
//DOOR SENSOR
//https://arduinogetstarted.com/tutorials/arduino-door-sensor
const int DOOR_SENSOR_PIN = 13;

//-------------------------------------------------------------------------------
//PASSWORD SECTION
String CURRENT_PASSWORD_FROM_API = "1337";
int CURRENT_RENTER_ID = 0;
String PREVIOUS_PASSWORD_FROM_API = "1337";
int password_enter_counter = 0;
String input_password = "";  //will be set from user with pin-pad

// API endpoint URLs

#include <ArduinoJson.h>  // Include ArduinoJSON library
#include <HttpClient.h>


struct CURRENT_BOOKING {
  // Member variables (fields) of the struct
  String time_start;
  String time_end;
  bool active;
  float price;
  int device_id;
  int user_id;
  String rent_charge;
  int pin;
  int id;
};

CURRENT_BOOKING current_booking;

void door_setup(){
  open_locker();
  delay(5000);
  int doorState = digitalRead(13);
  do{
    doorState = digitalRead(13);
    lcd.setCursor(0, 0);
    lcd.clear();
    if(doorState==HIGH)
    {
      lcd.print("Close door !");
      delay(5000);
    }
  }while(doorState == HIGH);

  close_locker();
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("Door done... !");
  delay(1000);
}

void setup() {
  Serial.begin(9600);
  lcd_setup();

  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("Setup starts... !");
  delay(5000);
  

  //SENSOR SETUP
  servo_motor_setup();
  pin_pad_setup();
 
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("Setup Door... !");
  door_setup();

  //WIFI SETUP
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("Setup Wifi... !");
  wifi_setup();
  //ThingSpeak SETUP
  ThingSpeak.begin(client);

  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("Setup Done!");

  //close_locker();
  delay(5000);
}

void loop() {
  do {
    check_wifi_connection();
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("Check booking...");
    api_get_latest_booking();  //with this comes info like Password  

    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("Wait for User!");
    unsigned long startTime = millis();  // Get the current time in milliseconds
    char custom_key = NO_KEY;            // Initialize custom_key to NO_KEY (no key pressed)

    // Loop and check for keypad input for up to 10 seconds (10000 milliseconds)
    while (millis() - startTime < 20000) {  // Continue loop until 10 seconds have elapsed
      custom_key = customKeypad.getKey();   // Check for keypad input
      if (custom_key == '#') {
        bool password_correct = user_pin_pad_input();
        if (password_correct) {
          open_locker();
          password_enter_counter += 1;
          if(password_enter_counter == 2){
            lcd.setCursor(0, 0);
            lcd.clear();
            lcd.print("Take your item");
            api_update_booking(CURRENT_RENTER_ID); 
            closing_door_user();
          }
          else {
          delay(2000);
          lcd.setCursor(0, 0);
          lcd.clear();
          lcd.print("Place your item");
          delay(5000);
          closing_door_user();
          }  
        } else if (!password_correct) {
          lcd.setCursor(0, 0);
          lcd.clear();
          lcd.print("wrong 3/3");
        }
      }
    }
    //ONLY SEND, WHEN CHARGE
    float current_watt = get_current_sensor_value();
    send_to_thingspeak(current_watt);
    delay(3000);
  } while (CURRENT_PASSWORD_FROM_API == PREVIOUS_PASSWORD_FROM_API);
}

void closing_door_user(){
   int doorState = digitalRead(13);  // read state
   do {
        doorState = digitalRead(13);  // read state

        if (doorState == HIGH) {  //High == open
          lcd.setCursor(0, 0);
          lcd.clear();
          lcd.print("Close door !");
          delay(5000);
        } else if (doorState == LOW){  //Low == closed
          lcd.setCursor(0, 0);
          lcd.clear();
          lcd.print("Thank you ! ");
          close_locker();
          delay(5000);
        }
      } while (doorState == HIGH);
}
void wifi_setup() {
  while (!Serial) {
    ;  // Wait for serial port to connect
  }
  // Attempt to connect to WiFi network
  Serial.print("Attempting to connect to WiFi network: ");
  Serial.println(ssid);

  while (WiFi.begin(ssid, wifi_password) != WL_CONNECTED) {
    Serial.print(".");
    delay(5000);  // Retry every 5 seconds if connection fails
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void api_update_booking(int bookingId) {
  // Create a DynamicJsonDocument for holding the JSON data
  DynamicJsonDocument doc(200);  // Adjust the capacity based on your JSON payload size
  doc["time_start"] = current_booking.time_start;
  doc["time_end"] = current_booking.time_end;
  doc["active"] = false;
  doc["booking_price"] = current_booking.price;
  doc["device_id"] = current_booking.device_id;
  doc["user_id"] = current_booking.user_id;
  doc["rent_charge"] = current_booking.rent_charge;
  doc["pin"] = current_booking.pin;
  doc["id"] = current_booking.id;

  // Serialize the JSON object into a string
  String json_payload;
  serializeJson(doc, json_payload);

  // Establish connection to the server
  if (client.connectSSL("f-itplfo6nya-uc.a.run.app", 443)) {
    Serial.println("Connected to server");

    // Prepare the HTTP PUT request with the endpoint and host
    client.print("PUT /bookings/");
    client.print(current_booking.id);  // Append the booking ID
    client.print(" HTTP/1.1\r\n");
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
    Serial.print("PUT /bookings/");
    Serial.print(current_booking.id);  // Append the booking ID
    Serial.print(" HTTP/1.1\r\n");
    Serial.print("Host: f-itplfo6nya-uc.a.run.app\r\n");
    Serial.print("Connection: close\r\n");
    Serial.print("Content-Type: application/json\r\n");
    Serial.print("Content-Length: ");
    Serial.print(json_payload.length());
    Serial.print("\r\n\r\n");
    Serial.println(json_payload);

    int counter = 0;
    // Wait for the response from the server
    while (client.connected() || client.available()) {
      String line = client.readStringUntil('\n');
      Serial.println("Response from server:");
      Serial.println(line);

      counter += 1;
      if (counter == 3) {
        break;
      }
    }

    // Close the connection
    client.stop();
    Serial.println("Connection closed");
  } else {
    Serial.println("Connection to server failed!");
  }

  delay(5000);  // Delay before sending the next request
}

void api_get_latest_booking() {
  bool jsonProcessed = false;
  // Establish connection to the server
  if (client.connectSSL("f-itplfo6nya-uc.a.run.app", 443)) {
    Serial.println("Connected to server");

    // Prepare the HTTP GET request with the endpoint and host
    client.print("GET /bookings/newest/");
    client.print(" HTTP/1.1\r\n");
    client.print("Host: f-itplfo6nya-uc.a.run.app\r\n");
    client.print("Connection: close\r\n");
    client.print("\r\n");  // No additional headers or payload for a GET request

    // Print the HTTP request to Serial for debugging
    Serial.println("Sending HTTP GET request:");
    Serial.print("GET /bookings/newest/");
    Serial.print(" HTTP/1.1\r\n");
    Serial.print("Host: f-itplfo6nya-uc.a.run.app\r\n");
    Serial.print("Connection: close\r\n");
    Serial.print("\r\n");

    // Wait for the response from the server
    while (client.connected() || client.available()) {
      String line = client.readStringUntil('\n');
      Serial.println("Response from server:");
      Serial.println(line);

      // Check if the response contains JSON data
      if (line.startsWith("{")) {
        // Parse the JSON response
        DynamicJsonDocument responseDoc(200);  // Adjust capacity based on response size
        DeserializationError error = deserializeJson(responseDoc, line);

        // Check if parsing was successful
        if (!error) {
          String time_start = responseDoc["time_start"].as<String>();  // Convert JSON value to String
          String time_end = responseDoc["time_end"].as<String>();
          bool active = responseDoc["active"].as<bool>();
          float price = responseDoc["price"].as<float>();
          int device_id = responseDoc["device_id"].as<int>();
          int user_id = responseDoc["user_id"].as<int>();
          String rent_charge = responseDoc["rent_charge"].as<String>();
          int pin = responseDoc["pin"].as<int>();
          int id = responseDoc["id"].as<int>();  // Assuming "id" is the correct field name

          // Assign parsed values to the struct member variables
          current_booking.time_start = time_start;
          current_booking.time_end = time_end;
          current_booking.active = active;
          current_booking.price = price;
          current_booking.device_id = device_id;
          current_booking.user_id = user_id;
          current_booking.rent_charge = rent_charge;
          current_booking.pin = pin;
          current_booking.id = id;

          CURRENT_PASSWORD_FROM_API = String(pin);  // Convert int to String

          // Access other fields as needed...

          // Print parsed data to Serial for debugging
          Serial.println("Parsed JSON response:");
          Serial.print("time_start: ");
          Serial.println(time_start);
          Serial.print("active: ");
          Serial.println(active);
          Serial.print("price: ");
          Serial.println(price);
          break;
          // Print other fields as needed...
        } else {
          Serial.println("Failed to parse JSON response!");
        }
      }
    }

    // Close the connection
    client.stop();
    Serial.println("Connection closed");
  } else {
    Serial.println("Connection to server failed!");
  }

  delay(5000);  // Delay before sending the next request
}

bool user_pin_pad_input() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter password");
  lcd.setCursor(0, 1);


  int counter_password_entries = 0;
  bool wait_for_user_input = true;

  while (wait_for_user_input) {
    char custom_key = customKeypad.getKey();
    bool password_correct = false;

    if (custom_key) {

      if (custom_key == '*') {
        password_correct = (input_password == CURRENT_PASSWORD_FROM_API);

        if (password_correct) {
          input_password = "";  //reset password entry
          return true;
        } else if (!password_correct) {
          counter_password_entries += 1;

          lcd.print("password wrong");
          lcd.clear();
          lcd.print("try again ");
          lcd.print(String(counter_password_entries));  // Convert integer to string
          lcd.print("/3");
          lcd.setCursor(0, 1);
          input_password = "";  //reset password entry

          if (counter_password_entries == 3) {
            return false;
          }
        }
      }
      if (custom_key != '*') {
        input_password += custom_key;
        lcd.print(custom_key);
      }
    }
  }
}

void send_to_thingspeak(float wattage_data) {

  ThingSpeak.setField(1, wattage_data);

  int response = ThingSpeak.writeFields(smart_room_channel_number, write_API_KEY);

  if (response == 200) {
    Serial.println("Data sent to ThingSpeak successfully");
  } else {
    Serial.print("Error sending data to ThingSpeak. HTTP error code: ");
    Serial.println(response);
  }
}

void door_sensor_setup() {
  pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP);
}

void pin_pad_setup() {
  Serial.begin(9600);
}

void lcd_setup() {
  lcd.begin(16, 2);  //Colums and rows to dedicated LED Screen
  lcd.setRGB(colorR, colorG, colorB);
}

void servo_motor_setup() {
  servo.attach(SENSOR_MOTOR_PIN);
  servo.write(0);
}

bool get_booking() {
  delay(300);
  bool booking = false;
  return booking;
}

void open_locker() {
  servo.write(90);
}

void close_locker() {
  servo.write(0);
}

float get_current_sensor_value() {
  int i = 0;
  current_sensor_value = 0;
  int times_sensor_read = 2000;

  while (i < times_sensor_read) {
    int tmp_sensor_value = analogRead(CURRENT_SENSOR_PIN);
    current_sensor_value = current_sensor_value + tmp_sensor_value;
    i++;
  }

  current = (((current_sensor_value / times_sensor_read) * current_sensor_scaling_factor) / 100);

  Serial.print("Current: ");
  Serial.print(current);
  Serial.print(" A Watts: ");
  Serial.print(current * voltage);
  Serial.print("\n");
  return current*voltage;
}

void reconnect_wifi() {
  Serial.print("WiFi connection lost. Reconnecting...");

  while (WiFi.begin(ssid, wifi_password) != WL_CONNECTED) {
    Serial.print(".");
    delay(5000);  // Retry every 5 seconds if connection fails
  }
}

void check_wifi_connection() {
  if (WiFi.status() != WL_CONNECTED) {
    reconnect_wifi();
    Serial.print("WiFi reconnected");
  }
  delay(2400);
}
