/**
 * @file  Arduino_MQTT_Connection.ino
 * 
 * @brief MQTTサーバーへの接続確認を行います
 *
 * @author IoT Algyan in Hiroshima. K.Nakamura.
 *
 * @date   2018.09.09
 */

#include <WiFiST.h>
#include <PubSubClient.h>

// SSID、WPA Key および mqtt_server を指定してください
char* ssid              = "SPWN_H36_66D7AE";
const char* wpaKey      = "tgfd3ad4jagb931";
const char* mqtt_server = "192.168.100.103";
uint16_t mqtt_port      = 1883;

// WiFIモジュール接続で SPI通信を行います
SPIClass SPI_3(PC12, PC11, PC10);
WiFiClass WiFi(&SPI_3, PE0, PE1, PE8, PB13);

// WiFiモジュールと MQTTプロトコル用の PubSubClient を用意します
WiFiClient stm32Client;
PubSubClient client(stm32Client);

/**
 * カウンタ経過チェック
 */
bool isElapsed(unsigned long* counter, unsigned long timeSpan) {
  
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
  Serial.println(".");

  // OTA firmware アップデート
//  Serial.println("Firmware update start.");
//  char buff[256]; 
//  uint8_t result = WiFi.FirmwareUpdate();
//  sprintf(buff, "Firmware update finished. : %d", result);
//  Serial.println(buff);
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
    if (client.connect(clientId.c_str())) {
      Serial.println("connected.");
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
    }
  }
}

/**
 * Arduino のセットアップをおこないます
 */
void setup() {

  // USBシリアル通信の開始
  Serial.begin(115200);

  // WiFi を設定
  setupWifi();

  // MQTTクライアントにMQTTサーバーを指定
  client.setServer(mqtt_server, mqtt_port);
}

/**
 * メインループ処理
 */
void loop() {

  unsigned long counter = 0;

  if (isElapsed(&counter, 5000)) {
    Serial.println("*");

    // MQTTと接続していない場合に再接続します
    if (!client.connected()) {
      reconnect();
    }
  }
  
  // MQTTクライアントのループ処理
  client.loop();
}


