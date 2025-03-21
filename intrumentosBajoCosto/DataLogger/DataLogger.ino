#include "FS.h"
#include "SD.h"
#include <RTClib.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels


#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES 10

Adafruit_BME280 bme;  // Revisar SDA y SCL
Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
Adafruit_Sensor *bme_pressure = bme.getPressureSensor();
Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();

RTC_DS3231 rtc;

const int oneWireBus = 4;  // GPIO
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);  //para temperatura

String dataMessage = "Vacio";
String identificador = "BME280";


void setup() {
  Serial.begin(9600);
  sensors.begin();
  if (!rtc.begin()) {
    Serial.println("RTC module is NOT found");
    Serial.flush();
    while (1)
      ;
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

 // Serial.begin(9600);

  if (!bme.begin(0x76)) {
    while (1) delay(10);
  }

  bme_temp->printSensorDetails();
  bme_pressure->printSensorDetails();
  bme_humidity->printSensorDetails();

  initSDCard();

  File file = SD.open("/data_LC_interval.csv");
  if (!file) {
    Serial.println("Archivo no existe");
    Serial.println("Creating file...");
    // modificar el encabezado para incluir fecha y hora
    writeFile(SD, "/data_LC_interval.csv", "Identificador,Fecha,Hora,Temperatura ºC,Humedad %,Presion hPa,Temperatura  2 ºC \r\n");
  } else {
    Serial.println("Archivo ya existe");
  }
  file.close();
}

void loop() {
  DateTime now = rtc.now();
  String fecha = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year());
  String hora = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());

  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);

  sensors_event_t temp_event, pressure_event, humidity_event;
  bme_temp->getEvent(&temp_event);
  bme_pressure->getEvent(&pressure_event);
  bme_humidity->getEvent(&humidity_event);
  String dataMessage = identificador + "," + fecha + "," + hora + "," + temp_event.temperature + "," + humidity_event.relative_humidity + "," + pressure_event.pressure + "," + String(temperatureC) + "\r\n";

  // incluir fecha, hora, temperatura, humedad y presion en el mensaje de datos
  if (now.second() % 5 == 0) {
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
      for (;;)
        ;  // Don't proceed, loop forever
    }
    display.clearDisplay();
    display.setTextSize(1);               // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);  // Draw white text
    display.setCursor(0, 0);              // Start at top-left corner
    display.println(fecha + " " + hora);
    display.println("T1: " + String(temp_event.temperature) + " T2:" + String(temperatureC));
    display.println("Humedad: " + String(humidity_event.relative_humidity) + " %");
    display.println("Presion: " + String(pressure_event.pressure) + " hPa");
    display.display();
  }
  if (now.second() == 0) {
    Serial.print("Guardando: ");
    Serial.println(dataMessage);
    appendFile(SD, "/data_LC_interval.csv", dataMessage.c_str());
  }

  delay(1000);
}


// Initialize SD card
void initSDCard() {
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
}
// Write to the SD card
void writeFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}
// Append data to the SD card
void appendFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}