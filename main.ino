#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL67NBiS_rL"
#define BLYNK_TEMPLATE_NAME "Project IoT Smart Lamp"
#define BLYNK_AUTH_TOKEN "6YD0Jk_-u1OhNKtKTpQcnDj9N0zsr1fI"

#include <BlynkSimpleEsp32.h>
#include <WiFi.h>

const char* WIFI_SSID = "SYAHREZA-LAPTOP";
const char* WIFI_PASS = "syahreza2003";

#define LED_1 18
#define LED_2 23
#define MOTION_SENSOR 27
#define LIGHT_SENSOR 4
#define ONBOARD_LED 2
#define WIFI_CONNECTION_TIMEOUT 10000

unsigned long wifiConnectionStartTime;
bool isLampu1ManuallyControlled = false;
bool isLampu2ManuallyControlled = false;
int previousLightSensorReading = LOW;
int previousMotionSensorReading = LOW;
bool isLampu1TriggeredByMotionSensor = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(ONBOARD_LED, OUTPUT);

  pinMode(LIGHT_SENSOR, INPUT);
  pinMode(MOTION_SENSOR, INPUT);


  Serial.begin(9600);

  connectToWiFi();

  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);
  
  Blynk.virtualWrite(V4, 1);
}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();

  checkWiFiConnection();


  if (isLampu1ManuallyControlled) {
    Blynk.virtualWrite(V2, "Manual on Switch");
  } else {
    Blynk.virtualWrite(V2, "Auto on Sensor");
  }

  if (isLampu2ManuallyControlled) {
    Blynk.virtualWrite(V3, "Manual on Switch");
  } else {
    Blynk.virtualWrite(V3, "Auto on Sensor");
  }

  // LIGHT SENSOR CODE
  int lightSensorReading = digitalRead(LIGHT_SENSOR);

  if (lightSensorReading != previousLightSensorReading) {
    if (lightSensorReading == HIGH) {
      if (!isLampu1ManuallyControlled) {
        digitalWrite(LED_1, HIGH);

        Blynk.virtualWrite(V0, 1);
      } 
      if (!isLampu2ManuallyControlled) {
        digitalWrite(LED_2, HIGH);

        Blynk.virtualWrite(V1, 1);
      }
    } else {
      if (!isLampu1ManuallyControlled) {
        digitalWrite(LED_1, LOW);

        Blynk.virtualWrite(V0, 0);
      }
      if (!isLampu2ManuallyControlled) {
        digitalWrite(LED_2, LOW);

        Blynk.virtualWrite(V1, 0);
      }
    }

    previousLightSensorReading = lightSensorReading;
  }

  // MOTION SENSOR READING
  int motionSensorReading = digitalRead(MOTION_SENSOR);

  if (motionSensorReading != previousMotionSensorReading) {
    if (motionSensorReading == HIGH) {
      if (!isLampu1ManuallyControlled) {
        digitalWrite(LED_1, HIGH);
        isLampu1TriggeredByMotionSensor = true;

        Blynk.virtualWrite(V0, 1);
      } 
    } else {
      if (!isLampu1ManuallyControlled && digitalRead(LED_1) == HIGH && isLampu1TriggeredByMotionSensor) {
        digitalWrite(LED_1, LOW);
        isLampu1TriggeredByMotionSensor = false;

        Blynk.virtualWrite(V0, 0);
      }
    }

    previousMotionSensorReading = motionSensorReading;
  }
}


BLYNK_WRITE(V0) {
  int virtualPinValue = param.asInt();

  isLampu1ManuallyControlled = !isLampu1ManuallyControlled;

  if (virtualPinValue == 1) {
    digitalWrite(LED_1, HIGH);
  } else {
    digitalWrite(LED_1, LOW);
  }
}

BLYNK_WRITE(V1) {
  int virtualPinValue = param.asInt();

  isLampu2ManuallyControlled = !isLampu2ManuallyControlled;

  if (virtualPinValue == 1) {
    digitalWrite(LED_2, HIGH);
  } else {
    digitalWrite(LED_2, LOW);
  }
}

BLYNK_WRITE(V4) {
  int virtualPinValue = param.asInt();

  if (virtualPinValue == 1) {
    pinMode(MOTION_SENSOR, INPUT);
  } else {
    pinMode(MOTION_SENSOR, OUTPUT);
  }
}

void connectToWiFi() {
  Serial.println("Connecting to Wi-Fi...");

  wifiConnectionStartTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - wifiConnectionStartTime < WIFI_CONNECTION_TIMEOUT) {
    digitalWrite(ONBOARD_LED, HIGH);
    delay(500);
    digitalWrite(ONBOARD_LED, LOW);
    delay(500);

    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi connected.");
  } else {
    Serial.println("\nFailed to connect to Wi-Fi.");
  }
}

void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ONBOARD_LED, HIGH);
    delay(500);
    digitalWrite(ONBOARD_LED, LOW);
    delay(500);
  }
}

