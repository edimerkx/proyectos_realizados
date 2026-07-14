#include <Adafruit_MAX31865.h>

// Pines pensados para NodeMCU / Wemos D1 mini.
// Cambialos si tu placa usa otro pinout.
static const uint8_t PIN_MAX_CS = 15;
static const uint8_t PIN_BTN_UP = 5;
static const uint8_t PIN_BTN_DOWN = 4;
static const uint8_t PIN_BUZZER = 2;

// Para PT100: RNOMINAL 100.0. Para PT1000: RNOMINAL 1000.0.
static const float RNOMINAL = 100.0;
// Revisa el valor impreso en tu placa MAX31865. Muchas placas PT100 usan 430 ohm.
static const float RREF = 430.0;

static const float SETPOINT_MIN = -50.0;
static const float SETPOINT_MAX = 250.0;
static const float SETPOINT_STEP = 0.5;
static const float HYSTERESIS = 0.5;

Adafruit_MAX31865 thermo = Adafruit_MAX31865(PIN_MAX_CS);

float setpoint = 30.0;
float lastTemperature = NAN;
bool buzzerOn = false;

unsigned long lastSampleMs = 0;
unsigned long lastSerialMs = 0;

struct ButtonState {
  uint8_t pin;
  bool stablePressed;
  bool lastRawPressed;
  unsigned long changedAt;
};

ButtonState btnUp = {PIN_BTN_UP, false, false, 0};
ButtonState btnDown = {PIN_BTN_DOWN, false, false, 0};

bool buttonPressedEvent(ButtonState &button) {
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
    return button.stablePressed;
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
  } else if (!buzzerOn && lastTemperature >= setpoint) {
    buzzerOn = true;
  } else if (buzzerOn && lastTemperature <= (setpoint - HYSTERESIS)) {
    buzzerOn = false;
  }

  digitalWrite(PIN_BUZZER, buzzerOn ? HIGH : LOW);
}

void setup() {
  pinMode(PIN_BTN_UP, INPUT_PULLUP);
  pinMode(PIN_BTN_DOWN, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  Serial.begin(115200);
  delay(200);

  thermo.begin(MAX31865_3WIRE); // Cambia a MAX31865_2WIRE o MAX31865_4WIRE si corresponde.
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
    const uint8_t fault = thermo.readFault();
    if (fault) {
      lastTemperature = NAN;
      thermo.clearFault();
    } else {
      lastTemperature = thermo.temperature(RNOMINAL, RREF);
    }
    updateBuzzer();
  }

  if ((now - lastSerialMs) >= 500) {
    lastSerialMs = now;
    sendStatus();
  }
}
