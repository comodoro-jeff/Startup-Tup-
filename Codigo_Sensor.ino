//Inclusão de bibliotecas do DHT
#include "DHT.h"

//Conectando ao pino digital 2 da placa
#define DHTPIN D2

//Definindo o tipo de sensor a ser usado
#define DHTTYPE DHT11

//Estabelece os dois parâmetros da função dht 
DHT dht(DHTPIN, DHTTYPE);

void setup(){
  Serial.begin(9600); // Configura a interface serial
  Serial.println("Sensor DHT11!");

  dht.begin(); // Inicializa o sensor
}

void loop(){
  //Aguarde 2 segundos entre as medições
  delay(2000);

  //Lê a umidade e armazena em u
  float u = dht.readHumidity();
  //Lê a temperatura em Celsius e armazena em t
  float t = dht.readTemperature();

  //Verifica se a leitura falhou ou saiu mais cedo
  if (isnan(u) || isnan(t)){
    Serial.println("Falha ao ler o sensor DHT11!");
    return;
  }
  
  Serial.print("Umidade: ");
  Serial.print(u);
  Serial.print("%");
  Serial.print(" \t");
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.println("°C");
}
