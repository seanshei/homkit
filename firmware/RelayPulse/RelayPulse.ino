static const uint8_t RELAY_PIN = D0;
static const bool RELAY_ACTIVE_HIGH = true;
static const unsigned long DOOR_PULSE_MS = 300;
static const unsigned long METER_TEST_PULSE_MS = 3000;
static bool pulseInProgress = false;
static unsigned long pulseStartedAt = 0;
static unsigned long activePulseMs = 0;

static void setRelay(bool on) {
  digitalWrite(RELAY_PIN, on == RELAY_ACTIVE_HIGH ? HIGH : LOW);
}

static void startPulse(unsigned long pulseMs, const char *label) {
  if (pulseInProgress) {
    Serial.println("Ignored: pulse already in progress");
    return;
  }

  pulseInProgress = true;
  pulseStartedAt = millis();
  activePulseMs = pulseMs;
  setRelay(true);
  Serial.print("Relay ON: ");
  Serial.println(label);
}

static void finishPulseIfNeeded() {
  if (!pulseInProgress) {
    return;
  }

  if (millis() - pulseStartedAt >= activePulseMs) {
    setRelay(false);
    pulseInProgress = false;
    activePulseMs = 0;
    Serial.println("Relay OFF");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  setRelay(false);

  delay(1000);
  Serial.println("RelayPulse manual mode");
  Serial.print("Relay pin: D0 / GPIO ");
  Serial.println(RELAY_PIN);
  Serial.println("Send 'p' then Enter for a 300 ms pulse.");
  Serial.println("Send 't' then Enter for a 3 second meter-test pulse.");
}

void loop() {
  finishPulseIfNeeded();

  while (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 'p' || command == 'P') {
      startPulse(DOOR_PULSE_MS, "300 ms");
    } else if (command == 't' || command == 'T') {
      startPulse(METER_TEST_PULSE_MS, "3 second meter test");
    }
  }
}
