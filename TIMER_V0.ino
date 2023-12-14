#include "WiFi.h"
#include "TimeLib.h"
#include "LiquidCrystal_I2C.h"
//============
//LCD 設定
//============
LiquidCrystal_I2C lcd(0x27, 16, 2);
String message;
// ===========================
// Enter your WiFi credentials
// ===========================
const char* ssid = "alvin";
const char* password = "0981066860";
//==================
//時間設定
//==================
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 8 * 60 * 60;  //格林威治時間,一格3600,GMT+8就是8*3600=28800
const int daylightOffset_sec = 0;
struct tm timeinfo;
unsigned long debounce=millis();
//============
//腳位設定
//============
const int beep = 23;  //蜂鳴器
const int RLED = 14;
const int GLED = 12;
const int typebtn = 32;
const int startbtn = 33;
const int modebtn = 35;
const int hourbtn = 25;
const int minbtn = 26;
const int secbtn = 27;
//============
//電子鐘設定
//============
int clockstate = 0;//電子鐘模式：1:設定鬧鐘 2:設定倒數計時器
int clockhour;
int clockmin;
int clocksec;
int clockmode;
int clockstart = false;
int blackstart = false;
int blackhour;
int blackmin;
int blacksec;


void setup() {
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(typebtn),btnstate,FALLING);
  attachInterrupt(digitalPinToInterrupt(startbtn),btnstart,FALLING);
  //attachInterrupt(digitalPinToInterrupt(modebtn),btnmode,FALLING);
  attachInterrupt(digitalPinToInterrupt(hourbtn),btnhour,FALLING);
  attachInterrupt(digitalPinToInterrupt(minbtn),btnmin,FALLING);
  attachInterrupt(digitalPinToInterrupt(secbtn),btnsec,FALLING);
  pinMode(beep, OUTPUT);
  pinMode(RLED, OUTPUT);
  pinMode(GLED, OUTPUT);
  pinMode(typebtn, INPUT_PULLUP);
  pinMode(startbtn, INPUT_PULLUP);
  pinMode(modebtn, INPUT_PULLUP);
  pinMode(hourbtn, INPUT_PULLUP);
  pinMode(minbtn, INPUT_PULLUP);
  pinMode(secbtn, INPUT_PULLUP);


 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
  lcd.init();
  lcd.backlight();
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  
  lcd.clear();

  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  if(clockstate == 0 ){
  Serial.print("日期:");
  Serial.println(&timeinfo,"%Y-%m-%d  %a");
  Serial.print("時間:");
  Serial.println(&timeinfo, "%H:%M:%S");
  //LCD
  lcd.clear();
  lcd.print(&timeinfo,"%Y-%m-%d  %a.");
  lcd.setCursor(0,1);
  lcd.print(&timeinfo, "%H:%M:%S");
  delay(1000);
  }
  //電子鐘設置鬧鐘
  if(clockstate == 1){
  lcd.clear();
  lcd.print("Clock SET");
  lcd.setCursor(0,1);
  lcd.print(clockhour);
  lcd.print(":");
  lcd.print(clockmin);
  lcd.print("   ");
  lcd.print("pre");
  Serial.println("pre");
  Serial.print(clockhour);
  Serial.print(":");
  Serial.println(clockmin);
  delay(1000);
  }
  if(clockhour == hour() && clockmin == min())
            tone(beep, NOTE_B, 200);
            tone(beep, NOTE_B, 200);
            tone(beep, NOTE_B, 200);
            delay(1000);
  //電子鐘設置倒數計時器
  if(clockstate == 2 &&  blackstart == false){
    lcd.clear();
    lcd.print("Black Clock");
    lcd.setCursor(0,1);
    lcd.print(blackhour);
    lcd.print(":");
    lcd.print(blackmin);
    lcd.print(":");
    lcd.print(blacksec);
    lcd.setCursor(10,1);
    lcd.print(blackstart);
    Serial.println(blackmin);
    Serial.println(blacksec);
    delay(1000);
  }
  if(clockstate == 2 && blackstart == true){
  int sum = (blackhour*60)+(blackmin*60)+blacksec;
    while (sum >= 1) {
    lcd.clear();
    lcd.print("Black Clock");
    lcd.setCursor(0,1);
    lcd.print(blackhour);
    lcd.print(":");
    lcd.print(blackmin);
    lcd.print(":");
    lcd.print(blacksec);
    lcd.setCursor(10,1);
    lcd.print(blackstart);
    delay(1000);
    //blackhoure--;
    blacksec--;
    sum--;
    /*if(blackhour == -1 && blacksec != 0){
        blackhour -= 1;
        blackmin = 59;
    */
      if(blacksec == -1 && blackmin != 0){
        blackmin -= 1;
        blacksec = 59;
      }
      else if(blacksec == 0 && blackmin == 0){
        blackstart = !blackstart;
          while (clockstate == 2){
            tone(beep, NOTE_B, 200);
            tone(beep, NOTE_B, 200);
            tone(beep, NOTE_B, 200);
            delay(1000);
          }
      }
    }
  }
}

//電子鐘插斷
void btnstate() {
   if(millis() - debounce >= 300){
    clockstate ++;
    digitalWrite(beep,HIGH);
    delayMicroseconds(15000);
    digitalWrite(beep,LOW);
    if(clockstate > 2)
      clockstate =0;
    debounce = millis();
}}
void btnstart() {
  if(millis() - debounce >= 300){
    digitalWrite(beep,HIGH);
    delayMicroseconds(15000);
    digitalWrite(beep,LOW);
    if(clockstate == 1)
      clockstart = !clockstart;
    else if(clockstate == 2)
      blackstart = !blackstart;
    debounce = millis();
}}

//void btnmode() {}
// 小時設置
void btnhour() {
   if(millis() - debounce >= 300){
    digitalWrite(beep,HIGH);
    delayMicroseconds(15000);
    digitalWrite(beep,LOW);
    if(clockstate == 1)
      clockhour ++;
    else if(clockstate == 2)
      blackhour ++;
    debounce = millis();
}}
// 分設置
void btnmin() {
   if(millis() - debounce >= 300){
      digitalWrite(beep,HIGH);
      delayMicroseconds(15000);
      digitalWrite(beep,LOW);
    if(clockstate == 1)
      clockmin ++;
    else if(clockstate == 2)
      blackmin ++;
    debounce = millis();
}}
// 秒設置
void btnsec() {
   if(millis() - debounce >= 300){
      digitalWrite(beep,HIGH);
      delayMicroseconds(15000);
      digitalWrite(beep,LOW);
    if(clockstate == 1)
      clocksec ++;
    else if(clockstate == 2)
      blacksec ++;
    debounce = millis();
}}

