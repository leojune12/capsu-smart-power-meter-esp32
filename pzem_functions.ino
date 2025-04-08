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

void reset_all_pzems() {
    for (int i = 0; i < NUM_PZEMS; i++) {
        if (pzems[i].resetEnergy()) {
            Serial.print("PZEM ");
            Serial.print(i + 1);
            Serial.println(" energy reset successful.");
        } else {
            Serial.print("PZEM ");
            Serial.print(i + 1);
            Serial.println(" energy reset failed.");
        }
    }
}
