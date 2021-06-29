/**
 * 
 * Make simple Asynchronous webserver using DHT22 for log temperature and humidity.
 * 
 */
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <DHTStable.h>
#define DHT22_PIN 4

DHTStable DHT;

AsyncWebServer server(8081);// Create a asynchronous webserver object that listens for HTTP request on port 8081

int led_status = 0;
float t = 0;
float h = 0;

const int LED_PIN = 2;
const int OK_HTTP = 200;

//Endpoints
const char* DHT22 = "/async-esp8285/api/v1/dht22";
const char* TEMPERATURES = "/async-esp8285/api/v1/dht22/temperatures";
const char* HUMIDITIES = "/async-esp8285/api/v1/dht22/humidities";
const char* LED = "/async-esp8285/api/v1/led";

const char* DHT_22 = "DHT-22";
const char* AM2302 = "AM2302";
const char* CENTIGRADE = "ºC";
const char* PERCENTAGE = "%";
const char* TEXT_PLAIN = "text/plain";
const char* APPLICATION_JSON = "application/json";
const char* ON = "ON";
const char* OFF = "OFF";
const char* ID = "id";
const char* NEW_STATUS = "status";
const char* SENSOR = "sensor";
const char* TYPE = "type";
const char* TEMPERATURE = "temperature";
const char* HUMIDITY = "humidity";

//Wifi network credentials
const char* ssid = "Filomena 2.4";
const char* password = "zAHF4n+MMHniN*G"; //aqui les dejo mi contraseña para que se conecten, wifi free :)

void setup(void) {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  delay(10);
  
  Serial.println('\n');
  WiFi.begin(ssid, password);

  //wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  initRoute();
  temperatures();
  humidities();
  dht22();
  handleLED();
  handleNotFound();

  server.begin();// Actually start the server
  Serial.println("HTTP server started");
}

void loop(void) {
  DHT.read22(DHT22_PIN);
  t = DHT.getTemperature();
  h = DHT.getHumidity();
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(PERCENTAGE);
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(CENTIGRADE);
  delay(2000);
}

//Welcome webserver Esp8266!!!
void initRoute() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request -> send(OK_HTTP, TEXT_PLAIN, "Welcome webserver Esp8266");
  });
}

//Temperaturesª
void temperatures() {
  server.on(TEMPERATURES, HTTP_GET, [](AsyncWebServerRequest * request) {
    String json;
    StaticJsonDocument<300> doc;
    doc[SENSOR] = DHT_22; 
    doc[TYPE] = AM2302;
    doc[TEMPERATURE] = t;
    serializeJson(doc, json);
    request -> send(OK_HTTP, APPLICATION_JSON, json);
  });
}

//Humidities%
void humidities() {
  server.on(HUMIDITIES, HTTP_GET, [](AsyncWebServerRequest * request) {
    String json;
    StaticJsonDocument<300> doc;
    doc[SENSOR] = DHT_22; 
    doc[TYPE] = AM2302;
    doc[HUMIDITY] = h;
    serializeJson(doc, json);
    request -> send(OK_HTTP, APPLICATION_JSON, json);
  });
}

//DHT22 humidities% and temperaturesª
void dht22() {
  server.on(DHT22, HTTP_GET, [](AsyncWebServerRequest * request) {
    String json;
    StaticJsonDocument<300> doc;
    doc[SENSOR] = DHT_22; 
    doc[TYPE] = AM2302;
    doc[TEMPERATURE] = t;
    doc[HUMIDITY] = h;
    serializeJson(doc, json);
    request -> send(OK_HTTP, APPLICATION_JSON, json);
  });
}

// On/off Led
void handleLED() {
  //Show led status
  server.on(LED, HTTP_GET, [](AsyncWebServerRequest * request) {
    led_status = digitalRead(LED_PIN);
    if (led_status == HIGH) {
      String json;
      StaticJsonDocument<300> doc;
      doc[ID] = LED_PIN;
      doc[NEW_STATUS] = ON;
      serializeJson(doc, json);
      request -> send(OK_HTTP, APPLICATION_JSON, json);
    } else {
      String json;
      StaticJsonDocument<300> doc;
      doc[ID] = LED_PIN;
      doc[NEW_STATUS] = OFF;
      serializeJson(doc, json);
      request -> send(OK_HTTP, APPLICATION_JSON, json);
    }
  });
  //async-esp8266/api/v1/led?id=2&status=on //ON
  //async-esp8266/api/v1/led?id=2&status=off //OFF
  server.on(LED, HTTP_PUT, [](AsyncWebServerRequest * request) {
    String id = request-> arg(ID);
    String new_status = request-> arg(NEW_STATUS);
    if (id.toInt() == LED_PIN && new_status.equalsIgnoreCase(ON)) {
      Serial.println("PIN HIGH");
      digitalWrite(LED_PIN, HIGH);
      String json;
      StaticJsonDocument<300> doc;
      doc[ID] = LED_PIN;
      doc[NEW_STATUS] = ON;
      serializeJson(doc, json);
      request -> send(OK_HTTP, APPLICATION_JSON, json);
    } else if (id.toInt() == LED_PIN && new_status.equalsIgnoreCase(OFF)) {
      Serial.println("PIN LOW");
      digitalWrite(LED_PIN, LOW);
      String json;
      StaticJsonDocument<300> doc;
      doc[ID] = LED_PIN;
      doc[NEW_STATUS] = OFF;
      serializeJson(doc, json);
      request -> send(OK_HTTP, APPLICATION_JSON, json);
    } else {
      request -> send(400, TEXT_PLAIN, "Bad Request");
    }
  });
}

//404
void handleNotFound() {
  //Error 404 not found
  server.onNotFound([](AsyncWebServerRequest * request) {
    request->send(404, TEXT_PLAIN, "Sorry, 404 not found.");
  });
}

// -- EOF --
