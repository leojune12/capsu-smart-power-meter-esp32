void post_request() {
  task_read_pzem.disable();
  
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
//        const char* room_1_voltage = responseDoc["room_1_voltage"];
//        const char* room_1_current = responseDoc["room_1_current"];
//        const char* room_1_power = responseDoc["room_1_power"];
//        const char* room_1_energy = responseDoc["room_1_energy"];
//        const char* room_1_frequency = responseDoc["room_1_frequency"];
//        const char* room_1_pf = responseDoc["room_1_pf"];
//
//        const char* room_2_voltage = responseDoc["room_2_voltage"];
//        const char* room_2_current = responseDoc["room_2_current"];
//        const char* room_2_power = responseDoc["room_2_power"];
//        const char* room_2_energy = responseDoc["room_2_energy"];
//        const char* room_2_frequency = responseDoc["room_2_frequency"];
//        const char* room_2_pf = responseDoc["room_2_pf"];
//
//        Serial.println("Parsed response:");
//        Serial.println("room_1_voltage: " + String(room_1_voltage));
//        Serial.println("room_1_current: " + String(room_1_current));
//        Serial.println("room_1_power: " + String(room_1_power));
//        Serial.println("room_1_energy: " + String(room_1_energy));
//        Serial.println("room_1_frequency: " + String(room_1_frequency));
//        Serial.println("room_1_pf: " + String(room_1_pf));
//        Serial.println();
//        Serial.println("room_2_voltage: " + String(room_2_voltage));
//        Serial.println("room_2_current: " + String(room_2_current));
//        Serial.println("room_2_power: " + String(room_2_power));
//        Serial.println("room_2_energy: " + String(room_2_energy));
//        Serial.println("room_2_frequency: " + String(room_2_frequency));
//        Serial.println("room_2_pf: " + String(room_2_pf));

        response_room_1_power_status = int(responseDoc["room_1_power_status"]);

        if (room_1_power_status != response_room_1_power_status) {
            room_1_power_status = response_room_1_power_status;
            EEPROM.write(room_1_power_status_address, room_1_power_status);\
            EEPROM.commit();
        }

        response_room_2_power_status = int(responseDoc["room_2_power_status"]);
    
        if (room_2_power_status != response_room_2_power_status) {
            room_2_power_status = response_room_2_power_status;
            EEPROM.write(room_2_power_status_address, room_2_power_status);
            EEPROM.commit();
        }
      } else {
        Serial.println("Failed to parse JSON response");
      }
    } else {
      Serial.println("Error on HTTP request");
    }

    http.end();
  }

  task_read_pzem.enable();
}
