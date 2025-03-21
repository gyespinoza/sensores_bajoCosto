#include <OneWire.h>
#include <DallasTemperature.h>
#define PinLed 16 //definir el pin del led

const int oneWireBus = 4;    // GPIO  
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

void setup() {
  pinMode(PinLed, OUTPUT);
  Serial.begin(115200);
  sensors.begin();
}

void loop() {
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  float temperatureF = sensors.getTempFByIndex(0);
  Serial.print(temperatureC);
  Serial.println("ºC");
  Serial.print(temperatureF);
  Serial.println("ºF");

   //validacion de temperatura
  if(temperatureC >= 30){
    digitalWrite(PinLed, HIGH);
  }else{
    digitalWrite(PinLed, LOW);
  }

  delay(500); 

}