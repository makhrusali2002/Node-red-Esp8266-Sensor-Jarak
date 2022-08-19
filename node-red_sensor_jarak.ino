#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <HCSR04.h>

#define triggerPin  D0
#define echoPin     D1

//MQTT TOPIC
#define MQTT_JARAK "/pkl/jarak"
#define MQTT_DURASI "/pkl/durasi"

long duration, jarak1;
const char* ssid = "PKL DUINO"; 
const char* password = "smgt77merdeka"; 
const char* mqtt_server = "broker.mqtt-dashboard.com";
unsigned long startMillis=0; //variabel menyimpan milidetik terakhir dari loop

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void JARAK_config(){
  long duration, jarak1;
  if ((jarak1) && (duration)){
    client.publish(MQTT_JARAK, "0");
    client.publish(MQTT_DURASI, "0");
    return;
  }
  
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2); 
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10); 
    digitalWrite(triggerPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    jarak1 = (duration/2) / 29.1;
    
  client.publish(MQTT_JARAK, String(jarak1).c_str());
  client.publish(MQTT_DURASI, String(duration).c_str());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "belajarmqttt";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) 
    {
      Serial.println("connected");
      client.publish("/pkl/jarak",  "Online");
      //client.subscribe("durasi");
    } 
    else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin (9600);
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);

}

void loop() {
  unsigned long currentMillis = millis();// variabel mengambil waktu yang berjalan dan menyimpan di variabel current millis
  if (currentMillis - startMillis >= 2000) { //setiap milis_sekarang - hitungan_milis yang mencapai nilai lebih besar atau sama dengan 2000
    startMillis = currentMillis;
    long duration,jarak1;
    JARAK_config();
    if (!client.connected()){
      reconnect();
    }
    
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2); 
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10); 
    digitalWrite(triggerPin, LOW);
    
    duration = pulseIn(echoPin, HIGH);
    jarak1 = (duration/2) / 29.1;
    
    Serial.print("Jarak :");
    Serial.print(jarak1);
    Serial.println(" cm");
  }
}
