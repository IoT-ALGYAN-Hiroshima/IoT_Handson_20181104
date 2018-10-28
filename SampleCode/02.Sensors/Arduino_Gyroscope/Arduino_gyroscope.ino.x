/**
 * @file  Arduino_gesture.ino
 * 
 * @brief ３軸地磁気センサーの値をモニタリングします
 *
 * @author IoT Algyan in Hiroshima. K.Nakamura.
 *
 * @date   2018.10.27
 */

/** 
 * ３軸地磁気センサー : LIS3MDL
 * ライブラリ         : STM32duino_LIS3MDL
 */


// ３軸地磁気センサーライブラリ
#include <LIS3MDLSensor.h>
// Ｍａｔｈヘッダ
#include <math.h>

#define I2C2_SCL    PB10
#define I2C2_SDA    PB11

// I2Cバス接続
TwoWire *dev_i2c;

// ３軸地磁気センサー
LIS3MDLSensor *Magneto;

void setup() {
  // Led.
  pinMode(LED_BUILTIN, OUTPUT);

  // シリアル出力初期化
  Serial.begin(115200);

  // I2Cバス初期化
  dev_i2c = new TwoWire(I2C2_SDA, I2C2_SCL);
  dev_i2c->begin();

  // ３軸地磁気センサー初期化
  Magneto = new LIS3MDLSensor(dev_i2c);
  Magneto->Enable();
}

void loop() {
  // Led blinking.
  digitalWrite(LED_BUILTIN, HIGH);
  delay(250);
  digitalWrite(LED_BUILTIN, LOW);
  delay(250);

  // ３軸地磁気センサデータ用バッファ [0] -> X, [1] -> Y, [2] -> Z に対応
  int32_t magnetometer[3] = {0};
  // ３軸地磁気センサのセンサデータの読み出し
  Magneto->GetAxes(magnetometer);

  Serial.print("Mag[mGauss]: X = ");
  Serial.print(magnetometer[0]);
  Serial.print(", Y = ");
  Serial.print(magnetometer[1]);
  Serial.print(", Z = ");
  Serial.println(magnetometer[2]);
}
