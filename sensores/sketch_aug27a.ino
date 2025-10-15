//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(1,0);
}

//pixeLEs 8*5
// columna dp fila
void loop()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Fingerspelling!");
  lcd.setCursor(1,1);
  lcd.print("la");
  lcd.setCursor(5,1);
  lcd.print("letra");
  lcd.setCursor(11,1);
  lcd.print("es:");
  lcd.setCursor(14,1);
  lcd.print("B"); }
