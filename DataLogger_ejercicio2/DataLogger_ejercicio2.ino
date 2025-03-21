//conexion a mysql
#define ESP32_MYSQL_DEBUG_PORT Serial
#define _ESP32_MYSQL_LOGLEVEL_ 1
#include <ESP32_MySQL.h>
#define USING_HOST_NAME true
#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 120       /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;



//************************ DS18B20 ************************
#include <OneWire.h>
#include <DallasTemperature.h>

const int oneWireBus = 4;  // GPIO
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
//********************************************************

//*********************** DHT ****************************
#include "DHT.h"       //    Libreria del sensor
#define DHTPIN 25      //GPIO
#define DHTTYPE DHT21  // Revisar si es 11,21,22

DHT dht(DHTPIN, DHTTYPE);  //Declaracion de Sensor
//********************************************************

//********************* BME280*****************************
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;  // Revisar SDA y SCL
Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
Adafruit_Sensor *bme_pressure = bme.getPressureSensor();
Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();

//MySQL
//conexion a SSID
const char *ssid = "LCCR";
const char *pass = "LCCR2025";
char user[] = "cr";
char password[] = "2025";
char server[] = "192.168.137.1";  //nombre del servidor

uint16_t server_port = 3306;  // puerto de conexión a MySQL
char database[] = "lc";       //base de datos
char table[] = "costarica";   //tabla

//valores a insertar
String identificador = "Equipo02";
String bmeD;
String dhtV;
String ds1820;
String ajustedht;
String ajusteds1820;
float ajustedht1 = 0.0;
float ajusteds18201 = 0.0;


//codigo sql para inserción de datos
//String INSERT_SQL = String("INSERT INTO ") + database + "." + table
 //                   + "( identificador, bme, dht, ds1820, ajustedht, ajusteds1820) VALUES ('" + identificador + "', '" + bmeD + "', '" + dhtV + "', '" + ds1820 + "', '" + ajustedht + "', '" + ajusteds1820 + "')";  //columnas= identificador y variable*/

String INSERT_SQL;
ESP32_MySQL_Connection conn((Client *)&client);
ESP32_MySQL_Query *query_mem;

//********************************************************

void setup() {
  Serial.begin(115200);
  sensors.begin();  //sensor DS18B20
  dht.begin();      //Arranque de sensor

  //****************BME280********************
  if (!bme.begin(0x76)) {
    Serial.println(F("No se ha encontrado el sensor BME280"));
    while (1) delay(10);
  }
  bme_temp->printSensorDetails();
  bme_pressure->printSensorDetails();
  bme_humidity->printSensorDetails();
  //******************************************


  //probar conexion a servidor de base de datos
  while (!Serial && millis() < 5000)
    ;  //esperando la conexion
  ESP32_MYSQL_DISPLAY1("\nStarting Basic_Insert_ESP on", ARDUINO_BOARD);
  // iniciar conexión wifi
  ESP32_MYSQL_DISPLAY1("Connecting to", ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    ESP32_MYSQL_DISPLAY0(".");
  }

  // imprimir info de la conexión
  ESP32_MYSQL_DISPLAY1("Connected to network. My IP address is:", WiFi.localIP());
  ESP32_MYSQL_DISPLAY3("Connecting to SQL Server @", server, ", Port =", server_port);
  ESP32_MYSQL_DISPLAY5("User =", user, ", PW =", password, ", DB =", database);

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
}

void runInsert() {
  ESP32_MySQL_Query query_mem = ESP32_MySQL_Query(&conn);
  if (conn.connected()) {
    ESP32_MYSQL_DISPLAY(INSERT_SQL);
    //ejecutando consulta
    if (!query_mem.execute(INSERT_SQL.c_str())) {
      ESP32_MYSQL_DISPLAY("Error de insercion");
    } else {
      ESP32_MYSQL_DISPLAY("Datos insertados");
    }
  } else {
    ESP32_MYSQL_DISPLAY("Desconectando del servidor. no se puede insertar");
  }
}

void loop() {
  //***********DS18B20******************************
  sensors.requestTemperatures();
  float temp_ds18b20 = sensors.getTempCByIndex(0);
  //************DHT**********************************
  float temp_dht = dht.readTemperature();
  //*****************BME280**********************************
  sensors_event_t temp_event, pressure_event, humidity_event;
  bme_temp->getEvent(&temp_event);
  bme_pressure->getEvent(&pressure_event);
  bme_humidity->getEvent(&humidity_event);

  float temperature1 = temp_event.temperature;

  //calibrar de acuerdo con BME280
  float ajuste1Result = temperature1 - temp_dht;
  float ajuste2Result = temperature1 - temp_ds18b20;

  //parametros a insertar en la tabla costarica
  bmeD = String(temp_event.temperature);
  dhtV = String(temp_dht);
  ds1820 = String(temp_ds18b20);
  ajustedht1 = ajuste1Result;
  ajusteds18201 = ajuste2Result;
  ajustedht = String(ajustedht1);
  ajusteds1820 = String(ajusteds18201);


  Serial.println("DHT: " + dhtV + " DS18B20: " + ds1820 + " BME280: " + bmeD + " Ajuste DHT: " + ajustedht + " Ajuste ds 1820:" + ajusteds1820);
  INSERT_SQL = String("INSERT INTO ") + database + "." + table
                    + "( identificador, bme, dht, ds1820, ajustedht, ajusteds1820) VALUES ('" + identificador + "', '" + bmeD + "', '" + dhtV + "', '" + ds1820 + "', '" + ajustedht + "', '" + ajusteds1820 + "')";  //columnas= identificador y variable*/
  //escribir en la base de datos 
  ESP32_MYSQL_DISPLAY("Conectando...");
  if (conn.connectNonBlocking(server, server_port, user, password) != RESULT_FAIL) {
    delay(500);
    runInsert();
    conn.close();  // cierra conexion a base de datos
  } else {
    ESP32_MYSQL_DISPLAY("\nConexion fallida.");
  }


  print_wakeup_reason();
  Serial.flush();
  esp_deep_sleep_start();
}

void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0: Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1: Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER: Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP: Serial.println("Wakeup caused by ULP program"); break;
    default: Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}
