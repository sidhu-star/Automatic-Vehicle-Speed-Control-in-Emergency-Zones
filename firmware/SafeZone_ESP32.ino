/*
  SafeZone: GPS + RFID speed-limiting demonstration
  Target: ESP32 + NEO-6M GPS + MFRC522 + DC motor driver.
  FOR LOW-VOLTAGE MODEL VEHICLE ONLY. Never connect to a real vehicle.

  Libraries: TinyGPSPlus, MFRC522
  Configure the example geofence and tag UIDs before use.
*/

#include <TinyGPSPlus.h>
#include <SPI.h>
#include <MFRC522.h>

static const int GPS_RX_PIN = 16;   // ESP32 RX2 <- GPS TX
static const int GPS_TX_PIN = 17;   // ESP32 TX2 -> GPS RX (optional)
static const int RFID_SS_PIN = 5;
static const int RFID_RST_PIN = 22;
static const int MOTOR_PWM_PIN = 25;
static const int BUZZER_PIN = 26;

static const int PWM_CHANNEL = 0;
static const int PWM_FREQ = 1000;
static const int PWM_RESOLUTION = 8; // duty 0..255
static const int NORMAL_PWM = 190;
static const int ZONE_PWM = 90;
static const int RAMP_STEP = 3;
static const unsigned long GPS_TIMEOUT_MS = 3000;

// Example location only: replace with a surveyed test location.
static const double ZONE_LAT = 17.3850;
static const double ZONE_LON = 78.4867;
static const double ZONE_RADIUS_M = 100.0;

// Replace these placeholder UIDs with your own RFID tag UIDs.
// Example formatting: "A1 B2 C3 D4"
const char *ENTRY_UID = "A1 B2 C3 D4";
const char *EXIT_UID  = "11 22 33 44";

TinyGPSPlus gps;
MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);

bool zoneConfirmed = false;
int currentPwm = 0;
unsigned long lastGpsUpdate = 0;
unsigned long lastPrint = 0;

String uidToString(MFRC522::Uid *uid) {
  String s;
  for (byte i = 0; i < uid->size; i++) {
    if (uid->uidByte[i] < 0x10) s += "0";
    s += String(uid->uidByte[i], HEX);
    if (i + 1 < uid->size) s += " ";
  }
  s.toUpperCase();
  return s;
}

double distanceMeters(double lat1, double lon1, double lat2, double lon2) {
  const double R = 6371000.0;
  const double toRad = 0.017453292519943295;
  double dLat = (lat2 - lat1) * toRad;
  double dLon = (lon2 - lon1) * toRad;
  double a = sin(dLat / 2) * sin(dLat / 2) +
    cos(lat1 * toRad) * cos(lat2 * toRad) *
    sin(dLon / 2) * sin(dLon / 2);
  return R * 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
}

void setMotorPwm(int duty) {
  duty = constrain(duty, 0, 255);
  ledcWrite(PWM_CHANNEL, duty);
}

void scanRfid() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  String uid = uidToString(&rfid.uid);
  Serial.print("RFID UID: ");
  Serial.println(uid);

  if (uid == String(ENTRY_UID)) {
    zoneConfirmed = true;
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("Zone entry confirmed: reduced-speed mode.");
  } else if (uid == String(EXIT_UID)) {
    zoneConfirmed = false;
    Serial.println("Zone exit confirmed: normal mode.");
  } else {
    Serial.println("Unknown tag; mode unchanged.");
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Arduino-ESP32 core 2.x API. On core 3.x, adapt LEDC setup to the
  // installed core's ledcAttach/ledcWrite API.
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(MOTOR_PWM_PIN, PWM_CHANNEL);
  setMotorPwm(0);

  Serial.println("\nSafeZone demo ready.");
  Serial.println("Test on a model vehicle only. Replace geofence and RFID UIDs.");
}

void loop() {
  while (Serial2.available()) {
    if (gps.encode(Serial2.read())) lastGpsUpdate = millis();
  }

  scanRfid();

  bool gpsFresh = gps.location.isValid() &&
                  (millis() - lastGpsUpdate < GPS_TIMEOUT_MS);
  bool nearZone = false;

  if (gpsFresh) {
    double distance = distanceMeters(gps.location.lat(), gps.location.lng(),
                                     ZONE_LAT, ZONE_LON);
    nearZone = distance <= ZONE_RADIUS_M;
  }

  // Geofence is advisory; RFID entry tag confirms the zone.
  // If GPS becomes stale, command motor output to zero for this demo.
  int targetPwm = 0;
  if (gpsFresh) {
    targetPwm = zoneConfirmed ? ZONE_PWM : NORMAL_PWM;
  }

  if (currentPwm < targetPwm) currentPwm = min(currentPwm + RAMP_STEP, targetPwm);
  else if (currentPwm > targetPwm) currentPwm = max(currentPwm - RAMP_STEP, targetPwm);
  setMotorPwm(currentPwm);

  if (millis() - lastPrint > 1000) {
    lastPrint = millis();
    Serial.print("GPS: ");
    Serial.print(gpsFresh ? "OK" : "UNAVAILABLE");
    Serial.print(" | Geofence: ");
    Serial.print(nearZone ? "NEAR/INSIDE" : "OUTSIDE/UNKNOWN");
    Serial.print(" | RFID-confirmed zone: ");
    Serial.print(zoneConfirmed ? "YES" : "NO");
    Serial.print(" | PWM: ");
    Serial.println(currentPwm);
  }

  delay(10);
}
