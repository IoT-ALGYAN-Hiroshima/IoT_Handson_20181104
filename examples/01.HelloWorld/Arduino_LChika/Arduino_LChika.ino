/**
 * @file  Arduino_LChika.ino
 * 
 * @brief LEDをちかちかさせます
 *
 * @author IoT Algyan in Hiroshima. K.Nakamura.
 *
 * @date   2018.09.09
 */

/**
 * Arduinoの初期化を行います
 */
void setup() {
  // put your setup code here, to run once:

  // LED2のピンを出力モードに設定
  pinMode(LED2, OUTPUT);  
}

/**
 * メインループ処理
 */
void loop() {
  // put your main code here, to run repeatedly:

  // LED2を点灯します
  digitalWrite(LED2, HIGH);

  // 1000msec (=1sec)待つ
  delay(1000);
  
  // LED2を消灯します
  digitalWrite(LED2, LOW);

  // 1000msec (=1sec)待つ
  delay(1000);
}
