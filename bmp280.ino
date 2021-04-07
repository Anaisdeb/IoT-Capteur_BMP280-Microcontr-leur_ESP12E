#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <PubSubClient.h>

const char *ssid = "mqtt";  //WIFI ssid
const char *password = "password";  //WIFI password
const char *mqtt_server = "192.168.137.118"; // MQTT server

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE 500
char msg[MSG_BUFFER_SIZE];

int i = 0;

//Define PIN-Wiring for I2C connection
#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

Adafruit_BMP280 bmp; // I2C

//Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
//Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

void setup() {
  Serial.begin(115200);
  connectWifi();
  client.setServer(mqtt_server, 1883);

  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

     snprintf (msg, MSG_BUFFER_SIZE, "%d,%f",i, bmp.readTemperature());
    Serial.print("Publish message temp: ");
    Serial.println(msg);
    client.publish("room/temperature", msg);
    snprintf (msg, MSG_BUFFER_SIZE, "%d,%f",i, bmp.readPressure());
    Serial.print("Publish message pression: ");
    Serial.println(msg);
    client.publish("room/pressure", msg);
    snprintf (msg, MSG_BUFFER_SIZE, "%d,%f",i, bmp.readAltitude(1013.25));
    Serial.print("Publish message altitude: ");
    Serial.println(msg);
    client.publish("room/altitude", msg);
    delay(1000);
      i++;
  
}

void connectWifi(){
  delay(1000);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  WiFi.begin(ssid, password);     //Connect to our WiFi router
  Serial.println("");
  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  //If connection successful show IP address in serial monitor 
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to our ESP
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
