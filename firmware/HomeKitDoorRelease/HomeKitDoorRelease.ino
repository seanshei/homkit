#include "HomeSpan.h"
#include "esp_timer.h"

static const uint8_t RELAY_PIN = D0;
static const bool RELAY_ACTIVE_HIGH = true;
static const unsigned long DOOR_PULSE_MS = 300;

#ifndef SERIAL_DIAGNOSTICS
#define SERIAL_DIAGNOSTICS 0
#endif

#ifndef SERIAL_PORT_ENABLED
#define SERIAL_PORT_ENABLED 1
#endif

static constexpr bool kSerialDiagnostics = SERIAL_DIAGNOSTICS;
static constexpr bool kSerialPortEnabled = SERIAL_PORT_ENABLED;

static void setRelay(bool on) {
  digitalWrite(RELAY_PIN, on == RELAY_ACTIVE_HIGH ? HIGH : LOW);
}

static void configureRelayOutputOff() {
  pinMode(RELAY_PIN, OUTPUT);
  setRelay(false);
}

static void logLine(const char *message) {
  if (kSerialDiagnostics && Serial) {
    Serial.println(message);
  }
}

static void logPulseIgnored(unsigned long now, unsigned long startedAt) {
  if (kSerialDiagnostics && Serial) {
    Serial.print("[");
    Serial.print(now);
    Serial.print(" ms] Ignored: pulse already in progress, elapsed=");
    Serial.print(now - startedAt);
    Serial.println(" ms");
  }
}

static void logPulseStarted(unsigned long now, const char *source) {
  if (kSerialDiagnostics && Serial) {
    Serial.print("[");
    Serial.print(now);
    Serial.print(" ms] Door release pulse started by ");
    Serial.println(source);
  }
}

static void logTimedRelayOff(unsigned long durationMs) {
  if (kSerialDiagnostics && Serial) {
    Serial.print("[");
    Serial.print(millis());
    Serial.print(" ms] Door release relay forced off after ");
    Serial.print(durationMs);
    Serial.println(" ms");
  }
}

struct DoorReleaseSwitch : Service::Switch {
  SpanCharacteristic *power;
  esp_timer_handle_t releaseTimer = nullptr;
  volatile bool pulseInProgress = false;
  volatile bool homeKitResetRequested = false;
  volatile bool pulseFinishedByTimer = false;
  volatile unsigned long pulseStartedAt = 0;
  volatile unsigned long lastPulseDurationMs = 0;

  DoorReleaseSwitch() : Service::Switch() {
    power = new Characteristic::On(false);
    configureRelayOutputOff();

    esp_timer_create_args_t timerArgs = {};
    timerArgs.callback = &DoorReleaseSwitch::releaseTimerCallback;
    timerArgs.arg = this;
    timerArgs.name = "door_release";
    esp_err_t err = esp_timer_create(&timerArgs, &releaseTimer);
    if (err != ESP_OK) {
      logLine("Failed to create relay cutoff timer");
    }
  }

  bool startPulse(const char *source) {
    unsigned long now = millis();

    if (pulseInProgress) {
      logPulseIgnored(now, pulseStartedAt);
      homeKitResetRequested = true;
      return false;
    }

    if (releaseTimer == nullptr) {
      logLine("Ignored: relay cutoff timer is unavailable");
      homeKitResetRequested = true;
      return false;
    }

    pulseInProgress = true;
    homeKitResetRequested = false;
    pulseFinishedByTimer = false;
    lastPulseDurationMs = 0;
    pulseStartedAt = now;
    setRelay(true);

    esp_timer_stop(releaseTimer);
    esp_err_t err = esp_timer_start_once(releaseTimer, DOOR_PULSE_MS * 1000ULL);
    if (err != ESP_OK) {
      setRelay(false);
      pulseInProgress = false;
      homeKitResetRequested = true;
      logLine("Failed to start relay cutoff timer");
      return false;
    }

    logPulseStarted(now, source);
    return true;
  }

  boolean update() override {
    if (!power->getNewVal()) {
      if (pulseInProgress) {
        logLine("HomeKit OFF received during pulse; relay remains timer-controlled");
        homeKitResetRequested = true;
        return true;
      }

      if (releaseTimer != nullptr) {
        esp_timer_stop(releaseTimer);
      }
      setRelay(false);
      pulseInProgress = false;
      homeKitResetRequested = false;
      pulseFinishedByTimer = false;
      logLine("Door release switch set off");
      return true;
    }

    startPulse("HomeKit");
    return true;
  }

  void loop() override {
    if (pulseFinishedByTimer) {
      logTimedRelayOff(lastPulseDurationMs);
      pulseFinishedByTimer = false;
    }

    if (homeKitResetRequested && !pulseInProgress) {
      power->setVal(false);
      homeKitResetRequested = false;
      logLine("HomeKit switch reset off");
    }
  }

  static void releaseTimerCallback(void *arg) {
    DoorReleaseSwitch *self = static_cast<DoorReleaseSwitch *>(arg);
    if (self != nullptr) {
      self->forceRelayOff();
    }
  }

  void forceRelayOff() {
    setRelay(false);
    lastPulseDurationMs = millis() - pulseStartedAt;
    pulseInProgress = false;
    homeKitResetRequested = true;
    pulseFinishedByTimer = true;
  }
};

static DoorReleaseSwitch *doorRelease = nullptr;

void setup() {
  configureRelayOutputOff();

  if (kSerialPortEnabled) {
    Serial.begin(115200);
  }

  homeSpan.setLogLevel(kSerialDiagnostics ? 1 : -1);
  homeSpan.setSerialInputDisable(!kSerialDiagnostics);
  homeSpan.setSketchVersion("0.1.0");
  homeSpan.begin(Category::Switches, "Momentary Relay", "MomentaryRelay", "XIAO ESP32-C6 Grove Relay");

  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Name("Momentary Relay");
      new Characteristic::Manufacturer("DIY");
      new Characteristic::SerialNumber("XIAO-C6-RELAY-001");
      new Characteristic::Model("XIAO ESP32-C6 Grove Relay");
      new Characteristic::FirmwareRevision("0.1.0");
    doorRelease = new DoorReleaseSwitch();

  if (kSerialDiagnostics) {
    logLine("HomeKitDoorRelease ready");
    logLine("Use HomeSpan CLI command W to set Wi-Fi credentials.");
    logLine("Use HomeSpan CLI command S <8-digit-code> to change the HomeKit setup code.");
    logLine("Local fallback: send @P then Enter to pulse the relay once.");

    new SpanUserCommand('P', "Pulse relay once", [](const char *) {
      if (doorRelease != nullptr) {
        doorRelease->startPulse("Serial @P");
      }
    });
  }
}

void loop() {
  homeSpan.poll();
}
