#include <PZEM004Tv30.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <TaskScheduler.h>

#define EEPROM_SIZE 2

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

int room_1_power_status = 0;
int room_2_power_status = 0;
byte response_room_1_power_status = 0;
byte response_room_2_power_status = 0;

void read_pzem();
void update_room_power_status();

Task task_read_pzem(1000, TASK_FOREVER, &read_pzem);
Task task_update_room_power_status(1000, TASK_FOREVER, &update_room_power_status);

Scheduler runner;

void setup() {
    EEPROM.begin(EEPROM_SIZE);

    //  Write initial value to EEPROM of new ESP32 board
    //  EEPROM.write(room_1_power_status_address, room_1_power_status);
    //  EEPROM.write(room_2_power_status_address, room_2_power_status);
    //  EEPROM.commit();
    
    room_1_power_status = EEPROM.read(room_1_power_status_address);
    room_2_power_status = EEPROM.read(room_2_power_status_address);

    delay(100);
  
    pinMode(ROOM_1_RELAY_PIN, OUTPUT);
    pinMode(ROOM_2_RELAY_PIN, OUTPUT);
    
    Serial.begin(115200);

    // For each PZEM, initialize it
    for(int i = 0; i < NUM_PZEMS; i++)
    {
      pzems[i] = PZEM004Tv30(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN, 0x01 + i);
    }

    //  Set power status relays to last value from EERPOM    
    update_room_power_status();
    
    connectToWiFi();

    runner.init();
    runner.addTask(task_read_pzem);
    runner.addTask(task_update_room_power_status);
  
    task_read_pzem.enable();
    task_update_room_power_status.enable();

    // Uncomment to reset PZEMs
    //  reset_all_pzems();
}

void loop() {
    runner.execute();
}

void update_room_power_status() {
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
