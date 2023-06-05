
#include <dht11.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

#define firebaseHost "https://iotarduinoproj-default-rtdb.firebaseio.com/"
#define firebaseAuth "AIzaSyAwjBBl9BzU9WDsyz971JRfHrrDtg9T0Ww"
#define ssid "GUSTO_209"
#define password "Gusto@123"
#define DHTTYPE DHT11
#define DHT11PIN D4

FirebaseData fbdo;
FirebaseConfig config;
FirebaseAuth auth;
Adafruit_ADS1X15 ads;
dht11 DHT11;

int16_t detectedValueFromMQ4 = 0;
int16_t detectedValueFromDHT11 = 0;
bool signupOK = false;

void setup() {
  Wire.begin(D2, D1);
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  config.api_key = firebaseAuth;
  config.database_url = firebaseHost;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  ads.begin();
}

void loop() {

  detectedValueFromMQ4 = ads.readADC_SingleEnded(0);
  int chk = DHT11.read(DHT11PIN);

  switch (chk) {
    case DHTLIB_OK:
      Serial.print("OK,\t");
      break;
    case DHTLIB_ERROR_CHECKSUM:
      Serial.print("Checksum error,\t");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      Serial.print("Time out error,\t");
      break;
    default:
      Serial.print("Unknown error,\t");
      break;
  }

  if (Firebase.ready() && signupOK) {

    if (Firebase.RTDB.setInt(&fbdo, "food_status", detectedValueFromMQ4)) {
      Serial.println("PASSED");
      Serial.println(detectedValueFromMQ4);
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setInt(&fbdo, "humidity", DHT11.humidity)) {
      Serial.println("PASSED");
      Serial.println(DHT11.humidity);
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setInt(&fbdo, "temperature", DHT11.temperature)) {
      Serial.println("PASSED");
      Serial.println(DHT11.temperature);
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }

  delay(1000);
}