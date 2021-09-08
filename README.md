# woomam-arduino



우아한 코끼리 아두이노 담당 이유리(@zzyury)입니다.

H/W는 Arduino Uno 보드에 ESP8266, LCD, Solenoid, Pull up button를 추가하였고, Arduino IDE를 사용하였습니다.



## WiFi

> ESP8266 WiFi module과 연결할 WiFi의 이름과 비밀번호를 입력합니다. 

```
const char* ssid = "alpha3173 5G";
const char* password = "31733173";
```

>  비트레이트를 115200으로 맞춰주고 WiFi를 연결합니다.

```
void setup() {

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting...");
  }
```

> Arduino JSON을 이용하여 http 통신을 진행합니다. GET으로 data를 요청하여 storeUID, washingMachineUID, phoneNumber, taskFrom, taskTo, bookedTime, qrState, arduinoState와 washingMachineState까지 총 9개의 data를 받아옵니다.

```
    HTTPClient http; //Object of class HTTPClient
    http.begin("http://35.213.84.65");
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
      const char* washingMachineState = doc[“washingMachineState"];
    }
```


## LCD
1. 첫번째 줄은 예약한 User의 핸드폰 번호 마지막 4자리를 보여줍니다.
2. 두번째 줄은 남은 세탁 시간을 보여줍니다.


#### User phone number
> JSON으로 받아온 data 중 phoneNumber를 print합니다.

```
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("USER : "); lcd.print(phoneNumber);
```


#### Time left 

> JSON으로 받아온 data 중 taskFrom과 taskTo를 이용해 남은 시간을 계산합니다.

```
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
```

> 계산된 남은시간 timeleft를 print합니다.

```
lcd.setCursor(0, 1);
lcd.print("Time : "); lcd.print(timeleft);
```

## PULL UP BUTTON w/ SOLENOID

> 본인인증을 하기 전, solenoid는 잠금 상태를 유지합니다.
본인인증을 마친 후, pull up button이 눌릴 경우에는 solenoid 잠금이 풀리며 pull up button을 놓으면 다시 잠깁니다.

```
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
```
