#include "FS.h"
#include "SD.h"
String dataMessage="Vacio";
String identificador="LC-1";
void setup(){
      Serial.begin(9600);
      initSDCard(); 
    File file = SD.open("/data_LC.csv");
    if(!file) {
              Serial.println("Archivo no existe");
              Serial.println("Creating file...");
              writeFile(SD, "/data_LC.csv", "Identificador \r\n");
    }
    else {
                Serial.println("Archivo ya existe");  
    }
    file.close();
}

void loop(){
                              dataMessage = identificador+"\r\n";
                              Serial.print("Guardando: ");
                              Serial.println(dataMessage);
                              appendFile(SD, "/data_LC.csv", dataMessage.c_str());
                             delay(1000);
}


// Initialize SD card
void initSDCard(){
   if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
}
// Write to the SD card
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}
// Append data to the SD card
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}