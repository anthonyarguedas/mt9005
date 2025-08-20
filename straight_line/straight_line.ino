#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to enable it
#endif

// Motor A pins
const int AIA = 12;  // Motor A - Forward pin
const int AIB = 14;  // Motor A - Backward pin
// Motor B pins  
const int BIA = 13;  // Motor B - Forward pin
const int BIB = 15;  // Motor B - Backward pin

// LEDC properties
const int freq = 5000;
const int resolution = 8;

BluetoothSerial SerialBT;

byte motorA_speed = 0;
byte motorB_speed = 0;
bool motorsRunning = false; // Motor control state
String command = "";      // For incoming command strings

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_MotorControl"); // Bluetooth device name
  Serial.println("Ready to pair via Bluetooth");
  
  // LEDC Setup
  ledcAttach(AIA, freq, resolution);
  ledcAttach(AIB, freq, resolution);
  ledcAttach(BIA, freq, resolution);
  ledcAttach(BIB, freq, resolution);

  Serial.println("Motor control ready");
  Serial.println("Available commands:");
  Serial.println("- 'start' - Start motor movement");
  Serial.println("- 'stop' - Stop motor movement"); 
  Serial.println("- 'speed:XXX' - Set Motor B speed (0-255)");
}

void loop() {
  // Check for Bluetooth data
  if (SerialBT.available() > 0) {
    char incomingChar = SerialBT.read();
    
    // Build command string until newline
    if (incomingChar == '\n' || incomingChar == '\r') {
      if (command.length() > 0) {
        processCommand(command);
        command = ""; // Clear command buffer
      }
    } else {
      command += incomingChar;
    }
  }
  
  // Execute movement pattern only if motors are running
  if (motorsRunning) {
    forward(); 
    delay(100);
  } else {
    // Stop all motors
    stopMotors();
    delay(100);
  }
}

void processCommand(String cmd) {
  cmd.trim(); // Remove whitespace
  cmd.toLowerCase(); // Convert to lowercase for easier parsing
  
  Serial.print("Received command: ");
  Serial.println(cmd);
  
  if (cmd == "start") {
    motorsRunning = true;
    Serial.println("Motors started");
    SerialBT.println("Motors started");
    
  } else if (cmd == "stop") {
    motorsRunning = false;
    Serial.println("Motors stopped");
    SerialBT.println("Motors stopped");
    
  } else if (cmd.startsWith("speedA:")) {
    // Extract speed value after "speed:"
    String speedStr = cmd.substring(7); // Remove "speed:" prefix
    int speedValue = speedStr.toInt();
    
    // Validate speed range
    if (speedValue >= 0 && speedValue <= 255) {
      motorA_speed = speedValue;
      Serial.print("Motor A speed set to: ");
      Serial.println(motorA_speed);
      SerialBT.print("Motor A speed set to: ");
      SerialBT.println(motorA_speed);
    } else {
      Serial.println("Invalid speed value (0-255)");
      SerialBT.println("Invalid speed value (0-255)");
    }
  } else if (cmd.startsWith("speedB:")) {
    String speedStr = cmd.substring(7); // Remove "speed:" prefix
    int speedValue = speedStr.toInt();

    // Validate speed range
    if (speedValue >= 0 && speedValue <= 255) {
      motorB_speed = speedValue;
      Serial.print("Motor B speed set to: ");
      Serial.println(motorB_speed);
      SerialBT.print("Motor B speed set to: ");
      SerialBT.println(motorB_speed);
    } else {
      Serial.println("Invalid speed value (0-255)");
      SerialBT.println("Invalid speed value (0-255)");
    }
} else if (cmd.startsWith("speed:")) {
    String speedStr = cmd.substring(6); // Remove "speed:" prefix
    int speedValue = speedStr.toInt();

    // Validate speed range
    if (speedValue >= 0 && speedValue <= 255) {
      motorA_speed = speedValue;
      motorB_speed = speedValue;
      Serial.print("Motor speeds set to: ");
      Serial.println(motorA_speed);
      SerialBT.print("Motor speeds set to: ");
      SerialBT.println(motorA_speed);
    } else {
      Serial.println("Invalid speed value (0-255)");
      SerialBT.println("Invalid speed value (0-255)");
    }
  } else {
    Serial.println("Unknown command");
    SerialBT.println("Unknown command");
    SerialBT.println("Available: start, stop, speedA:N, speedB:N, speed:N");
  }
}

void stopMotors() {
  ledcWrite(AIA, 0);
  ledcWrite(AIB, 0);
  ledcWrite(BIA, 0);
  ledcWrite(BIB, 0);
}

void forward() {
  ledcWrite(AIA, motorA_speed);
  ledcWrite(AIB, 0);
  ledcWrite(BIA, motorB_speed);
  ledcWrite(BIB, 0);
}