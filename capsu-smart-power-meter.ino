#include <PZEM004Tv30.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <TaskScheduler.h>

#define PZEM_SERIAL Serial2
#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17
#define NUM_PZEMS 2

#define ROOM_1_RELAY_PIN 32
#define ROOM_2_RELAY_PIN 33

PZEM004Tv30 pzems[NUM_PZEMS];

// Replace with your network credentials
const char* ssid = "Black Mamba";
const char* password = "Godissogood";

// Replace with your server's URL
const char* serverName = "http://192.168.8.102:8000/api/test";

struct reading {
    float voltage;
    float current;
    float power;
    float energy;
    float frequency;
    float pf; 
};

reading room_1 = {0,0,0,0,0,0};
reading room_2 = {0,0,0,0,0,0};

// EEPROM Address
byte room_1_power_status_address = 0;
byte room_2_power_status_address = 1;

byte room_1_power_status = 0;
byte room_2_power_status = 0;

void read_pzem();
void update_room_status();

Task task_read_pzem(2000, TASK_FOREVER, &read_pzem);
Task task_update_room_status(2000, TASK_FOREVER, &update_room_status);

Scheduler runner;

void setup() {
//    EEPROM.put(room_1_power_status_address, room_1_power_status);
    EEPROM.get(room_1_power_status_address, room_1_power_status);

//    EEPROM.put(room_2_power_status_address, room_2_power_status);
    EEPROM.get(room_2_power_status_address, room_2_power_status);
  
    pinMode(ROOM_1_RELAY_PIN, OUTPUT);
    pinMode(ROOM_2_RELAY_PIN, OUTPUT);
    
    Serial.begin(115200);

    // For each PZEM, initialize it
    for(int i = 0; i < NUM_PZEMS; i++)
    {
      pzems[i] = PZEM004Tv30(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN, 0x01 + i);
    }

    connectToWiFi();

    runner.init();
    runner.addTask(task_read_pzem);
    runner.addTask(task_update_room_status);
  
    task_read_pzem.enable();
    task_update_room_status.enable();
}

void loop() {
    runner.execute();
}

void read_pzem() {
    // Print out the measured values from each PZEM module
    for(int i = 0; i < NUM_PZEMS; i++){
        if (pzems[i].getAddress() == 1) {
            // Print the Address of the PZEM
            Serial.print("PZEM ");
            Serial.print(i);
            Serial.print(" - Address:");
            Serial.println(pzems[i].getAddress(), HEX);
            Serial.println("===================");
    
    
    
            // Read the data from the sensor
            room_1.voltage = pzems[i].voltage();
            room_1.current = pzems[i].current();
            room_1.power = pzems[i].power();
            room_1.energy = pzems[i].energy();
            room_1.frequency = pzems[i].frequency();
            room_1.pf = pzems[i].pf();
    
    
            // Check if the data is valid
            if(isnan(room_1.voltage)){
                Serial.println("Error reading voltage");
            } else if (isnan(room_1.current)) {
                Serial.println("Error reading current");
            } else if (isnan(room_1.power)) {
                Serial.println("Error reading power");
            } else if (isnan(room_1.energy)) {
                Serial.println("Error reading energy");
            } else if (isnan(room_1.frequency)) {
                Serial.println("Error reading frequency");
            } else if (isnan(room_1.pf)) {
                Serial.println("Error reading power factor");
            } else {
                // Print the values to the Serial console
                Serial.print("Voltage: ");      Serial.print(room_1.voltage);      Serial.println("V");
                Serial.print("Current: ");      Serial.print(room_1.current);      Serial.println("A");
                Serial.print("Power: ");        Serial.print(room_1.power);        Serial.println("W");
                Serial.print("Energy: ");       Serial.print(room_1.energy,3);     Serial.println("kWh");
                Serial.print("Frequency: ");    Serial.print(room_1.frequency, 1); Serial.println("Hz");
                Serial.print("PF: ");           Serial.println(room_1.pf);
            }
    
            Serial.println("-------------------");
            Serial.println();
        }

        if (pzems[i].getAddress() == 2) {
            // Print the Address of the PZEM
            Serial.print("PZEM ");
            Serial.print(i);
            Serial.print(" - Address:");
            Serial.println(pzems[i].getAddress(), HEX);
            Serial.println("===================");
    
    
    
            // Read the data from the sensor
            room_2.voltage = pzems[i].voltage();
            room_2.current = pzems[i].current();
            room_2.power = pzems[i].power();
            room_2.energy = pzems[i].energy();
            room_2.frequency = pzems[i].frequency();
            room_2.pf = pzems[i].pf();
    
    
            // Check if the data is valid
            if(isnan(room_2.voltage)){
                Serial.println("Error reading voltage");
            } else if (isnan(room_2.current)) {
                Serial.println("Error reading current");
            } else if (isnan(room_2.power)) {
                Serial.println("Error reading power");
            } else if (isnan(room_2.energy)) {
                Serial.println("Error reading energy");
            } else if (isnan(room_2.frequency)) {
                Serial.println("Error reading frequency");
            } else if (isnan(room_2.pf)) {
                Serial.println("Error reading power factor");
            } else {
                // Print the values to the Serial console
                Serial.print("Voltage: ");      Serial.print(room_2.voltage);      Serial.println("V");
                Serial.print("Current: ");      Serial.print(room_2.current);      Serial.println("A");
                Serial.print("Power: ");        Serial.print(room_2.power);        Serial.println("W");
                Serial.print("Energy: ");       Serial.print(room_2.energy,3);     Serial.println("kWh");
                Serial.print("Frequency: ");    Serial.print(room_2.frequency, 1); Serial.println("Hz");
                Serial.print("PF: ");           Serial.println(room_2.pf);
            }
    
            Serial.println("-------------------");
            Serial.println();
        }
    }

    post_request();
}

