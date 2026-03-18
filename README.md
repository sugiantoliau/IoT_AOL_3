
### **🛠 Project Overview**

Title: IoT-Based Smart Room Control: Virtual Mock-up with Edge Logic and Blynk.io Integration

Description: 
This ESP32-based Smart Room project, simulated on Wokwi, uses a virtual mock-up to automate electricity control through bidirectional person counting (Ultrasonic) and motion detection (PIR). Focused on functional logic rather than physical hardware, the system integrates with Blynk.io to provide real-time cloud analytics and trend visualization. This version enhances the original scope by introducing IoT security simulations, including authentication tokens and login protocols, alongside automated cloud notifications that trigger alerts when system parameters exceed safe limits.

<br>

### **🛠 Prerequisites**

Platform Requirements:
1. Simulation Platform: [Wokwi](https://wokwi.com) (Browser-based ESP32 Simulation).
2. Blynk IoT: Cloud platform for remote control and data visualization (blynk.io).

<br>

Hardware Components (Simulated):
1. ESP32 Dev Board
2. Ultrasonic HC-SR04
3. PIR Motion Sensor
4. Relay Module (1-Ch)
5. LED Bulb & Fitting
5. Breadboard & Jumper Wires



<br>

### 🛠 **How to Run the Simulation**

1. Hardware Configuration Review the wiring logic and component assembly as detailed in the `Technical Documentation.pdf`.

2. Access the Wokwi Simulation, Open the virtual circuit via the following link to view the ESP32 setup and source code:  
   ```
   https://wokwi.com/projects/453664707532384257
    ```

3. Configure Blynk Credentials
    In the 'Wokwi_sketch.ino` tab, locate and replace the placeholder with your unique token from the Blynk dashboard:

    ```cpp
    #define BLYNK_AUTH_TOKEN "Your_Blynk_Auth_Token"
    ```

4. Execute Simulation Simply click the "Start Simulation"  (Play) button in Wokwi