# SafeZone — Automatic Vehicle Speed Control in Emergency Zones

A student prototype that combines **GPS geofencing + RFID confirmation** to demonstrate automatic speed limiting on a small DC-motor model vehicle. It is intended for SIH/EEE learning and road-safety research.

> **Safety:** This firmware is for a low-voltage tabletop/model vehicle only. Do not connect it to a real vehicle's throttle, ECU, CAN bus, or braking system. Production vehicle speed intervention requires automotive-grade hardware, validation, fail-safe design, and qualified engineering review. The demo does not guarantee real-world safety or zone accuracy.

## How it works
1. The NEO-6M GPS reports location.
2. A configurable geofence warns that a zone is nearby.
3. An MFRC522 RFID reader confirms a permitted zone-entry tag.
4. The ESP32 applies a reduced PWM command to a DC motor through a suitable motor driver.
5. The display/serial monitor reports the mode. A zone-exit RFID tag returns the model to normal mode.

GPS is advisory: its accuracy varies with sky view and environment. RFID confirmation is required before the prototype enters speed-limited mode.

## Repository
- `firmware/SafeZone_ESP32.ino` — ESP32 demonstration firmware
- `docs/system-design.md` — architecture, wiring notes, and test plan
- `dashboard/index.html` — simple local demo dashboard (simulated data; not connected to hardware)

## Hardware (prototype)
- ESP32 development board
- NEO-6M GPS module (UART)
- MFRC522 RFID reader and entry/exit tags
- Low-voltage DC motor + compatible motor driver (do not drive a motor directly from GPIO)
- Common ground, appropriate power supply, optional buzzer/LED
- Model vehicle chassis

**Logic levels:** MFRC522 is a 3.3 V device. Verify every module's voltage and pin compatibility before wiring. Motor power must be separate from the ESP32 supply where appropriate, with a common ground.

## Pin mapping (edit to match your board)
| Module | ESP32 pin |
|---|---|
| GPS TX → ESP32 RX2 | GPIO 16 |
| GPS RX ← ESP32 TX2 | GPIO 17 (often not needed) |
| RFID SDA/SS | GPIO 5 |
| RFID SCK | GPIO 18 |
| RFID MOSI | GPIO 23 |
| RFID MISO | GPIO 19 |
| RFID RST | GPIO 22 |
| Motor driver PWM/enable | GPIO 25 |
| Buzzer (optional) | GPIO 26 |

Use the Arduino IDE with ESP32 board support and install **TinyGPSPlus** and **MFRC522** libraries. Select your ESP32 board and upload the firmware. Open Serial Monitor at 115200 baud.

## Configure
In the firmware, set the example geofence coordinates/radius and replace the example RFID UIDs with your own tags. UIDs below are placeholders; read your tags using the Serial Monitor. Set entry and exit tags separately.

## Demo limitations
- The dashboard uses sample values only.
- This is not certified vehicle-control software.
- RFID read range is short and depends on antenna/tag placement.
- GPS radius checks are approximate; test outdoors and avoid relying on GPS alone for safety-critical decisions.
- The sample controller ramps PWM gradually and stops the motor command on GPS staleness. Real implementations need a reviewed safe-state strategy.

## Test plan
1. Test GPS readings outdoors with the motor disconnected.
2. Verify entry/exit tag UIDs and geofence status.
3. Test motor driver with wheels lifted and a low-voltage supply.
4. Simulate GPS loss and confirm the status changes to GPS unavailable.
5. Test the complete model at low speed in a clear, enclosed area.
6. Record false positives, detection latency, and speed repeatability.

## License
MIT — see [LICENSE](LICENSE).
