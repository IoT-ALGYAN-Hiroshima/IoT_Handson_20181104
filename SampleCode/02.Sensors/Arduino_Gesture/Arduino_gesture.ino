/**
 * @file  Arduino_gesture.ino
 * 
 * @brief ジェスチャセンサーの値をモニタリングします
 *
 * @author IoT Algyan in Hiroshima. K.Nakamura.
 *
 * @date   2018.10.27
 */

/** 
 * ジェスチャセンサー : VL53L0X
 * ライブラリ         : STM32duino_VL53L0X
 */

// I2Cバス接続ライブラリ
#include <Wire.h>
// VL53L0Xライブラリ
#include <vl53l0x_class.h>

// STL ライブラリ
#include <iterator>
#include <list>

// I2Cバス接続
TwoWire WIRE1(PB11, PB10);  // SDA=PB11 & SCL=PB10
// ジェスチャセンサ
VL53L0X Vl53LOXSensor(&WIRE1, PC6, PC7); // XSHUT=PC6 & INT=PC7

// STL ネームスペース指定
using namespace std;

// 距離の最大値
uint32_t maxDistance = 0;
// 現在の距離
uint32_t currentDistance = 0;

// ジェスチャ用のコンテナ
list<int> gesture;

void setup() {
  int status;

  // Led.
  pinMode(LED_BUILTIN, OUTPUT);

  // シリアル出力初期化
  Serial.begin(115200);

  // I2Cバス初期化
  WIRE1.begin();

  // ジェスチャセンサOFF
  Vl53LOXSensor.VL53L0X_Off();

  // ジェスチャセンサ初期化
  status = Vl53LOXSensor.InitSensor(0x10);
  if(status)
  {
    Serial.println("Init Vl53LOXSensor failed...");
  }
}

void loop() {

  int status;
  // ジェスチャセンサの距離を測定
  status = Vl53LOXSensor.GetDistance(&currentDistance);

  if (status == VL53L0X_ERROR_NONE)
  {
    char report[64] = {0};
    snprintf(report, sizeof(report), "| Distance [mm]: %ld |", currentDistance);
    Serial.print(report);

    // ジェスチャ用コンテナに追加
    gesture.push_back(currentDistance);

    // ジェスチャ用コンテナの要素数が１６個以上になったら先頭を削除
    if (gesture.size() > 16) {
      gesture.pop_front();
    }

    // ジェスチャ用コンテナをシリアル出力
    list<int>::iterator ite;
    for (ite = gesture.begin(); ite != gesture.end(); ++ite) {
      Serial.print(*ite);
      Serial.print(", ");
    }
    Serial.println("");

    // 距離の最大値を更新
    maxDistance = max(maxDistance, currentDistance);
  }

  // 距離に合わせて点滅の速度を変更
  digitalWrite(LED_BUILTIN, HIGH);
  delay((int)((1000 * currentDistance) / maxDistance));
  digitalWrite(LED_BUILTIN, LOW);
}
