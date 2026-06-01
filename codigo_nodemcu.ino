#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// ==========================================
// CONFIGURAÇÕES DE REDE E BROKER MQTT
// ==========================================
const char* ssid     = "SUA_REDE_WIFI"; // Ajuste seu Wi-Fi
const char* password = "SUA_SENHA_WIFI";     // Ajuste sua Senha

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port     = 1883;

// Tópicos MQTT do seu projeto simplificado
const char* TOPICO_CHUVA  = "mackenzie/gideias/chuva";
const char* TOPICO_LED    = "mackenzie/gideias/led"; 

// ==========================================
// DEFINIÇÃO DOS PINOS DO HARDWARE
// ==========================================
#define PIN_CHUVA D5       // Sensor de Chuva no D5
#define PIN_LED D6         // LED Vermelho (Atuador) no D6

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long tempoAnterior = 0;
const long intervalo = 4000; // Envia a cada 4 segundos

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no [");
  Serial.print(topic);
  Serial.print("]: ");
  
  String mensagem = "";
  for (unsigned int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }
  Serial.println(mensagem);

  if (String(topic) == TOPICO_LED) {
    if (mensagem == "1") {
      digitalWrite(PIN_LED, HIGH); // Liga o LED
      Serial.println(">>> LED LIGADO VIA MQTT");
    } else if (mensagem == "0") {
      digitalWrite(PIN_LED, LOW);  // Desliga o LED
      Serial.println(">>> LED DESLIGADO VIA MQTT");
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    String clientId = "ESP8266Client-Gideias-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado ao Broker!");
      client.subscribe(TOPICO_LED);
    } else {
      Serial.print("Falha. Erro=");
      Serial.print(client.state());
      Serial.println(" Retentando em 4s...");
      delay(4000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_CHUVA, INPUT);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW); 

  Serial.println();
  Serial.print("Conectando em: ");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Conectado!");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long tempoAtual = millis();
  if (tempoAtual - tempoAnterior >= intervalo) {
    tempoAnterior = tempoAtual;

    // Leitura direta do Sensor de Chuva (LOW = Chuva / HIGH = Sem Chuva)
    int leitura = digitalRead(PIN_CHUVA);
    int statusChuva = (leitura == LOW) ? 1 : 0; 

    Serial.print("Status do Sensor de Chuva: ");
    Serial.println(statusChuva ? "CHUVOSO (1)" : "SECO (0)");

    // Publica no MQTT
    client.publish(TOPICO_CHUVA, String(statusChuva).c_str());
    Serial.println("Dado de chuva enviado para o Broker!");
  }
}