void reset_energy(byte pzem_address) {
  pzems[pzem_address].resetEnergy();
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connected to WiFi");
}

void post_request() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  // Make a POST request
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    // Create JSON object
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["room_1_voltage"] = room_1.voltage;
    jsonDoc["room_1_current"] = room_1.current;
    jsonDoc["room_1_power"] = room_1.power;
    jsonDoc["room_1_energy"] = room_1.energy;
    jsonDoc["room_1_frequency"] = room_1.frequency;
    jsonDoc["room_1_pf"] = room_1.pf;

    jsonDoc["room_2_voltage"] = room_2.voltage;
    jsonDoc["room_2_current"] = room_2.current;
    jsonDoc["room_2_power"] = room_2.power;
    jsonDoc["room_2_energy"] = room_2.energy;
    jsonDoc["room_2_frequency"] = room_2.frequency;
    jsonDoc["room_2_pf"] = room_2.pf;

    String requestBody;
    serializeJson(jsonDoc, requestBody);

    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response: " + response);

      // Parse JSON response
      StaticJsonDocument<200> responseDoc;
      DeserializationError error = deserializeJson(responseDoc, response);

      if (!error) {
        const char* room_1_voltage = responseDoc["room_1_voltage"];
        const char* room_1_current = responseDoc["room_1_current"];
        const char* room_1_power = responseDoc["room_1_power"];
        const char* room_1_energy = responseDoc["room_1_energy"];
        const char* room_1_frequency = responseDoc["room_1_frequency"];
        const char* room_1_pf = responseDoc["room_1_pf"];
        room_1_power_status = int(responseDoc["room_1_power_status"]);

        const char* room_2_voltage = responseDoc["room_2_voltage"];
        const char* room_2_current = responseDoc["room_2_current"];
        const char* room_2_power = responseDoc["room_2_power"];
        const char* room_2_energy = responseDoc["room_2_energy"];
        const char* room_2_frequency = responseDoc["room_2_frequency"];
        const char* room_2_pf = responseDoc["room_2_pf"];
        room_2_power_status = int(responseDoc["room_2_power_status"]);

        Serial.println("Parsed response:");
        Serial.println("room_1_voltage: " + String(room_1_voltage));
        Serial.println("room_1_current: " + String(room_1_current));
        Serial.println("room_1_power: " + String(room_1_power));
        Serial.println("room_1_energy: " + String(room_1_energy));
        Serial.println("room_1_frequency: " + String(room_1_frequency));
        Serial.println("room_1_pf: " + String(room_1_pf));
        Serial.println();
        Serial.println("room_2_voltage: " + String(room_2_voltage));
        Serial.println("room_2_current: " + String(room_2_current));
        Serial.println("room_2_power: " + String(room_2_power));
        Serial.println("room_2_energy: " + String(room_2_energy));
        Serial.println("room_2_frequency: " + String(room_2_frequency));
        Serial.println("room_2_pf: " + String(room_2_pf));
      } else {
        Serial.println("Failed to parse JSON response");
      }
    } else {
      Serial.println("Error on HTTP request");
    }

    http.end();
  }
}

void update_room_status() {
    if (room_1_power_status == 0) {
        digitalWrite(ROOM_1_RELAY_PIN, LOW);
    } else {
        digitalWrite(ROOM_1_RELAY_PIN, HIGH);
    }

    if (room_2_power_status == 0) {
        digitalWrite(ROOM_2_RELAY_PIN, LOW);
    } else {
        digitalWrite(ROOM_2_RELAY_PIN, HIGH);
    }
}
