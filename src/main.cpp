#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h> // Biblioteca para comunicação MQTT

const char* ssid = ""; // Nome do seu Wifi
const char* password = ""; // Senha do wifi
const char* name = "Pedro"; 
const char* mqttServer = "agrotechlab.lages.ifsc.edu.br"; // URL do servidor MQTT
const int mqttPort = 1883; // Porta MQTT
const char* clientName = "qhbg2n126yb6me65ccd7"; // Nome do cliente MQTT
const char* clientId = "ESP8266"; // ID do cliente MQTT
JsonDocument json; // Objeto JSON para manipular os dados


WiFiClient espClient; // Objeto para comunicação WiFi
PubSubClient client(espClient); // Objeto para comunicação MQTT

void setup() {
  Serial.begin(9600); // Inicialização da comunicação serial com a velocidade de 9600 bps
  pinMode(LED_BUILTIN, OUTPUT);
  delay(10);
  Serial.println('\n');

  WiFi.begin(ssid, password); // Conexão à rede WiFi
  Serial.print("Connecting to");
  Serial.println(ssid); Serial.println("...");

  int i = 0;
  while (WiFi.status()!= WL_CONNECTED) { // Aguarda até que a conexão WiFi seja estabelecida
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print(++i);Serial.print(' ');
     // Liga o LED embutido no ESP8266
  }

  Serial.print("\n");
  Serial.print("Connection estabilished"); // Mensagem indicando que a conexão foi estabelecida
  Serial.print("IP Adress :\t");
  Serial.print(WiFi.localIP()); // Mostra o endereço IP local do dispositivo

  client.setServer(mqttServer, mqttPort); // Configura o servidor MQTT

  while (!client.connected()) { // Loop até que a conexão MQTT seja estabelecida
    Serial.println("Conectando ao servidor MQTT...");
    if (client.connect(clientId, clientName, NULL)) { // Tenta conectar ao servidor MQTT
      Serial.println("Conectado ao servidor MQTT!");
      digitalWrite(LED_BUILTIN, HIGH); // Desliga o LED embutido
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos"); // Se a conexão falhar, espera 5 segundos antes de tentar novamente
      delay(5000);
    }
  }
}

// Controle de tempo
unsigned long tempoTemp = 0; // Obtém o tempo atual em milissegundos
unsigned long tempoUm = 0; 
unsigned long tempoVent = 0; 
unsigned long tempoLed = 0; 
bool ledState = HIGH; 

void loop() {
  unsigned long tempo = millis();
   
   if (tempo - tempoLed >= 1000) { 
    tempoLed = tempo; 
    ledState = !ledState; 
    digitalWrite(LED_BUILTIN, ledState); 
   }

  if (client.connected()) {
    if(tempo - tempoTemp >= 5000){ // Verifica se passaram 5 segundos desde a última leitura de temperatura
      Serial.print("entrou");
      tempoTemp = tempo; // Atualiza o tempo da última leitura de temperatura
      float temperatura = random(500, 1599) / 100.0; // Gera um valor de temperatura aleatório entre 5.00 e 15.99
      json.clear();
      json["temp"] = temperatura; // Adiciona a temperatura ao objeto JSON
      char enviar[256];
      serializeJson(json, enviar); // Serializa o objeto JSON para uma string
      client.publish("v1/devices/me/telemetry", enviar); // Publica os dados de temperatura no tópico MQTT "v1/devices/me/telemetry"
    }
      
    if(tempo - tempoUm >= 10000){ 
       tempoUm = tempo; 
      float umidade = random(4000, 8999) / 100.0; 
      json.clear();
      json["umid"] = umidade; 
      char enviar[256];
      serializeJson(json, enviar); 
      client.publish("v1/devices/me/telemetry", enviar); 
    }

    if(tempo - tempoVent >= 15000){ 
      tempoVent = tempo;
      float vento = random(0, 1999) / 100.0; 
      json.clear();
      json["vel_vento"] = vento;
      char enviar[256];
      serializeJson(json, enviar); 
      client.publish("v1/devices/me/telemetry", enviar); 
    }
  }else{
      while (!client.connected()) { // Loop até que a conexão MQTT seja estabelecida
    Serial.println("Conectando ao servidor MQTT...");
    if (client.connect(clientId, clientName, NULL)) { // Tenta conectar ao servidor MQTT
      Serial.println("Conectado ao servidor MQTT!");
      digitalWrite(LED_BUILTIN, HIGH); // Desliga o LED embutido
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos"); // Se a conexão falhar, espera 5 segundos antes de tentar novamente
      delay(5000);
    }
  }
  }
}
