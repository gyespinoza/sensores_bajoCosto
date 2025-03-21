#include <RTClib.h>

RTC_DS3231 rtc;

char daysOfWeek[7][12] = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

void setup () {
  Serial.begin(9600);
  if (! rtc.begin()) {
    Serial.println("RTC module is NOT found");
    Serial.flush();
    while (1);
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //Esta Linea Actualiza el Reloj con la hora de la computadora

}

void loop () {//Aqui revisaremos que la hora se haya guardado
  DateTime now = rtc.now();
  Serial.print("ESP32 RTC Date Time: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);
  delay(1000);
}