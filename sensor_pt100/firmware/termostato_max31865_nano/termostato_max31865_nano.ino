#include <Adafruit_MAX31865.h>

// Arduino Nano
// MAX31865:
// CLK/SCK -> D13
// SDO/MISO -> D12
// SDI/MOSI -> D11
// CS       -> D10
static const uint8_t PIN_MAX_CS = 10;

// Pulsadores con resistencias externas de 4k.
// Si estan como pull-up externa: pin en HIGH sin presionar y LOW al presionar.
static const uint8_t PIN_BTN_UP = 2;
static const uint8_t PIN_BTN_DOWN = 3;

// Buzzer
static const uint8_t PIN_BUZZER = 4;
// Ponelo en true si tu buzzer suena cuando el pin esta en LOW.
static const bool BUZZER_ACTIVE_LOW = true;

// Para PT100: RNOMINAL 100.0. Para PT1000: RNOMINAL 1000.0.
static const float RNOMINAL = 100.0;

// Revisa el valor de referencia de tu placa MAX31865.
// La mayoria de modulos para PT100 usan 430 ohm.
static const float RREF = 430.0;

static const float SETPOINT_MIN = -50.0;
static const float SETPOINT_MAX = 250.0;
static const float SETPOINT_STEP = 0.5;
static const float HYSTERESIS = 0.5;
static const unsigned long BUTTON_REPEAT_DELAY_MS = 650;
static const unsigned long BUTTON_REPEAT_INTERVAL_MS = 160;

Adafruit_MAX31865 thermo = Adafruit_MAX31865(PIN_MAX_CS);

float setpoint = 30.0;
float lastTemperature = NAN;
bool buzzerOn = false;

unsigned long lastSampleMs = 0;
unsigned long lastSerialMs = 0;

void setBuzzerOutput(bool enabled) {
  if (BUZZER_ACTIVE_LOW) {
    digitalWrite(PIN_BUZZER, enabled ? LOW : HIGH);
  } else {
    digitalWrite(PIN_BUZZER, enabled ? HIGH : LOW);
  }
}

struct ButtonState {
  uint8_t pin;
  bool stablePressed;
  bool lastRawPressed;
  unsigned long changedAt;
  unsigned long pressedAt;
  unsigned long lastRepeatAt;
};

bool buttonPressedEvent(ButtonState &button);

ButtonState btnUp = {PIN_BTN_UP, false, false, 0, 0, 0};
ButtonState btnDown = {PIN_BTN_DOWN, false, false, 0, 0, 0};

bool buttonPressedEvent(ButtonState &button) {
  // Con pull-up externa: presionado = LOW.
  const bool rawPressed = digitalRead(button.pin) == LOW;
  const unsigned long now = millis();

  if (rawPressed != button.lastRawPressed) {
    button.lastRawPressed = rawPressed;
    button.changedAt = now;
  }

  if ((now - button.changedAt) < 35) {
    return false;
  }

  if (rawPressed != button.stablePressed) {
    button.stablePressed = rawPressed;
    if (button.stablePressed) {
      button.pressedAt = now;
      button.lastRepeatAt = now;
      return true;
    }
    return false;
  }

  if (button.stablePressed &&
      (now - button.pressedAt) >= BUTTON_REPEAT_DELAY_MS &&
      (now - button.lastRepeatAt) >= BUTTON_REPEAT_INTERVAL_MS) {
    button.lastRepeatAt = now;
    return true;
  }

  return false;
}

void clampSetpoint() {
  if (setpoint < SETPOINT_MIN) setpoint = SETPOINT_MIN;
  if (setpoint > SETPOINT_MAX) setpoint = SETPOINT_MAX;
}

void sendStatus() {
  Serial.print("TEMP:");
  if (isnan(lastTemperature)) {
    Serial.print("nan");
  } else {
    Serial.print(lastTemperature, 2);
  }
  Serial.print(";SET:");
  Serial.print(setpoint, 2);
  Serial.print(";BUZZ:");
  Serial.println(buzzerOn ? 1 : 0);
}

void handleSerialCommand(String command) {
  command.trim();
  command.toUpperCase();

  if (command == "UP") {
    setpoint += SETPOINT_STEP;
    clampSetpoint();
    sendStatus();
    return;
  }

  if (command == "DOWN") {
    setpoint -= SETPOINT_STEP;
    clampSetpoint();
    sendStatus();
    return;
  }

  if (command.startsWith("SET=")) {
    setpoint = command.substring(4).toFloat();
    clampSetpoint();
    sendStatus();
  }
}

void readSerialCommands() {
  static String buffer;

  while (Serial.available() > 0) {
    const char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (buffer.length() > 0) {
        handleSerialCommand(buffer);
        buffer = "";
      }
    } else if (buffer.length() < 40) {
      buffer += c;
    }
  }
}

void updateBuzzer() {
  if (isnan(lastTemperature)) {
    buzzerOn = false;
    setBuzzerOutput(false);
    return;
  } else if (!buzzerOn && lastTemperature >= setpoint) {
    buzzerOn = true;
  } else if (buzzerOn && lastTemperature <= (setpoint - HYSTERESIS)) {
    buzzerOn = false;
  }

  setBuzzerOutput(buzzerOn);
}

void setup() {
  pinMode(PIN_BTN_UP, INPUT_PULLUP);
  pinMode(PIN_BTN_DOWN, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  setBuzzerOutput(false);

  Serial.begin(115200);
  delay(300);

  // Cambia a MAX31865_2WIRE o MAX31865_4WIRE si el sensor no es de 3 hilos.
  thermo.begin(MAX31865_2WIRE);
  sendStatus();
}

void loop() {
  const unsigned long now = millis();

  readSerialCommands();

  if (buttonPressedEvent(btnUp)) {
    setpoint += SETPOINT_STEP;
    clampSetpoint();
  }

  if (buttonPressedEvent(btnDown)) {
    setpoint -= SETPOINT_STEP;
    clampSetpoint();
  }

  if ((now - lastSampleMs) >= 250) {
    lastSampleMs = now;
    const float measuredTemperature = thermo.temperature(RNOMINAL, RREF);
    const uint8_t fault = thermo.readFault();
    if (fault || isnan(measuredTemperature)) {
      lastTemperature = NAN;
      buzzerOn = false;
      setBuzzerOutput(false);
      thermo.clearFault();
    } else {
      lastTemperature = measuredTemperature;
    }
    updateBuzzer();
  }

  if ((now - lastSerialMs) >= 500) {
    lastSerialMs = now;
    sendStatus();
  }
}
