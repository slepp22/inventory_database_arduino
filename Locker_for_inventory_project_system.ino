#include <Wire.h>

//LCD SECTIOM
//https://wiki.seeedstudio.com/Grove-LCD_RGB_Backlight/
#include "rgb_lcd.h"

const int colorR = 255;
const int colorG = 0;
const int colorB = 0;


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



//PINS
rgb_lcd lcd;          //PIN I2C
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 





void setup() 
{
  pin_pad_setup();
  lcd_setup();
  delay(1000);
}

void pin_pad_setup()
{
  Serial.begin(9600);
}

void lcd_setup()
{
    lcd.begin(16, 2);
    lcd.setRGB(colorR, colorG, colorB);
}


void loop() 
{
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 0);
    // print the number of seconds since reset:
    lcd.print("Enter password");

    lcd.setCursor(0,1);

    

    char custom_key = customKeypad.getKey();

    bool password_correct = false;
  
  
    if (custom_key){

      if(custom_key == '*')
      {
        password_correct = (input_password == password);

        if(password_correct)
        {
          lcd.print("Password correct");
          delay(300);
        }
        else if(!password_correct)
        {
          lcd.print("password wrong");

        }
      }

  
      input_password += custom_key;
      lcd.print(input_password);
    }
    

    delay(200);
}





