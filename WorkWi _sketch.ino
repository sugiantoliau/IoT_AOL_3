#define BLYNK_TEMPLATE_ID "TMPL6ClFgS-2L"
#define BLYNK_TEMPLATE_NAME "ESP32 IOT Project Study"
#define BLYNK_AUTH_TOKEN "JC6k0TRdHYwcTjCiOKKMBStae2eF3KmO"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <NewPing.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h> 

// --- Konfigurasi Authentication by Serial Monitor ---
String inputCode = ""; 
String correctCode = "1234"; 
bool isLoggedIn = false;

// --- Konfigurasi WiFi ---
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// --- Definisi Pin Hardware ---
#define LED_BUILTIN_PIN 2 
#define PIR_PIN 13       
#define RELAY_PIN 27     
#define TRIG_PIN 14      
#define ECHO_PIN 12      
#define MAX_DISTANCE 400 
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);
BlynkTimer timer;
LiquidCrystal_I2C lcdBig(0x27, 20, 4);  

// --- Deklarasi Variabel ---
bool systemStarted = false;
int counterOrang = 0;
unsigned long lastMotionTime = 0;
bool motionDetected = false;
bool lastSonarState = false; 
unsigned int maxDistanceRadar = 90;
String pirMsg = "idle";
String sonarMsg = "idle";
String visitorStatus = "Safe";
const unsigned long TIMEOUT_LAMP_OFF = 1000; // 1 detik
unsigned long pirTriggerTime = 0;


void setup() {
  Serial.begin(115200);
  Serial.println("Masukkan Kode untuk mengaktifkan sistem:");  
  fn_lcdBig_Initialization(); 
  fn_PinHardware_Initialization();
  fn_turnOffLamp();  
}

void loop() {
  if (!isLoggedIn && Serial.available()) {
    inputCode = Serial.readStringUntil('\n');
    inputCode.trim();
    if (inputCode == correctCode) {
      isLoggedIn = true;
      fn_RunonLoginSuccess();
      fn_startSystem(); 
    } else {
      Serial.println("Kode salah. Coba lagi.");
    }
  }

  if (isLoggedIn) {
    Blynk.run();
    timer.run();
  }
}


void fn_checkPIR() {  
  int pirState = digitalRead(PIR_PIN);
  
  if (pirState == HIGH && !motionDetected) {
    pirTriggerTime = millis(); // catat waktu trigger
    fn_turnOnLamp();
    unsigned long responseTime = millis() - pirTriggerTime;
    fn_PrintMessageToConsole("Response PIR->Lamp: " + String(responseTime) + " ms");
    Blynk.virtualWrite(V10, responseTime); // kirim ke V10
    pirMsg = "Gerak=Y"; 
  }

  if (pirState == LOW && motionDetected) {
    if (millis() - lastMotionTime >= TIMEOUT_LAMP_OFF) {
      fn_turnOffLamp();
      pirMsg = "Gerak=N";   
      fn_PrintMessageToConsole("Tidak ada gerakan, Lampu OFF");   
    }
  }
  fn_refreshLCDBig();
}


void fn_checkUltrasonic() {
  unsigned int distance = sonar.ping_cm();  
  fn_PrintMessageToConsole(String("distance ") + distance);
  Blynk.virtualWrite(V5, distance); // kirim jarak ke V5

  if (distance > 0 && distance < maxDistanceRadar) {
    if (!lastSonarState) { 
      lastSonarState = true; 
	  counterOrang++;      
      sonarMsg = "Visit++";   
      visitorStatus = "Stop"; 
      fn_PrintMessageToConsole("Visitor bertambah");  
      Blynk.virtualWrite(V4, counterOrang); 
      Blynk.virtualWrite(V9, visitorStatus);       
    }
  } else {
    lastSonarState = false; 
    sonarMsg = "idle";   
    visitorStatus = "Safe"; 
    fn_PrintMessageToConsole("Tidak ada visitor baru");  
    Blynk.virtualWrite(V9, visitorStatus); 
  }
  fn_refreshLCDBig();
}


