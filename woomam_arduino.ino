// i2C 통신을 위한 라이브러리
#include <Wire.h>

// LCD module
#include <LiquidCrystal_I2C.h> // LCD 1602 I2C용 라이브러리
LiquidCrystal_I2C lcd(0x27, 16, 2); // 접근주소: 0x3F or 0x27

// wifi module
#include <SoftwareSerial.h>
#define BT_RXD 2
#define BT_TXD 3
SoftwareSerial ESP_wifi(BT_RXD, BT_TXD);

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
// #include <ArduinoJson.h> // const char* -> void char* error : https://github.com/bblanchon/ArduinoJson/issues/1381
#include <Arduino_JSON.h>

const char* ssid = "alpha3173 5G";
const char* password = "31733173";

// time counter
int time_lcd[3] = {0,};

// set up
void setup() {

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting...");
  }

  lcd.init(); // LCD 초기화
  lcd.backlight(); // 백라이트 켜기

  pinMode(9, INPUT_PULLUP);
  pinMode(13, OUTPUT);
}

// loop

void loop() {

  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http; //Object of class HTTPClient
    http.begin("http://35.213.84.65/wms/one/uuid");
    int httpCode = http.GET();

    if (httpCode > 0)
    {
      String jsonBodyString = http.getString();
      int str_len = jsonBodyString.length() + 1;
      char docToChar[str_len];
      jsonBodyString.toCharArray(docToChar, str_len);
      JSONVar doc = JSON.parse(docToChar);
      // .hasOwnProperty("key") => true 
      const char* storeUID = doc["storeUID"];
      const char* washingMachineUID = doc["washingMachineUID"];
      const char* phoneNumber = doc["phoneNumber"];
      const char* taskFrom = doc["taskFrom"];
      const char* taskTo = doc["taskTo"];
      const char* bookedTime = doc["bookedTime"];
      const char* qrState = doc["qrState"];
      const char* arduinoState = doc["arduinoState"];
      const char* washingMachineState = doc["washingMachineState"];

      //const char* dateStr = taskTo - taskFrom;
      // get taskTo into digits
      int toYear, toMonth, toDay, toHour, toMinute, toSecond;
      int fromYear, fromMonth, fromDay, fromHour, fromMinute, fromSecond;

      sscanf(taskTo, "%d-%d-%dT%d:%d:%fZ", &toYear, &toMonth, &toDay, &toHour, &toMinute, &toSecond);
      sscanf(taskFrom, "%d-%d-%dT%d:%d:%fZ", &fromYear, &fromMonth, &fromDay, &fromHour, &fromMinute, &fromSecond);

      long finish = toHour * 3600 + toMinute * 60 + toSecond;
      long start = fromHour * 3600 + fromMinute * 60 + fromSecond;

      long delta = finish - start;
      int hours = delta / 3600;
      delta = delta - hours * 3600;
      int minutes = delta / 60;
      delta = delta - minutes * 60;
      int seconds = delta;
    
      time_lcd[0] = seconds;
      time_lcd[1] = minutes;
      time_lcd[2] = hours;
      
      char* timeleft = "10:10:10";
      sprintf(timeleft, "%d:%d:%d", time_lcd[2], time_lcd[1], time_lcd[0]);

      // LCD
      lcd.clear();
      lcd.setCursor(0, 0); // 1번째, 1라인
      lcd.print("USER : "); lcd.print(phoneNumber);
      lcd.setCursor(0, 1); // 1번째, 2라인
      lcd.print("Time : "); lcd.print(timeleft);

      // Pull-up button with solenoid
      int sensorVal = digitalRead(9);
      if (arduinoState == "closed") {
        digitalWrite(13, HIGH);
      } else {
        if (sensorVal == HIGH) {
          digitalWrite(13, HIGH); // turn the SOLENOID on (HIGH is the voltage level)
        } else {
          digitalWrite(13, LOW); // turn the SOLENOID off by making the voltage LOW
        }
      }

    }
    http.end(); //Close connection
  }
}
