/*
 
  An adaption of https://github.com/LilyGO/TTGO-T4-DEMO to run on the https://github.com/Xinyuan-LilyGO/TTGO-T-Display

  Liked the idea and have the https://github.com/Xinyuan-LilyGO/TTGO-T-Display so adapted to suit.

  Remember to update the wifi stuff before uploading ;) Happy coding!
  

  The MIT License (MIT)

  Copyright © 2018 Médéric NETTO

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <ArduinoJson.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <SPI.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_ILI9341.h>
#include <TFT_eSPI.h>
#include <Button2.h>
#include <TimeLib.h>

#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN 0x10
#endif

#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 23

#define TFT_BL 3  // Display backlight control pin
#define ADC_EN 14
#define ADC_PIN 34
#define BUTTON_1 35
#define BUTTON_2 0

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

const char* ssid = "Your WiFi SSID";
const char* password = "Your WiFi Password";

const char host[] = "api.coinmarketcap.com";

// Colors
int TFT_COLOR;
#define CUSTOM_DARK TFT_BLACK // 0x4228 // Background color

// Bitmap_WiFi
extern uint8_t wifi_1[];
extern uint8_t wifi_2[];
extern uint8_t wifi_3[];

// Bitmap_CryptoMonnaies
extern uint8_t bitcoin[];
extern uint8_t ethereum[];
extern uint8_t litecoin[];
extern uint8_t ripple[];
extern uint8_t cardano[];

unsigned long previousMillis = 0;
long interval = 0;

int coin = 0;
String crypto[] = {"bitcoin", "ethereum", "ripple", "litecoin", "cardano"};
String oldPrice[5];

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setTextWrap(true);
  tft.setCursor(0, 0);
  //  tft.setTextDatum(MC_DATUM);
  //  tft.setTextSize(1);
  if (TFT_BL > 0) { // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    pinMode(TFT_BL, OUTPUT); // Set backlight pin to output mode
    digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
  }
  tft.println(">>> Connecting to: ");
  tft.println(" ");
  tft.println(ssid);
  Serial.println("start");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    tft.drawBitmap(12, 100, wifi_1, 100, 100, TFT_WHITE);
    delay(200);
    tft.drawBitmap(12, 100, wifi_2, 100, 100, TFT_WHITE);
    delay(200);
    tft.drawBitmap(12, 100, wifi_3, 100, 100, TFT_WHITE);
    delay(200);
    tft.fillRect(12, 100, 100, 100, CUSTOM_DARK);
  }
  //Serial.println("start");
  tft.println(" ");
  tft.print(">>> WiFi connected");
  //tft.print("IP address: ");
  //tft.println(WiFi.localIP());
  delay(1500);
  tft.fillScreen(CUSTOM_DARK); // Clear Screen
  tft.setTextColor(TFT_BLUE);
  tft.setCursor(0, 150);
  tft.setTextSize(1);
  tft.println("CryptoMoneyTicker_v1");
  tft.drawLine(0, 130, 135, 130, TFT_WHITE);
  tft.drawLine(0, 185, 135, 185, TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(5, 207);
  tft.setTextColor(TFT_WHITE);
  tft.println("Data from:");
  tft.setCursor(5, 217);
  tft.setTextColor(TFT_WHITE);
  tft.println("CoinMarketCap.com");
}

void loop() {
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(1, 10);
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    interval = 30000; //                              <<<--------------------
    if (coin == 5 ) {
      coin = 0;
    }
    //Serial.print(">>> Connecting to ");
    //Serial.println(host);
    WiFiClientSecure client;
    const int httpsPort = 443;
    if (!client.connect(host, httpsPort)) {
      tft.fillScreen(CUSTOM_DARK);
      tft.println(">>> Connection failed");
      return;
    }
    //Serial.print("Requesting URL: ");
    //Serial.println("Connected to server!");
    client.println("GET /v1/ticker/" + crypto[coin] + "/ HTTP/1.1");
    client.println("Host: api.coinmarketcap.com");
    client.println("Connection: close");
    client.println();
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        tft.fillScreen(CUSTOM_DARK);
        tft.println(">>> Client Timeout!");
        client.stop();
        return;
      }
    }
    String data;
    while (client.available()) {
      data = client.readStringUntil('\r');
      //Serial.println(data);
    }
    data.replace('[', ' ');
    data.replace(']', ' ');
    char buffer[data.length() + 1];
    data.toCharArray(buffer, sizeof(buffer));
    buffer[data.length() + 1] = '\0';
    const size_t bufferSize = JSON_OBJECT_SIZE(15) + 110;
    DynamicJsonBuffer jsonBuffer(bufferSize);
    JsonObject& root = jsonBuffer.parseObject(buffer);
    if (!root.success()) {
      tft.println("parseObject() failed");
      return;
    }
    String name = root["name"]; // "Bitcoin"
    String symbol = root["symbol"]; // "BTC"
    String price_usd = root["price_usd"]; // "573.137"
    int stringLength = price_usd.length();
    int findPeriod = price_usd.indexOf('.');
    price_usd.remove(findPeriod + 5); // Remove everything 4 numbers after the period
    String percent_change_1h = root["percent_change_1h"]; // "0.04"
    String last_updated = root["last_updated"]; // "1472762067" <-- Unix Time Stamp
    String error = root["error"]; // id not found
    printTransition();
    switch (coin) {
      case 0 : // Bitcoin
        tft.drawBitmap(5, 5, bitcoin, 45, 45, TFT_ORANGE);
        printName(name, symbol);
        printPrice(price_usd);
        printChange(percent_change_1h);
        printTime(last_updated);
        printPagination();
        printError(error);
        tft.fillCircle(42, 230, 4, TFT_WHITE);
        break;
      case 1 : // Ethereum
        tft.drawBitmap(5, 5, ethereum, 45, 45, TFT_DARKGREY);
        printName(name, symbol);
        printPrice(price_usd);
        printChange(percent_change_1h);
        printTime(last_updated);
        printPagination();
        printError(error);
        tft.fillCircle(52, 230, 4, TFT_WHITE);
        break;
      case 2 : // Ripple
        tft.drawBitmap(5, 5, ripple, 45, 45, TFT_NAVY);
        printName(name, symbol);
        printPrice(price_usd);
        printChange(percent_change_1h);
        printTime(last_updated);
        printPagination();
        printError(error);
        tft.fillCircle(62, 230, 4, TFT_WHITE);
        break;
      case 3 : // Litecoin
        tft.drawBitmap(5, 5, litecoin, 45, 45, TFT_LIGHTGREY);
        printName(name, symbol);
        printPrice(price_usd);
        printChange(percent_change_1h);
        printTime(last_updated);
        printPagination();
        printError(error);
        tft.fillCircle(72, 230, 4, TFT_WHITE);
        break;
      case 4 : // Cardano
        tft.drawBitmap(5, 5, cardano, 45, 45, TFT_CYAN);
        printName(name, symbol);
        printPrice(price_usd);
        printChange(percent_change_1h);
        printTime(last_updated);
        printPagination();
        printError(error);
        tft.fillCircle(82, 230, 4, TFT_WHITE);
        break;
    }
    oldPrice[coin] = price_usd;
    coin++;
  }
}

void printName(String name, String symbol) {
  tft.setTextSize(1);
  tft.setCursor(65, 10);
  tft.println(name);
  tft.setTextSize(1);
  tft.setCursor(65, 33);
  tft.print(symbol);
  tft.drawLine(65, 54, 240, 54, TFT_WHITE);
}

void printPrice(String price_usd) {
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(5, 75);
  tft.print("Price:");
  if (price_usd != oldPrice[coin]) {
    if (price_usd > oldPrice[coin]) {
      TFT_COLOR = TFT_GREENYELLOW;
    } else {
      TFT_COLOR = TFT_RED;
    }
  }
  tft.setTextSize(2);
  tft.setTextColor(TFT_COLOR);
  tft.setCursor(5, 110);
  tft.print("$");
  tft.println(price_usd);
}

void printChange(String percent_change_1h) {
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(5, 150);
  tft.print("Change(1h):");
  if (percent_change_1h >= "0") {
    tft.setTextColor(TFT_GREENYELLOW);
  } else {
    tft.setTextColor(TFT_RED);
  }
  tft.setTextSize(2);
  tft.setCursor(35, 175);
  tft.print(percent_change_1h);
  tft.print("%");
}

void printTime(String last_updated) {
  long int timeData = last_updated.toInt();
  time_t t = timeData;
  //time_t Z = 1515675789;
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(5, 210);
  tft.print("Last Updated:");
  tft.setTextSize(1);
  tft.setCursor(100, 210);
  /*
    printDigits(day(t));
    tft.print("/");
    printDigits(month(t));
    tft.print("/");
    tft.print(year(t));
    tft.print(" ");
  */
  printDigits(hour(t) + 1); // +1 for the French time
  tft.print(":");
  printDigits(minute(t));
  //tft.print(":");
  //printDigits(second(t));
}

void printPagination() {
  tft.drawCircle(42, 230, 4, TFT_WHITE);
  tft.drawCircle(52, 230, 4, TFT_WHITE);
  tft.drawCircle(62, 230, 4, TFT_WHITE);
  tft.drawCircle(72, 230, 4, TFT_WHITE);
  tft.drawCircle(82, 230, 4, TFT_WHITE);
}

void printError(String error) {
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(65, 22);
  tft.println(error);
}

void printTransition() {
  tft.fillScreen(CUSTOM_DARK);
  yield();
  tft.fillScreen(TFT_RED);
  yield();
  tft.fillScreen(TFT_GREEN);
  yield();
  tft.fillScreen(TFT_BLUE);
  yield();
  tft.fillScreen(CUSTOM_DARK);
  yield();
}

void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  if (digits < 10)
    tft.print('0');
  tft.print(digits);
}
