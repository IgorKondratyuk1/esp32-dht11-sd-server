#include <Arduino.h>
#include <U8g2lib.h>

extern char todayDate[10];
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

void configU8G2Lib() {
  if (!u8g2.begin()) {
    Serial.println("Failed u8g2 start");
    while (1);
  }
  Serial.println("u8g2 started!");
}

void displayValuesOnScreen(float tempTemperature, float tempHumidity) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  u8g2.setCursor(0, 10);
  u8g2.printf("Today: %s", todayDate);

  u8g2.setCursor(0, 25);
  u8g2.printf("Temperature: %.2f", tempTemperature);

  u8g2.setCursor(0, 35);
  u8g2.printf("Humidity: %.2f", tempHumidity);

  u8g2.sendBuffer();
}