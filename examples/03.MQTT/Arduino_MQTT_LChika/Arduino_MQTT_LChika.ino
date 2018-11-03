/**
 * @file  Arduino_MQTT_LChika.ino
 * 
 * @brief MQTT経由でＬチカします
 *
 * @author IoT Algyan in Hiroshima. K.Nakamura.
 *
 * @date   2018.11.03
 */

/** 
 * 温度・湿度センサー : HTS221
 * ライブラリ         : STM32duino_HTS221
 * 
 * WIFIドライバー     : STM32duino_ISM43362-M3G-L44
 * MQTTクライアント   : PubSubClient
 */

#include <SPI.h>
#include <WiFiST.h>
#include <PubSubClient.h>
#include <HTS221Sensor.h>

// WiFi 設定 (SSID)
char* ssid              = "Camps-guest";
// WiFi 設定 (パスワード)
const char* password    = "9a7vnkoz4";

// MQTT サーバ (ＩＰアドレス)
#define AIO_SERVER      "192.168.100.103"
// MQTT サーバ (ポート番号)
#define AIO_SERVERPORT  1883
// MQTT サーバ (ユーザーＩＤ)
#define AIO_USERNAME    "user"
// MQTT サーバ (パスワード)
#define AIO_KEY         "pass"

// SPIバス接続ライブラリ(WiFiで使用)
SPIClass SPI_3(PC12, PC11, PC10);

// FiWiモジュールの定義
WiFiClass WiFi(&SPI_3, PE0, PE1, PE8, PB13);
WiFiClient STClient;

// FiWiステータス
int status = WL_IDLE_STATUS;

// HTS221センサーオブジェクト
HTS221Sensor  *HumTemp;

// I2Cバス接続ライブラリ
TwoWire *dev_i2c;
#define I2C2_SCL    PB10
#define I2C2_SDA    PB11

// MQTTクライアント
PubSubClient client(STClient);

long lastMsg = 0;
char msg[8];
char publishMsg[256];

/**
 * セットアップ
 */
void setup() {
  // LEDピン設定
  pinMode(LED_BUILTIN, OUTPUT);

  // シリアル通信初期化
  Serial.begin(115200);

  //  WiFi のセットアップ
  setup_wifi();

  // MQTTクライアント接続設定
  client.setServer(AIO_SERVER, AIO_SERVERPORT);
  // MQTT Subscibe メッセージ購読用コールバック関数設定
  client.setCallback(callback);

  // I2Cバス接続ライブラリの初期化
  dev_i2c = new TwoWire(I2C2_SDA, I2C2_SCL);
  dev_i2c->begin();

  // 温度・湿度センサーライブラリの初期化
  HumTemp = new HTS221Sensor (dev_i2c);
  HumTemp->Enable();
}

/**
 *  WiFi のセットアップ
 */
void setup_wifi() {

  delay(10);

  // WiFiステータスのチェック (WL_NO_SHIELD の場合は起動できない)
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi module not detected");
    // don't continue:
    while (true);
  }

  // WiFiファームウェアバージョンの取得とチェック
  String fv = WiFi.firmwareVersion();
  Serial.print("Firmware version: ");
  Serial.println(fv);

  if (fv != "C3.5.2.3.BETA9") {
    Serial.println("Please upgrade the firmware");
  }

  // WiFiが接続できるまでループする
  Serial.print("Attempting to connect to network: ");
  Serial.println(ssid);
  while (status != WL_CONNECTED) {
    Serial.print(".");
    // WiFiネットワークに接続
    status = WiFi.begin(ssid, password);
    // ２秒ウェイト
    delay(2000);
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("SSID : ");
  Serial.println(ssid);
  Serial.print("IP address : ");
  Serial.println(WiFi.localIP());
}

/**
 * MQTT Subscibe メッセージ購読用コールバック関数
 */
void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (length > 1) {
    // メッセージの内容(Payload) が "ON" の場合(2文字目が 'N')
    if ((char)payload[1] == 'N') {
     // LED 点灯
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
     // LED 消灯
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}

/**
 * MQTT接続(再接続)
 * MQTTは切断される場合があるので切断されている場合に再接続を行う処理が必須となる。
 */
void reconnect() {
  // MQTT接続ループ
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // MQTT接続処理
    if (client.connect("STM32Client", AIO_USERNAME, AIO_KEY)) {
      Serial.println("connected");
      // 接続時のメッセージ (以下は Topicが "user/feeds/hello", Payloadが "Hi, I'm STM32 user!"となる)
      client.publish(AIO_USERNAME"/feeds/hello", "Hi, I'm STM32 user!");
      // MQTTサーバへ購読メッセージ送信 (Topic名が "user/feeds/onoff" のメッセージを購読すい)
      client.subscribe(AIO_USERNAME"/feeds/onoff");
    } else {
      // 失敗した場合は  5 秒 スリープして再チャレンジ
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

/**
 * メインループ処理
 */
void loop() {
  float temperature, humidity;
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // 前回から 10000msec (10秒) 経過していれば、Publishメッセージを送信
  long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;

    // 温度・湿度センサーから温度を取得
    HumTemp->GetTemperature(&temperature);
    dtostrf(temperature, 2, 2, msg);
    Serial.print("Publish temperature ");
    Serial.println(msg);
    // publish メッセージを送信 (Topic名は "user/feeds/temp", Payloadは 温度)
    client.publish(AIO_USERNAME"/feeds/temp", msg);

    // 温度・湿度センサーから湿度を取得
    HumTemp->GetHumidity(&humidity);
    snprintf (msg, 8, "%u", (unsigned int)humidity);
    Serial.print("Publish humidity ");
    Serial.println(msg);
    // publish メッセージを送信 (Topic名は "user/feeds/hum", Payloadは 湿度)
    client.publish(AIO_USERNAME"/feeds/hum", msg);
  }
}
