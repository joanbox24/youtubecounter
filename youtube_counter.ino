

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// === Paramètres WiFi ===
const char* ssid = "Ton_SSID";
const char* password = "Ton_MDP";

// === API YouTube ===
const String apiKey = "TA_CLE_API";       // ⚠️ remplace par ta clé API Google
const String channelId = "TON_ID_CHAINE"; // ⚠️ remplace par ton ID de chaîne YouTube

// === Écran OLED 128x64 ===
Adafruit_SSD1306 display(128, 64, &Wire);

void setup() {
  Serial.begin(115200);

  // Connexion WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connexion WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" OK !");
  
  // Initialisation OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Échec écran OLED !");
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("YouTube Counter");
  display.display();
  delay(2000);
}

void loop() {
  if(WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();  // ✅ désactive la vérification SSL (évite erreur -5)

    HTTPClient http;

    String url = "https://www.googleapis.com/youtube/v3/channels?part=statistics&id=" 
                 + channelId + "&key=" + apiKey;

    http.begin(client, url);

    int httpCode = http.GET();
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      StaticJsonDocument<512> doc;
      DeserializationError error = deserializeJson(doc, payload);
      
      if (!error) {
        long subscribers = doc["items"][0]["statistics"]["subscriberCount"];

        // Affichage Serial
        Serial.print("Abonnés: ");
        Serial.println(subscribers);

        // Affichage OLED
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 10);
        display.println("Subs:");
        display.setTextSize(3);
        display.setCursor(0, 35);
        display.println(subscribers);
        display.display();
      } else {
        Serial.println("Erreur JSON");
      }
    } else {
      Serial.print("Erreur HTTP: ");
      Serial.println(httpCode);
    }
    http.end();
  } else {
    Serial.println("WiFi déconnecté !");
  }

  delay(60000); // mise à jour toutes les 60 sec
}
