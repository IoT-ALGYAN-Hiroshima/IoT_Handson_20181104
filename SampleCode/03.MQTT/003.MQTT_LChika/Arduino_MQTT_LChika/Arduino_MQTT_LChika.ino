/**
 * @file  Arduino_MQTT_HelloWorld.ino
 * 
 * @brief MQTTで Hello World !
 *
 * @author IoT Algyan in Hiroshima. K.Nakamura.
 *
 * @date   2018.09.09
 */
 
#define MQTT_VERSION MQTT_VERSION_3_1_1

#include <WiFiST.h>
#include <PubSubClient.h>

// SSID、WPA Key および mqtt_server を指定してください
//char* ssid              = "SPWN_H36_66D7AE";
//const char* wpaKey      = "tgfd3ad4jagb931";
//const char* mqtt_server = "192.168.100.103";
char* ssid              = "kei.nak5";
const char* wpaKey      = "kei.nak5";
const char* mqtt_server = "192.168.137.1";
uint16_t mqtt_port      = 1883;

unsigned long counter = 0;

// Timers auxiliar variables
long now = millis();
long now_a = now;
long now_b = now;
long lastMeasure = 0;

const int lamp = LED2;

// WiFIモジュール接続で SPI通信を行います
//SPIClass SPI_3(PC12, PC11, PC10);
//WiFiClass WiFi(&SPI_3, PE0, PE1, PE8, PB13);

// WiFiモジュールと MQTTプロトコル用の PubSubClient を用意します
WiFiClient stm32Client;
PubSubClient client(stm32Client);

/**
 * カウンタ経過チェック
 */
boolean isElapsed(unsigned long* counter, unsigned long timeSpan) {
  
  unsigned long now = millis();
  if (now > (*counter + timeSpan)) {
    *counter = now;
    return true;
  }
  
  return false;
}

/**
 * WiFi のセットアップを行います 
 */
void setupWifi() {

  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.print(" ... ");

  // WiFi network へ接続を開始します
  WiFi.begin(ssid, wpaKey);

  // WiFi が接続状態となるまで待ちます
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // 乱数の種を初期化します  
  randomSeed(micros());

  Serial.print("connected to ");
  Serial.print(WiFi.localIP());
  Serial.print(" (");
  Serial.print(WiFi.firmwareVersion());
  Serial.println(" )");
}

/**
 * MQTT の再接続を行います
 */
void reconnect() {

  if (!client.connected()) {

    // クライアントID文字列を作成します
    String clientId = "stm32_";
    clientId += String(random(0xffff), HEX);
    Serial.print("Attempting MQTT connection " + clientId + " ... ");

    // MQTT接続を待ちます
    if (client.connect("STM32Client" /*clientId.c_str()*/)) {
      Serial.println("connected.");
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
    }
  }
}


/**
 * 
 */
void callback(char* topic, uint8_t* message, uint32_t length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  String topicTemp = topic;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic room/lamp, you check if the message is either on or off. Turns the lamp GPIO according to the message
  if(topicTemp == "room/lamp"){
      Serial.print("Changing Room lamp to ");
      if(messageTemp == "on"){
        digitalWrite(lamp, HIGH);
        Serial.print("On");
      }
      else if(messageTemp == "off"){
        digitalWrite(lamp, LOW);
        Serial.print("Off");
      }
  }
  Serial.println();
}

/**
 * Arduino のセットアップをおこないます
 */
void setup() {

  // LEDのピンの指定します (ここでは OUTPUT = 出力モード)
  pinMode(LED2, OUTPUT);

  // USBシリアル通信の開始
  Serial.begin(115200);

  // MQTTクライアントにMQTTサーバーを指定
  client.setServer(mqtt_server, mqtt_port);

  // Subscribe メッセージ受信用のコールバック
  client.setCallback(callback);

  // WiFi を設定
  setupWifi();
}

/**
 * メインループ処理
 */
void loop() {
  char buff[128] = {0};
  
  if (!client.connected()) {
    reconnect();
  }

  now_a = millis();
  uint32_t now = millis();
  if(!client.loop()) {
    reconnect();
  }
  now_b = millis();
//  sprintf(buff, "loop : %d msec", now_b - now_a);
//  Serial.println(buff);

  now = millis();
  // Publishes new temperature and humidity every 30 seconds
  if (now - lastMeasure > 10000) {
    lastMeasure = now;

    // Publishes Temperature and Humidity values
    client.publish("room/temperature", "32");
    client.publish("room/humidity", "90");
    Serial.println("publish message.");
  }
}

