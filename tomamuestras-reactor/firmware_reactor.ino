const int dirPin = 6;       // Pin de dirección
const int stepPin = 7;      // Pin de pulso (paso)
const int PinSensor = 2;    // Caudalímetro 1
const int PinSensor2 = 3;   // Caudalímetro 2
const int enable = 8;       // Pin de habilitación del driver
const int RELE = 4;

const int pinAnalogico1 = A0; // Entrada analógica 1
const int pinAnalogico2 = A1; // Entrada analógica 2

float factor_conversion1 = 7.5;
float factor_conversion2 = 7.5;
bool releActivo = false; // Variable para controlar el estado del relé

volatile unsigned long contadorPulsos1 = 0;
volatile unsigned long contadorPulsos2 = 0;
unsigned long tiempoUltimaMedicion = 0;
float frecuencia1 = 0, frecuencia2 = 0;

int stepDelay = 1000;       // Retardo entre pasos en microsegundos
bool motorEnMarcha = false;
int pasosPorHueco = 200;    // Pasos por hueco (valor por defecto)
unsigned long tiempoDetencion = 1000; // Tiempo de pausa entre huecos (en milisegundos)

void ContarPulsos() {
  contadorPulsos1++;
}

void ContarPulsos2() {
  contadorPulsos2++;
}


void CalcularFrecuencia() {
  unsigned long tiempoActual = millis();
  if (tiempoActual - tiempoUltimaMedicion >= 400) {
    noInterrupts(); // Pausar interrupciones momentáneamente
    frecuencia1 = contadorPulsos1;
    frecuencia2 = contadorPulsos2;
    contadorPulsos1 = 0;
    contadorPulsos2 = 0;
    interrupts(); // Reanudar interrupciones
    tiempoUltimaMedicion = tiempoActual;

    // Leer valores analógicos y convertirlos a voltaje
    int lectura1 = analogRead(pinAnalogico1);
    int lectura2 = analogRead(pinAnalogico2);
    float voltaje1 = (lectura1 / 1023.0) * 3.3;
    float voltaje2 = (lectura2 / 1023.0) * 3.3;

    // Imprimir datos al serial
//    Serial.print(frecuencia1); Serial.print(",");
    Serial.print(frecuencia1,0); Serial.print(",");   //caudal en litros/minuto  
  //  Serial.print(frecuencia2); Serial.print(",");
    Serial.print(frecuencia2,0); Serial.print(",");     //volumen =volumen+(caudal/60)*(dt/10000)
    Serial.print(frecuencia1 / factor_conversion1,1); Serial.print(",");
    Serial.println(frecuencia2 / factor_conversion2,1);//Serial.print(",");
   //Serial.print(frecuencia1,0); Serial.print(",");  
   //Serial.println(frecuencia2,0); 
  }
}

void MoverMotor() {
  static unsigned long ultimaTransicion = 0;  
  static unsigned long inicioPausa = 0;      
  static int pasosRealizados = 0;            
  static bool enPausa = false;               

  if (motorEnMarcha && stepDelay > 0 && !enPausa) {
    if (pasosRealizados < pasosPorHueco) {
      unsigned long tiempoActual = micros();
      if (tiempoActual - ultimaTransicion >= stepDelay) {
        ultimaTransicion = tiempoActual;
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(2); 
        digitalWrite(stepPin, LOW);
        pasosRealizados++;
      }
    } else {
      enPausa = true;
      inicioPausa = millis();
      pasosRealizados = 0;
    }
  } else if (enPausa) {
    if (millis() - inicioPausa >= tiempoDetencion) {
      enPausa = false; 
    }
  }
}

void ProcesarComando() {
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();

    if (comando == "RELE_ON") {
      releActivo = true;
      digitalWrite(RELE, LOW); 
      delay(2000);
      releActivo = false; 
      digitalWrite(RELE, HIGH);
    } else if (comando == "RELE_OFF") {
      releActivo = false; 
      //digitalWrite(RELE, HIGH);
    } else if (comando.startsWith("SPEED:")) {
      int nuevaVelocidad = comando.substring(6).toInt();
      stepDelay = max(1, nuevaVelocidad); 
    } else if (comando == "START") {
      motorEnMarcha = true;
    } else if (comando == "STOP") {
      motorEnMarcha = false;
    } else if (comando == "DIR") {
      digitalWrite(dirPin, !digitalRead(dirPin));
    } else if (comando.startsWith("Ka:")) {
      float nuevoFactor1 = comando.substring(3).toFloat();
      factor_conversion1 = max(5, nuevoFactor1); 
      Serial.println("factor actualizado a :" + String(factor_conversion1));
    } else if (comando.startsWith("Kb:")) {
      float nuevoFactor2 = comando.substring(3).toFloat();
      factor_conversion2 = max(5, nuevoFactor2); 
       Serial.print("Nuevo K2: ");
      Serial.println("factor actualizado a :" + String(factor_conversion1));
    } else if (comando.startsWith("PAUSE:")) {
      int nuevoTiempo = comando.substring(6).toInt();
      tiempoDetencion = max(0, nuevoTiempo);
    }
  }
}

void setup() {
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(enable, OUTPUT);
  pinMode(RELE, OUTPUT);
  pinMode(pinAnalogico1, INPUT);
  pinMode(pinAnalogico2, INPUT);

  digitalWrite(RELE, HIGH);
  digitalWrite(enable, LOW);
  Serial.begin(9600);

  pinMode(PinSensor, INPUT);
  pinMode(PinSensor2, INPUT);
  attachInterrupt(digitalPinToInterrupt(PinSensor), ContarPulsos, RISING);
  attachInterrupt(digitalPinToInterrupt(PinSensor2), ContarPulsos2, RISING);
}

void loop() {
  ProcesarComando();
  CalcularFrecuencia();
  MoverMotor();
}
