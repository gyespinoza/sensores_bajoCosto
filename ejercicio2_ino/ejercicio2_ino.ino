#include <BH1750.h>
#include <Wire.h>

const int ledPin = 4;

BH1750 lightMeter;//Revisar conexiones SDA y SCL 

float valorMinimo = 5000;
float valorMaximo = 0;
float umbralBajo = 0;
float umbralAlto = 0;

//valores de brillo
const int brilloBajo = 50;
const int brilloMedio = 150;
const int brilloAlto = 255;

void setup() {
  pinMode(ledPin, OUTPUT);

  Serial.begin(9600);
  Wire.begin();
  lightMeter.begin();
  Serial.println(F("BH1750 Test begin"));
}

void loop() {
  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");


  if (lux < valorMinimo && lux > 0){
    valorMinimo = lux;
  }
  if(lux > valorMaximo){
    valorMaximo = lux;
  }
  
  //calculo de niveles
  float rango = valorMaximo - valorMinimo;
  umbralBajo = valorMinimo + (rango / 3);
  umbralAlto = valorMaximo - (rango / 3);

  int brilloLED;

  if(lux < umbralBajo){
    brilloLED = brilloBajo;
  }
  else if (lux > umbralAlto) {
    brilloLED = brilloAlto;
  }
  else {
    brilloLED = brilloMedio;
  }
  analogWrite(ledPin, brilloLED); //establecer el brillo del LED



  delay(500);
}
