#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

const char* SSID = "YOUR_SSID";
const char* PASSWORD = "YOUR_PASSWORD";

const int RED_PIN = 27;
const int BLUE_PIN = 26;
const int GREEN_PIN = 25;

const int RED = 0;
const int GREEN = 1;
const int BLUE = 2;

const int RESOLUTION = 8;
const int FREQUENCY = 5000;
const int MAX_VALUE = 255;

AsyncWebServer server(80);
AsyncWebSocket webSocket("/chat");

void initializeWiFiConnection() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print("."); delay(500);
    }
    Serial.println("\nLocal IP: " + WiFi.localIP().toString());
}

void initializeOutputs() {
    ledcSetup(RED, FREQUENCY, RESOLUTION);
    ledcSetup(GREEN, FREQUENCY, RESOLUTION);
    ledcSetup(BLUE, FREQUENCY, RESOLUTION);

    ledcAttachPin(RED_PIN, RED);
    ledcAttachPin(GREEN_PIN, GREEN);
    ledcAttachPin(BLUE_PIN, BLUE);

    ledcWrite(RED, MAX_VALUE);
    ledcWrite(GREEN, MAX_VALUE);
    ledcWrite(BLUE, MAX_VALUE);
}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    Serial.println("Websocket client connection received");
  } else if(type == WS_EVT_DISCONNECT){
    Serial.println("Client disconnected");
  } else if (type == WS_EVT_DATA) {
    DynamicJsonDocument document(16384);

    deserializeJson(document, (char *) data);
    int red = document["red"];
    int green = document["green"];
    int blue = document["blue"];
    Serial.print("red: ");
    Serial.println(red);

    Serial.print("green: ");
    Serial.println(green);

    Serial.print("blue: ");
    Serial.println(blue);
    
    Serial.println();
    
    // Use this if your RGB LED is common anode / Comment out if your RGB LED is common cathode
    ledcWrite(RED, MAX_VALUE - red);
    ledcWrite(GREEN, MAX_VALUE - green);
    ledcWrite(BLUE, MAX_VALUE - blue);
    // Use this if your RGB LED is common cathode / Comment out if your RGB LED is common anode
    // ledcWrite(RED, red);
    // ledcWrite(GREEN, green);
    // ledcWrite(BLUE, blue);
  }
}

void setup() {
    Serial.begin(9600);
    initializeOutputs();
    if(!SPIFFS.begin()){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
    
    initializeWiFiConnection();
    webSocket.onEvent(onWsEvent);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html");
    });

    server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.js");
    });

    server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/styles.css");
    });

    server.addHandler(&webSocket);
    server.begin();

    Serial.println("Server is running!");
}

void loop() {}