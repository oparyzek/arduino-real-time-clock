#include <U8g2lib.h>
#include <Wire.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0); // U8g2 knihovna pro SH1106 OLED displej

#define DS1307_ADDRESS 0x68 // Adresa RTC modulu

byte decToBcd(byte val) {
  return ((val / 10 * 16) + (val % 10));
}

byte bcdToDec(byte val) {
  return ((val / 16 * 10) + (val % 16));
}

void setup() {
  Wire.begin(); // Inicializace I2C sběrnice

  u8g2.begin(); // Inicializace OLED displeje

  // Nastavení data a času na hodnoty kompilace
  setDateTime(__DATE__, __TIME__);
}

void loop() {
  showDateTime();
  delay(1000); // Zpoždění 1 sekunda
}

void setDateTime(const char* date, const char* time) {
  int year, month, day, hour, minute, second;

  sscanf(date, "%*s %s %d %d", month, &day, &year);
  sscanf(time, "%d:%d:%d", &hour, &minute, &second);

  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(0); // Nastavení adresy začátku dat v RTC modulu
  Wire.write(decToBcd(second));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(decToBcd(0)); // Den týdne (není použito)
  Wire.write(decToBcd(day));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year - 2000));
  Wire.endTransmission();
}

void showDateTime() {
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(0); // Nastavení adresy začátku dat v RTC modulu
  Wire.endTransmission();

  Wire.requestFrom(DS1307_ADDRESS, 7);

  int second = bcdToDec(Wire.read() & 0x7F);
  int minute = bcdToDec(Wire.read());
  int hour = bcdToDec(Wire.read() & 0x3F);
  Wire.read(); // Den týdne (není použito)
  int day = bcdToDec(Wire.read());
  int month = bcdToDec(Wire.read() & 0x1F);
  int year = bcdToDec(Wire.read()) + 2000;

  char dateTimeStr[9];
  sprintf(dateTimeStr, "%02d:%02d:%02d", hour, minute, second);

  char dateStr[11];
  sprintf(dateStr, "%02d/%02d/%04d", day, month, year);

  u8g2.clearBuffer(); // Vymaže obsah bufferu displeje
  u8g2.setFont(u8g2_font_profont12_tf); // Nastaví font
  u8g2.setCursor(0, 20); // Nastaví pozici kurzoru pro čas
  u8g2.print(dateTimeStr); // Zobrazí čas
  u8g2.setCursor(0, 40); // Nastaví pozici kurzoru pro datum
  u8g2.print(dateStr); // Zobrazí datum
  u8g2.sendBuffer(); // Odeslání dat na displej
}
