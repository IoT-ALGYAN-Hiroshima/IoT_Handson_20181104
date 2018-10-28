#include <WiFiST.h>
#include <PubSubClient.h>


// Update these with values suitable for your network.
char* ssid              = "kei.nak5";
const char* password    = "kei.nak5";
const char* mqtt_server = "192.168.137.1";
SPIClass SPI_3(PC12, PC11, PC10);
WiFiClass WiFi(&SPI_3, PE0, PE1, PE8, PB13);

char *pubTopic = "/pub/STM32";
char *subTopic = "/sub/STM32";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.firmwareVersion());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(pubTopic, "Coming Packets");
      int qos = 0;
//      client.subscribe(subTopic, qos);
      Serial.println("Subscribed.");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(2400);
  setup_wifi();
  
//  client.setServer(mqtt_server, 1883);
//  client.setCallback(mqttCallback);
}
   
void mqttCallback (char* topic, uint8_t* payload, uint32_t length) {
    char buff[256]; 
    String str = "";
    Serial.print("Received. topic=");
    Serial.println(topic);
    sprintf(buff, "%s:%d", payload, length);
    Serial.println(buff);
#if 0
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
        str += (char)payload[i];
    }
    Serial.print("\n");
    delay(300);
#endif
}

void loop() {
/*
  static int count = 0;
    if (count == 4) {
      Serial.println("Firmware update start.");
      WiFi.FirmwareUpdate();
      Serial.println("Firmware update finished.");
    
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      Serial.println(WiFi.firmwareVersion());
    }
    count++;
    delay(1000);
*/
  if (!client.connected()) {
    Serial.print("*");
    reconnect();
  } else {
    long now = millis();
    if (now - lastMsg > 2000) {
      lastMsg = now;
      ++value;
      snprintf (msg, 75, "hello world #%ld", value);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish(pubTopic, msg);
    } else {
      Serial.print("<");
      client.loop();
      Serial.print(">");
    }
  }
}


