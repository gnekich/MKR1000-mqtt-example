/*
  Author: Gordan Nekić <gordan@neki.ch>
  Project usecase: this is designed to quickly demo what can be done with Arduino MKR 1000
  Please do not use this in production in any way, this is a school project for Kids (Members of KIOK)
  It is great for learning, nothing else.
  License: MIT
*/

#include <ArduinoMqttClient.h>
#include <WiFi101.h>
#include "arduino_secrets.h"
#include "MKR1000_defs.h"
#include <string>

// #include <Servo.h>

// Servo MyServo;

// Secrets for the WiFi are saved in arduino_secrets.h please update it there
char ssid[] = SECRET_SSID; // your network SSID
char pass[] = SECRET_PASS; // your network password

// Create WiFi client
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// Set everything that we will need for connecting to MQTT broker
const int port = MQTT_PORT;
const char mqtt_host[] = MQTT_HOST;
const char client_id[] = CLIENT_ID;
const char mqtt_username[] = MQTT_USERNAME;
const char mqtt_password[] = MQTT_PASSWORD;

const char topic1[] = "kiok/feeds/lampice";
const char topic2[] = "kiok/feeds/relay";
const char topic3[] = "kiok/feeds/motor";

const int shouldIListenToTopic3 = 1; // Flip this to 0 if you don't need to listen for third topic
int motorWorking = 0;

void setup()
{
  // Init GPIO
  // Set inputs
  pinMode(D0, INPUT); // Touch Button
  pinMode(D1, INPUT); // White Button
  pinMode(D2, INPUT); // Red Button

  // Set outputs
  pinMode(D7, OUTPUT); // Red Led
  pinMode(D8, OUTPUT); // Relay

  // l293d
  pinMode(D11, OUTPUT); // Motor 1 direction control 1
  pinMode(D12, OUTPUT); // Motor 1 direction control 2
  pinMode(A2, OUTPUT);  // Motor speed by using PWM

  digitalWrite(D11, HIGH);
  digitalWrite(D12, LOW);

  // Set servo
  // MyServo.attach(D9);

  // Initialize serial and wait for port to open:
  Serial.begin(9600);

  // No need to wait, this is legacy
  /*
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  */

  // Attempt to connect to Wifi network:
  Serial.print("Attempting to connect to SSID: ");
  Serial.print(ssid);
  Serial.println();

  // Loop while not connected
  while (WiFi.begin(ssid, pass) != WL_CONNECTED)
  {
    // Failed, retry
    Serial.print(".");
    delay(5000);
  }

  // Success! We are connected!
  Serial.println("You're connected to the network!");
  Serial.println();

  // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
  // mqttClient.setId(CLIENT_ID);

  // You can provide a username and password for authentication
  mqttClient.setUsernamePassword(mqtt_username, mqtt_password);

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(mqtt_host);

  if (!mqttClient.connect(mqtt_host, port))
  {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    // Error while connecting to MQTT
    while (1)
      ;
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  // Set the message receive callback
  mqttClient.onMessage(onMqttMessage);

  Serial.print("Subscribing to topic: ");
  Serial.println(topic1);
  Serial.println();

  // Subscribe to a topic
  mqttClient.subscribe(topic1);
  mqttClient.subscribe(topic2);
  mqttClient.subscribe(topic3);

  // Topics can be unsubscribed using:
  // mqttClient.unsubscribe(topic);

  Serial.print("Topic: ");
  Serial.println(topic1);
  Serial.print("Topic: ");
  Serial.println(topic2);
  Serial.print("Topic: ");
  Serial.println(topic3);

  Serial.println();
}

void loop()
{
  // call poll() regularly to allow the library to receive MQTT messages and
  // send MQTT keep alive which avoids being disconnected by the broker
  mqttClient.poll();

  int buttonTouchState = digitalRead(D0);
  if (buttonTouchState)
  {
    Serial.println("Taknuo si me! Touch senzor V2 aktivan!");
    mqttClient.beginMessage(topic1);
    mqttClient.print("ON");
    mqttClient.endMessage();

    mqttClient.beginMessage(topic3);
    mqttClient.print("ON");
    mqttClient.endMessage();
    delay(1000);
  }

  int buttonWhiteState = digitalRead(D1);
  if (buttonWhiteState)
  {
    Serial.println("OFF Button pressed!");
    mqttClient.beginMessage(topic1);
    mqttClient.print("OFF");
    mqttClient.endMessage();

    mqttClient.beginMessage(topic2);
    mqttClient.print("OFF");
    mqttClient.endMessage();
    delay(300);
  }

  int buttonRedState = digitalRead(D2);
  if (buttonRedState)
  {
    Serial.println("ON Button pressed!");
    mqttClient.beginMessage(topic1);
    mqttClient.print("ON");
    mqttClient.endMessage();

    mqttClient.beginMessage(topic2);
    mqttClient.print("ON");
    mqttClient.endMessage();
    delay(300);
  }

  if (motorWorking == 1)
  {
    // Kick
    // analogWrite(A2, 255);
    // delay(25);
    // Run
    // for (int i = 145; i >= 30; --i) {
    //  analogWrite(A2, i);
    //  delay(30);
    //}
  }
}

void onMqttMessage(int messageSize)
{
  // Get topic
  String topic = ""; // Use Strings :( , but this is demo project anyway... :)
  topic = mqttClient.messageTopic();

  // Use the Stream interface to print the contents
  String messagePayload = ""; // Use Strings :( , but this is demo project anyway... :)
  while (mqttClient.available())
  {
    messagePayload += (char)mqttClient.read();
    // Serial.print((char)mqttClient.read());
  }

  // We received a message, print out the topic and contents
  Serial.println("Received a message with topic '");
  Serial.print(topic);
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");
  Serial.println(messagePayload);

  // Logika za komande
  if (topic == topic1)
  {
    if (messagePayload == "ON" || messagePayload == "on") // Sometimes it is easier to check than to lowercase ;)
    {
      digitalWrite(D7, HIGH);
    }
    else if (messagePayload == "OFF" || messagePayload == "off")
    {
      digitalWrite(D7, LOW);
    }
  }

  if (topic == topic2)
  {
    if (messagePayload == "ON" || messagePayload == "on")
    {
      digitalWrite(D8, HIGH);
    }
    else if (messagePayload == "OFF" || messagePayload == "off")
    {
      digitalWrite(D8, LOW);
    }
  }

  if (topic == topic3 && shouldIListenToTopic3 == 1)
  {
    if (messagePayload == "ON" || messagePayload == "on")
    {
      motorWorking = 1;
      for (int i = 145; i >= 50; --i)
      {
        analogWrite(A2, i);
        delay(30);
      }
    }
    else if (messagePayload == "OFF" || messagePayload == "off")
    {
      motorWorking = 0;
    }
  }

  Serial.println();
  Serial.println();
}
