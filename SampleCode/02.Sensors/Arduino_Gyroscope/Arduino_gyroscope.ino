/**
 * @file  Arduino_gesture.ino
 * 
 * @brief ３Ｄジャイロセンサ・加速度センサーの値をモニタリングします
 *
 * @author IoT Algyan in Hiroshima. K.Nakamura.
 *
 * @date   2018.10.27
 */

/** 
 * ３Ｄジャイロセンサ・加速度センサー : LSM6DSL
 * ライブラリ         : STMduino_LSM6DSL
 */

// ３Ｄジャイロセンサ・加速度センサーライブラリ
#include <LSM6DSLSensor.h>

#define I2C2_SCL    PB10
#define I2C2_SDA    PB11

// 割り込みピン
#define INT1        PD11

// 割り込みイベントフラグ
volatile int mems_event = 0;
// ステップカウント
uint16_t step_count = 0;
// シリアル出力バッファ
char report[256];

// 割り込みハンドラ
void INT1Event_cb();
// 方位の判定
void sendOrientation();

// I2Cバス接続ライブラリ
TwoWire *dev_i2c;
// ３Ｄジャイロセンサ・加速度センサー
LSM6DSLSensor *AccGyr;

void setup() {

  // シリアル出力初期化
  Serial.begin(115200);

  // I2Cバス初期化
  dev_i2c = new TwoWire(I2C2_SDA, I2C2_SCL);
  dev_i2c->begin();

  // INT1ピンの立ち上がり時に割り込みハンドラをアタッチ
  attachInterrupt(INT1, INT1Event_cb, RISING);

  // ３Ｄジャイロセンサ・加速度センサーライブラリ初期化
  AccGyr = new LSM6DSLSensor(dev_i2c, LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW);
  AccGyr->Enable_X();

  // 万歩計モーション判定ＯＮ
  AccGyr->Enable_Pedometer();
  // 回転モーション判定ＯＮ
  AccGyr->Enable_Tilt_Detection();
  // 自由落下モーション判定ＯＮ
  AccGyr->Enable_Free_Fall_Detection();
  // シングルタップモーション判定ＯＮ
  AccGyr->Enable_Single_Tap_Detection();
  // ダブルタップモーション判定ＯＮ
  AccGyr->Enable_Double_Tap_Detection();
  // ６軸オリエンテーションモーション判定ＯＮ
  AccGyr->Enable_6D_Orientation();
}

void loop() {
  if (mems_event)
  {
    mems_event = 0;
    LSM6DSL_Event_Status_t status;
    AccGyr->Get_Event_Status(&status);

    if (status.StepStatus)
    {
      // New step detected, so print the step counter
      AccGyr->Get_Step_Counter(&step_count);
      snprintf(report, sizeof(report), "Step counter: %d", step_count);
      Serial.println(report);
    }

    if (status.FreeFallStatus)
    {
      // Output data.
      Serial.println("Free Fall Detected!");
    }

    if (status.TapStatus)
    {
      // Output data.
      Serial.println("Single Tap Detected!");
    }

    if (status.DoubleTapStatus)
    {
      // Output data.
      Serial.println("Double Tap Detected!");
    }

    if (status.TiltStatus)
    {
      // Output data.
      Serial.println("Tilt Detected!");
    }

    if (status.D6DOrientationStatus)
    {
      // Send 6D Orientation
	    sendOrientation();
    }
  }
}

void INT1Event_cb()
{
  mems_event = 1;
}

void sendOrientation()
{
  uint8_t xl = 0;
  uint8_t xh = 0;
  uint8_t yl = 0;
  uint8_t yh = 0;
  uint8_t zl = 0;
  uint8_t zh = 0;

  AccGyr->Get_6D_Orientation_XL(&xl);
  AccGyr->Get_6D_Orientation_XH(&xh);
  AccGyr->Get_6D_Orientation_YL(&yl);
  AccGyr->Get_6D_Orientation_YH(&yh);
  AccGyr->Get_6D_Orientation_ZL(&zl);
  AccGyr->Get_6D_Orientation_ZH(&zh);

  if ( xl == 0 && yl == 0 && zl == 0 && xh == 0 && yh == 1 && zh == 0 )
  {
    sprintf( report, "\r\n  ________________  " \
                      "\r\n |                | " \
                      "\r\n |  *             | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |________________| \r\n" );
  }

  else if ( xl == 1 && yl == 0 && zl == 0 && xh == 0 && yh == 0 && zh == 0 )
  {
    sprintf( report, "\r\n  ________________  " \
                      "\r\n |                | " \
                      "\r\n |             *  | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |________________| \r\n" );
  }

  else if ( xl == 0 && yl == 0 && zl == 0 && xh == 1 && yh == 0 && zh == 0 )
  {
    sprintf( report, "\r\n  ________________  " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |  *             | " \
                      "\r\n |________________| \r\n" );
  }

  else if ( xl == 0 && yl == 1 && zl == 0 && xh == 0 && yh == 0 && zh == 0 )
  {
    sprintf( report, "\r\n  ________________  " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |                | " \
                      "\r\n |             *  | " \
                      "\r\n |________________| \r\n" );
  }

  else if ( xl == 0 && yl == 0 && zl == 0 && xh == 0 && yh == 0 && zh == 1 )
  {
    sprintf( report, "\r\n  __*_____________  " \
                      "\r\n |________________| \r\n" );
  }

  else if ( xl == 0 && yl == 0 && zl == 1 && xh == 0 && yh == 0 && zh == 0 )
  {
    sprintf( report, "\r\n  ________________  " \
                      "\r\n |________________| " \
                      "\r\n    *               \r\n" );
  }

  else
  {
    sprintf( report, "None of the 6D orientation axes is set in LSM6DSL - accelerometer.\r\n" );
  }

  Serial.print(report);
}
