#include "esp_camera.h"
#include <WiFi.h>
#include "ESP32Servo.h"  //SG90
#include "Freenove_WS2812_Lib_for_ESP32.h"
#include "DHT.h"
#include "TimeLib.h"
#include <TridentTD_LineNotify.h>
#define LINE_TOKEN "SzGAbbGYBF9HkbOt4JAjKAs3NItoJF6WCqjtKtFqjaN"
//LED RGB 設定
#define LEDS_COUNT 8
#define LEDS_PIN 37
#define CHANNEL 0
//溫溼度設定
#define DHTPIN 36
#define DHTTYPE DHT11

//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//
//            You must select partition scheme from the board menu that has at least 3MB APP space.
//            Face Recognition is DISABLED for ESP32 and ESP32-S2, because it takes up from 15
//            seconds to process single frame. Face Detection is ENABLED if PSRAM is enabled as well

// ===================
// Select camera model
// ===================
//#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
//#define CAMERA_MODEL_ESP_EYE // Has PSRAM
#define CAMERA_MODEL_ESP32S3_EYE  // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
//#define CAMERA_MODEL_M5STACK_UNITCAM // No PSRAM
//#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM
//#define CAMERA_MODEL_XIAO_ESP32S3 // Has PSRAM
// ** Espressif Internal Boards **
//#define CAMERA_MODEL_ESP32_CAM_BOARD
//#define CAMERA_MODEL_ESP32S2_CAM_BOARD
//#define CAMERA_MODEL_ESP32S3_CAM_LCD
//#define CAMERA_MODEL_DFRobot_FireBeetle2_ESP32S3 // Has PSRAM
//#define CAMERA_MODEL_DFRobot_Romeo_ESP32S3 // Has PSRAM
#include "camera_pins.h"


// ===========================
// Enter your WiFi credentials
// ===========================
const char* ssid = "alvin";
const char* password = "0981066860";
//==================
//時間設定
//==================
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 28800;  //格林威治時間,一格3600,GMT+8就是8*3600=28800
const int daylightOffset_sec = 3600;
//============
//腳位設定
//============
int beep = 35;        //蜂鳴器
int mq5 = 19;         //瓦斯偵測
int isFlamePin = 20;  //火焰偵測
//int servopin = 12;//伺服馬達
int relayPin = 41;
Servo myservo;
int Temp_Pin = 4;
int pos = 0;
int isFlame = HIGH;
float Temp_Val;
float Temp_Dat;
float GasValue;  //宣告變數GasValue
DHT dht(DHTPIN, DHTTYPE);
Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);

void startCameraServer();
void setupLedFlash(int pin);

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  //Mq5氣體感測設定
  pinMode(mq5, INPUT);
  //蜂鳴器設定
  pinMode(beep, OUTPUT);
  //火焰偵測設定
  pinMode(isFlamePin, INPUT);
  //伺服馬達設定
  pinMode(relayPin, OUTPUT);
  //myservo.attach(servopin);
  //ESP32S3 48port RBG_LED設定
  strip.begin();
  //時間設定
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);


  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;  // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t* s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);        // flip it back
    s->set_brightness(s, 1);   // up the brightness just a bit
    s->set_saturation(s, -2);  // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, FRAMESIZE_QVGA);
  }

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif

// Setup LED FLash if LED pin is defined in camera_pins.h
#if defined(LED_GPIO_NUM)
  setupLedFlash(LED_GPIO_NUM);
#endif

  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void loop() {
  int GasValue = analogRead(mq5);  //讀取的mq5讀取的數值放在GasValue
  int temp_DH11 = dht.readTemperature();
  int sumbreaktime = 0;
  int sumrelaytime = 0;
  isFlame = digitalRead(isFlamePin);

  //無火有瓦斯
  if (isFlame == HIGH && GasValue > 1100) {
    Serial.println("瓦斯外洩");  //訊號輸出
    digitalWrite(beep, HIGH);
    //myservo.write(0);   // 告訴 servo 走到 'pos' 的位置
    //delay(500);   // 等待 15ms 讓 servo 走到指定位置
    strip.setLedColorData(0, 0xFF0000);
    strip.show();
    LINE.setToken(LINE_TOKEN);
    LINE.notify("瓦斯外洩");
    sumrelaytime++;
    delay(5000);
  }
  //有火有瓦斯
  else if (isFlame == LOW && GasValue > 1100) {
    strip.setLedColorData(0, 0xFF0000);
    strip.show();
    delay(10000);
    if (temp_DH11 > 27) {
      Serial.println("溫度過高");
      LINE.setToken(LINE_TOKEN);
      LINE.notify("持續溫度過高，請注意....");
      sumbreaktime += 1;
      //倒數計時啟動
      //
    } else {
      Serial.println("監控中...");  //訊號輸出
    }
  }
  //有火無瓦斯
  else if (isFlame == LOW && GasValue < 1100) {
    Serial.println("已點火，請注意...");  //訊號輸出
    strip.setLedColorData(0, 0xFF0000);
    strip.show();
    delay(10000);
  }
  //無火無瓦斯
  else {
    Serial.println("待機中....");
    digitalWrite(beep, LOW);
    strip.setLedColorData(0, 0x008000);
    strip.show();
    delay(10000);
  }

  if (sumrelaytime > 2) {
    LINE.setToken(LINE_TOKEN);
    LINE.notify("瓦斯自動關閉");
    digitalWrite(relayPin, HIGH);
    delay(10000);
    digitalWrite(relayPin, LOW);
    delay(100);
    sumrelaytime = 0;
  }


  Serial.println();
  printLocalTime();
  Serial.println();
  Serial.print("瓦斯濃度:");
  Serial.print(abs(GasValue - 550));  //GasValue顯示在序列視窗
  Serial.println("ppm");
  Serial.println(GasValue);
  Serial.print("溫度:");
  Serial.print(temp_DH11);
  Serial.println("°C");
  Serial.println();
  Serial.println(sumrelaytime);
  /*
  Temp_Val = analogRead(Temp_Pin);//將val設置為讀取到的數值
  Temp_Dat = (Temp_Val/80);//計算當前溫度數字dat
  Serial.println();
  Serial.println(Temp_Dat);  //顯示計算的温度值
  Serial.println("C");//顯示C，並自動換行
  */

  /*
    myservo.write(135);    // 告訴 servo 走到 'pos' 的位置
    delay(1000);        // 等待 15ms 讓 servo 走到指定位置
    myservo.write(0);   // 告訴 servo 走到 'pos' 的位置
    delay(1000);   // 等待 15ms 讓 servo 走到指定位置
*/
}


void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
