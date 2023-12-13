#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>


const char* ssid = "Redmi 12";
const char* password = "66664444";

// Установите адрес MQTT брокера
const char* mqtt_server = "broker.emqx.io";

const int mqtt_port = 8883;

// Инициализация клиента WiFi и MQTT
WiFiClientSecure espClient;
PubSubClient client(espClient);

// Пины для подключения ультразвукового дальномера
const int trigPin = D1;  // GPIO5
const int echoPin = D2;  // GPIO4

void setup_wifi() {
  delay(10);
  // Подключаемся к Wi-Fi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("WiFi connected - IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Переподключение, если соединение с MQTT было потеряно
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Попытка подключения с использованием имени пользователя и пароля
    if (client.connect("ESP8266Client", mqtt_username, mqtt_password)) {
      Serial.println("connected");
      // Подписка на топик
      // client.subscribe("your/topic/here"); // Если нужно подписаться на топик
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  espClient.setInsecure(); // Используйте этот метод, если сертификаты не требуются
  // espClient.setCACert(root_ca); // Раскомментируйте и укажите корневой сертификат для безопасного соединения
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback); // Укажите функцию обратного вызова, если она требуется

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long duration, distance;
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  // Отправка данных на MQTT-топик
  char msg[50];
  sprintf(msg, "Distance: %ld cm", distance);
  client.publish("esp/distance", msg);

  delay(1000); // Задержка между измерениями
}
