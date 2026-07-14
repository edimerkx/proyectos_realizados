#include <LiquidCrystal.h>

// Definición de pines
const int botonInicio = A1;
const int botonParada = A0;
const int botonAumentarVelocidad = A3;
const int botonDisminuirVelocidad = A4;
const int sensorParada = 8;
const int releMotor = 10;     //9-10
const int potenciometroVelocidad = A6;

// Configuración del LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Variables de estado
bool motorEncendido = false;
int velocidad = 0;
const int velocidadMaxima = 100;
const int velocidadMinima = 0;
int ultimaVelocidad = -1;
bool botonInicioPresionado = false;
bool botonParadaPresionado = false;

void setup() {
    Serial.begin(9600);
    configurarPines();
    lcd.begin(16, 4);
    lcd.print("Sistema Iniciado");
    Serial.println("Sistema Motor Iniciado");
    delay(1000);
}

void loop() {
    gestionarMotor();
    actualizarVelocidad();
    actualizarDisplay();
    delay(100);
}

void configurarPines() {
    pinMode(botonInicio, INPUT_PULLUP);
    pinMode(botonParada, INPUT_PULLUP);
    pinMode(botonAumentarVelocidad, INPUT_PULLUP);
    pinMode(botonDisminuirVelocidad, INPUT_PULLUP);
    pinMode(sensorParada, INPUT_PULLUP);
    pinMode(releMotor, OUTPUT);
    digitalWrite(releMotor, HIGH);
}

void gestionarMotor() {
    bool inicioPresionado = digitalRead(botonInicio) == LOW;
    bool paradaPresionada = digitalRead(botonParada) == LOW;
    bool sensorActivado = digitalRead(sensorParada) == LOW;
    
    if (inicioPresionado && !botonInicioPresionado) {
        botonInicioPresionado = true;
        encenderMotor();
    } else if (!inicioPresionado) {
        botonInicioPresionado = false;
    }
    
    if ((sensorActivado || paradaPresionada) && !botonParadaPresionado) {
        botonParadaPresionado = true;
        apagarMotor();
    } else if (!paradaPresionada) {
        botonParadaPresionado = false;
    }
}

void encenderMotor() {
    if (!motorEncendido) {
        motorEncendido = true;
        digitalWrite(releMotor, LOW);
        Serial.println("Motor ON");
    }
}

void apagarMotor() {
    if (motorEncendido) {
        motorEncendido = false;
        digitalWrite(releMotor, HIGH);
        Serial.println("Motor OFF");
    }
}

void actualizarVelocidad() {
    int valorPotenciometro = analogRead(potenciometroVelocidad);
    velocidad = map(valorPotenciometro, 170, 0, velocidadMinima, velocidadMaxima);
    
    if (abs(velocidad - ultimaVelocidad) > 2) {
        Serial.print("Velocidad: ");
        Serial.print(velocidad);
        Serial.println("% *");
        ultimaVelocidad = velocidad;
    }
}

void actualizarDisplay() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Estado: ");
    lcd.print(motorEncendido ? "ON" : "OFF");
    lcd.setCursor(0, 1);
    lcd.print("Velocidad: ");
    lcd.print(velocidad);
    lcd.print("%");
    lcd.setCursor(7, 4);
     lcd.print("UIDET IEA");
}