void fn_turnOnLamp() {
  digitalWrite(RELAY_PIN, HIGH);
  motionDetected = true;
  lastMotionTime = millis();
  Blynk.virtualWrite(V0, 1);  
  Blynk.virtualWrite(V1, 1);
  fn_PrintMessageToConsole("Lampu ON");    
}


void fn_turnOffLamp() {
  digitalWrite(RELAY_PIN, LOW);
  motionDetected = false;
  Blynk.virtualWrite(V0, 0);    
  Blynk.virtualWrite(V1, 0);
  fn_PrintMessageToConsole("Lampu OFF");
}


void fn_refreshLCDBig() {
  // Baris 1: Status visitor Stop/Safe
  lcdBig.setCursor(0, 0);
  lcdBig.print("Status: ");
  lcdBig.print(visitorStatus);
  lcdBig.print("        "); 

  // Baris 2: Visitor count
  lcdBig.setCursor(0, 1);
  lcdBig.print("Visitor: ");
  lcdBig.print(counterOrang);
  lcdBig.print("        "); 

  // Baris 3: Lamp ON/OFF
  lcdBig.setCursor(0, 2);
  lcdBig.print("Lamp: ");
  lcdBig.print(motionDetected ? "ON " : "OFF");
  lcdBig.print("        "); 

  // Baris 4: Gabungan pesan PIR dan Sonar
  lcdBig.setCursor(0, 3);  
  lcdBig.print("                     "); 
  lcdBig.setCursor(0, 3);
  lcdBig.print(pirMsg);
  lcdBig.setCursor(10, 3);
  lcdBig.print(" | ");
  lcdBig.print(sonarMsg);
}


void fn_lcdBig_Initialization() {
  lcdBig.init();
  lcdBig.backlight();
  lcdBig.setCursor(0, 0);
  lcdBig.print("System Locked...");
  lcdBig.setCursor(0, 1);
  lcdBig.print("Login Required");
}

void fn_PinHardware_Initialization() {
  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_BUILTIN_PIN, OUTPUT);
}

void fn_RunonLoginSuccess() {
  Serial.println("Login berhasil! Sistem aktif.");
  lcdBig.clear();
  lcdBig.setCursor(0,0);
  lcdBig.print("System Active");
  lcdBig.setCursor(0,1);
  lcdBig.print("Welcome!");
}

void fn_startSystem() {
  if (!systemStarted) {
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    timer.setInterval(150L, fn_checkUltrasonic); 
    timer.setInterval(500L, fn_checkPIR);
    timer.setInterval(1000L, fn_myTimerEvent);
    timer.setInterval(1000L, fn_Blink_LED);
    systemStarted = true;
  }
}


void fn_PrintMessageToConsole(String iMsg) {
  fn_PrintCurrentTime();
  Serial.println(iMsg);
}


void fn_PrintCurrentTime() {
  unsigned long currentTime = millis(); 
  unsigned long seconds = currentTime / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  seconds %= 60;
  minutes %= 60;
  hours %= 24;
  Serial.print("[");
  fn_padZero(hours);
  Serial.print(":");
  fn_padZero(minutes);
  Serial.print(":");
  fn_padZero(seconds);
  Serial.print("]: ");
}


void fn_padZero(unsigned long value) {
  if (value < 10) Serial.print('0');
  Serial.print(value);
}


void fn_Blink_LED() {
  static bool state = false;
  state = !state;
  digitalWrite(LED_BUILTIN_PIN, state ? HIGH : LOW);
}


void fn_myTimerEvent() {
  Blynk.virtualWrite(V2, millis() / 1000);
}


BLYNK_WRITE(V0) {
  int buttonState = param.asInt();
  if (buttonState == 1) fn_turnOnLamp();
  else fn_turnOffLamp();
}

BLYNK_CONNECTED() {
  Blynk.setProperty(V3, "offImageUrl","https://imgur.com/WHx1WG6");
  Blynk.setProperty(V3, "onImageUrl","https://imgur.com/a/gASrmP1");
