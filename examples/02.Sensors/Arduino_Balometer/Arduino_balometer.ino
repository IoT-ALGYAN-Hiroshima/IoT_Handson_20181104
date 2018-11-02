/**
 * @file  Arduino_balometer.ino
 * 
 * @brief 気圧計センサーの値をモニタリングします
 *
 * @author IoT Algyan in Hiroshima. K.Nakamura.
 *
 * @date   2018.10.27
 */

/** 
 * 気圧計センサー : LPS22HB
 * ライブラリ     : STM32duino_LPS22HB
 */

// LPS22HBセンサーライブラリをインクルード
#include <LPS22HBSensor.h>

// LPS22HBセンサーオブジェクト
LPS22HBSensor *PressTemp;

// I2Cバス接続オブジェクト
TwoWire *dev_i2c;
#define I2C2_SCL    PB10
#define I2C2_SDA    PB11

// loop中で使用する変数
long lastMsg = 0;
char temp[6] = {0};
char publishMsg[256] = {0};

/**
 * Arduinoの初期化を行います
 */
void setup() {
  // put your setup code here, to run once:

  // シリアル出力の開始
  Serial.begin(115200);

  // I2Cバス接続オブジェクトのインスタンス生成
  dev_i2c = new TwoWire(I2C2_SDA, I2C2_SCL);
  // I2Cバス接続開始
  dev_i2c->begin();

  // LPS22HBセンサーオブジェクトのインスタンス生成
  // (LPS22HB Sensorでは I2C を使用します)
  PressTemp = new LPS22HBSensor(dev_i2c);
  PressTemp->Enable();
}

/**
 * メインループ処理
 */
void loop() {
  // put your main code here, to run repeatedly:

  float pressure, temperature = 0.0;

  // 前回の処理から 1000msec 経過した場合に処理を行う
  long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;

    // 温度の取得
    PressTemp->GetTemperature(&temperature);
  
    // 気圧の取得
    PressTemp->GetPressure(&pressure);

    // 温度(float)を、文字列に変換(文字列長６文字、小数部２桁)
    dtostrf(temperature, 6, 2, temp);

    // 出力文字列を作成 (C言語のような複雑な書式指定はできないので注意)
    snprintf(publishMsg, 256, "{\"temperature\":%s, \"pressure\":%u}", temp, (unsigned int)pressure);

    // シリアルで出力
    Serial.print("publish : ");
    Serial.println(publishMsg);
  }
}
