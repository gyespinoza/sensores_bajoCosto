#define ESP32_MYSQL_DEBUG_PORT      Serial
#define _ESP32_MYSQL_LOGLEVEL_      1
#include <ESP32_MySQL.h>
#define USING_HOST_NAME     true


//conexion a SSID
const char* ssid = "LCCR";
const char* pass = "LCCR2025";
char user[] = "cr";
char password[] = "2025";
char server[] = "192.168.137.1"; //nombre del servidor

uint16_t server_port = 3306;   // puerto de conexión a MySQL
char database[] = "lc";  //base de datos
char table[]    = "costarica"; //tabla

//valores a insertar
String identificador = "ID01";
String variable = "Variable ID01";


//codigo sql para inserción de datos
String INSERT_SQL = String("INSERT INTO ") + database + "." + table 
                 + "( identificador, variable) VALUES ('" + identificador + "', '"+ variable +"')"; //columnas= identificador y variable

ESP32_MySQL_Connection conn((Client *)&client);
ESP32_MySQL_Query *query_mem;

void setup()
{
  Serial.begin(115200);
  while (!Serial && millis() < 5000); //esperando la conexion
  ESP32_MYSQL_DISPLAY1("\nStarting Basic_Insert_ESP on", ARDUINO_BOARD);
  // iniciar conexión wifi
  ESP32_MYSQL_DISPLAY1("Connecting to", ssid);  
  WiFi.begin(ssid, pass);  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    ESP32_MYSQL_DISPLAY0(".");
  }

  // imprimir info de la conexión
  ESP32_MYSQL_DISPLAY1("Connected to network. My IP address is:", WiFi.localIP());
  ESP32_MYSQL_DISPLAY3("Connecting to SQL Server @", server, ", Port =", server_port);
  ESP32_MYSQL_DISPLAY5("User =", user, ", PW =", password, ", DB =", database);
}

void runInsert()
{
  ESP32_MySQL_Query query_mem = ESP32_MySQL_Query(&conn);
  if (conn.connected())
  {
    ESP32_MYSQL_DISPLAY(INSERT_SQL);    
    //ejecutando consulta
    if ( !query_mem.execute(INSERT_SQL.c_str()) )
    {
      ESP32_MYSQL_DISPLAY("Error de insercion");
    }
    else
    {
      ESP32_MYSQL_DISPLAY("Datos insertados");
    }
  }
  else
  {
    ESP32_MYSQL_DISPLAY("Desconectando del servidor. no se puede insertar");
  }
}

void loop()
{
  ESP32_MYSQL_DISPLAY("Conectando...");  
  if (conn.connectNonBlocking(server, server_port, user, password) != RESULT_FAIL)
  {
    delay(500);
    runInsert();
    conn.close(); // cierra conexion a base de datos
  } 
  else 
  {
    ESP32_MYSQL_DISPLAY("\nConexion fallida.");
  }

  delay(10000); //realizar inserción a cada 10 segundos 
}
