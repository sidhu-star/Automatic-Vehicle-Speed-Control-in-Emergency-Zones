# SafeZone System Design

## Goal
Demonstrate a hybrid GPS + RFID method for zone-aware speed limiting on a small electric model vehicle.

## Architecture
GPS receiver (coarse location) → ESP32 geofence check → RFID entry/exit confirmation → gradual PWM command → motor driver → model DC motor.

The GPS geofence indicates proximity to a configured location. RFID tags confirm the physical checkpoint. An optional dashboard can later receive telemetry over Wi-Fi/MQTT; the included HTML dashboard is a standalone mock-up and is not connected to the ESP32.

## State logic
- **GPS unavailable:** prototype motor PWM is commanded to zero.
- **GPS valid + no entry tag:** normal model PWM.
- **GPS valid + entry tag:** reduced model PWM.
- **Exit tag:** normal model PWM resumes.
- **Unknown tag:** no state change.

This is a teaching example, not a safety-certified state machine. RFID state persistence and GPS-loss behavior must be reviewed for any real deployment.

## Wiring notes
- MFRC522 uses SPI and 3.3 V logic. Do not power it from 5 V.
- GPS TX connects to ESP32 RX2 (GPIO 16 in this example).
- Motor driver input/enable accepts PWM on GPIO 25 only if compatible with the chosen driver.
- Never power a motor directly from an ESP32 GPIO.
- Confirm board pinout, motor-driver current rating, supply voltage, and common ground before powering.

## Evaluation metrics
1. GPS fix acquisition time and location drift.
2. RFID tag detection success rate and read latency.
3. Time between tag detection and reduced PWM.
4. Repeatability of speed reduction under the same load.
5. Behavior when GPS signal is lost.
6. False activation rate outside the zone.

## Future improvements
- Secure authenticated zone messages and signed configuration.
- Roadside LoRa/BLE beacon for temporary zones.
- Telemetry over MQTT with explicit connection-loss handling.
- Sensor-based wheel speed measurement and closed-loop control.
- Automotive-grade functional-safety engineering for any real-vehicle application.
