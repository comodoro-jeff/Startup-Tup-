// Inclusão de bibliotecas
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <TimeAlarms.h>
#include "DHT.h"

const char* ssid = "XXXXXXX"; // Nome da rede wifi ao qual se deseja conectar
const char* password = "XXXXXXXXXX";// Senha da rede wifi ao qual se deseja conectar
const char* mqtt_server = "demo.thingsboard.io"; // Endereço para ThingsBoard live demo
const char* device_token = "XXXXXXXXXXXXXXXXXX"; // Credenciais para o device que se deseja conectar

// Instancia o cliente wifi
WiFiClient wifi_client;

// Instancia o cliente MQTT
PubSubClient mqtt_client(wifi_client);

#define DHTPIN D2 //Conectando ao pino digital 2 da placa
#define DHTTYPE DHT11 //Definindo o tipo de sensor a ser usado
DHT dht(DHTPIN, DHTTYPE);//Estabelece os dois parâmetros da função dht

void conectar_wifi() {
  delay(10);

  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  // Tenta iniciar a conexao com a rede wifi
  WiFi.begin(ssid, password);
  
  // Verifica o status e aguarda a conexão
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Imprime na serial IP da conexão wifi 
  Serial.println("");
  Serial.println("Conectado ao AP");
  Serial.println("Endereco IP: ");
  Serial.println(WiFi.localIP());
}

void reconectar() {
  // Loop até que esteja reconectado com o broker MQTT 
  while (!mqtt_client.connected()) {

    // Verifica a conexão wifi
    if (WiFi.status() != WL_CONNECTED) {
      // Reconecta com o wifi
      conectar_wifi();
    }

    Serial.print("Tentando conexao com o ThingsBoard via MQTT...");

    // Tentativa de conexão
    if (mqtt_client.connect("Wemos", device_token, NULL)) {
      Serial.println("Conectado");
    } else {
      Serial.print("Falhou, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" Tentando novamente em 5s");
      // Tenta novamente daqui a 5 segundos
      delay(5000);
    }
  }
}

void coletar_e_enviar_dados() {
  
  Serial.print("Coletando dados de temperatura e umidade... ");
  
  //Lê a umidade e armazena em u
  float h = dht.readHumidity();
  //Lê a temperatura em Celsius e armazena em t
  float t = dht.readTemperature();

  //Verifica se a leitura falhou ou saiu mais cedo
  if (isnan(h) || isnan(t)){
    Serial.println("Falha ao ler o sensor DHT11!");
    return;
  }

  // Imprime na serial os valores lidos dos sensores //
  Serial.print("Umidade: ");
  Serial.print(h);
  Serial.print(" % | Temperatura: ");
  Serial.print(t);
  Serial.println(" *C ");

  // Converte os dados lidos para String 
  String temperatura = String(t);
  String umidade = String(h);

  // Ccria o payload da mensagem MQTT no formato JSON
  String payload = "{";
  payload += "\"temperatura\":"; payload += temperatura; payload += ",";
  payload += "\"umidade\":"; payload += umidade;
  payload += "}";

  // Converte os dados em formato JSON de String para char[] 
  char dados_json[100];
  payload.toCharArray(dados_json, 100);

  // Envia o pacote MQTT com os dados dos sensores 
  mqtt_client.publish( "v1/devices/me/telemetry", dados_json);
}

void setup() {
  // Configura a interface serial 
  Serial.begin(115200);
  // Inicializa o sensor
  dht.begin();
  // Conectar a rede wifi
  conectar_wifi();
  // Conectar ao broker MQTT 
  mqtt_client.setServer(mqtt_server, 1883);
  // Programa um evento de tempo a cada 10 segundos para enviar os dados dos sensores 
  Alarm.timerRepeat(10, coletar_e_enviar_dados);

}

void loop() {
  // Verifica status da conexão
  if (!mqtt_client.connected()) {
    reconectar();
  }
  // Executa a função de loop da biblioteca TimeAlarm 
  Alarm.delay(100);
  // Executa a funçãoo de loop do cliente MQTT 
  mqtt_client.loop();

}
