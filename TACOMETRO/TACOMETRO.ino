#include <LiquidCrystal.h>

// Configuración de la pantalla LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // Pines RS, E, D4, D5, D6, D7

const int pinSensor = 2; // Pin para la interrupción del sensor
const int pinRele = 3; // Pin para controlar el relé

volatile unsigned long contadorPulsos = 0; // Contador de pulsos
unsigned long tiempoUltimoPulso = 0; // Tiempo del último pulso

unsigned long tiempoUltimaLectura = 0; // Tiempo de la última lectura
unsigned long tiempoInicioEnsayo = 0; // Tiempo de inicio del ensayo
const unsigned long intervaloLectura = 1000; // Intervalo de 1 segundo para calcular RPM

const int pulsosPorVuelta = 1; // Número de rendijas
const unsigned long tiempoAntiRebote = 10; // Tiempo mínimo entre pulsos en milisegundos para evitar rebotes
const unsigned long tiempoDeteccionParada = 2000; // Tiempo sin interrupciones para considerar que el motor se ha detenido (en ms)

// Variables para el filtro de media móvil
const int numeroLecturas = 10;
float lecturas[numeroLecturas];
int indiceLectura = 0;
float totalLecturas = 0;
float promedioRPM = 0;

unsigned long vueltasTotales = 0; // Variable para acumular las vueltas totales

bool motorEnMarcha = false; // Variable para monitorear si el motor ha comenzado a girar
bool releActivo = false; // Variable para mantener el estado del relé

void setup() {
  pinMode(pinSensor, INPUT_PULLUP);
  pinMode(pinRele, OUTPUT); // Configurar el pin del relé como salida
  attachInterrupt(digitalPinToInterrupt(pinSensor), contarPulso, RISING); // Configurar interrupción
  Serial.begin(9600); // Inicializar comunicación serial
  
  // Inicializar la pantalla LCD
  lcd.begin(16, 2); // Configurar la pantalla de 16 columnas y 2 filas
  lcd.print("Iniciando...");

  tiempoInicioEnsayo = millis(); // Guardar el tiempo de inicio del ensayo
  for (int i = 0; i < numeroLecturas; i++) {
    lecturas[i] = 0;
  }
}

void loop() {
  unsigned long tiempoActual = millis(); // Obtener el tiempo actual

  if (tiempoActual - tiempoUltimaLectura >= intervaloLectura) { // Cada 1 segundo
    noInterrupts(); // Deshabilitar interrupciones para leer el contador de manera segura
    unsigned long contadorPulsosTemporal = contadorPulsos;
    contadorPulsos = 0;
    interrupts(); // Habilitar interrupciones nuevamente

    // Calcular RPM
    float vueltas = contadorPulsosTemporal / (float)pulsosPorVuelta;
    float rpm = vueltas * 60.0; // Convertir revoluciones por segundo a RPM

    // Acumular las vueltas totales
    vueltasTotales += (unsigned long)vueltas;

    // Agregar la lectura actual al total y actualizar el promedio
    totalLecturas -= lecturas[indiceLectura];
    lecturas[indiceLectura] = rpm;
    totalLecturas += lecturas[indiceLectura];
    indiceLectura = (indiceLectura + 1) % numeroLecturas;
    promedioRPM = totalLecturas / numeroLecturas;

    // Actualizar el estado del motor
    if (promedioRPM > 0) {
      motorEnMarcha = true; // El motor ha comenzado a girar
    }

    // Controlar el relé en función de la falta de interrupciones
    if (motorEnMarcha && (tiempoActual - tiempoUltimoPulso) >= tiempoDeteccionParada && !releActivo) {
      digitalWrite(pinRele, HIGH); // Activar el relé si no hay interrupciones por un tiempo determinado
      motorEnMarcha = false; // Reiniciar el estado del motor
      releActivo = true; // Mantener el relé activado
    }

    // Calcular el tiempo transcurrido desde el inicio
    unsigned long tiempoTranscurrido = tiempoActual - tiempoInicioEnsayo; // Tiempo transcurrido en milisegundos
    float tiempoTranscurridoSegundos = tiempoTranscurrido / 1000.0; // Convertir a segundos

    // Mostrar en la pantalla LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RPM:");
    lcd.print(promedioRPM);
    lcd.setCursor(0, 1);
    lcd.print("Vueltas:");
    lcd.print(vueltasTotales);

    // Imprimir en formato para el Plotter Serial
    Serial.print("Vueltas:");
    Serial.print(vueltas);
    Serial.print(", RPM:");
    Serial.print(promedioRPM);
    Serial.print(", Velocidad:");
    Serial.print(analogRead(A0) * (5.0 / 1023.0) * 100); // Asumiendo un rango de 0 a 100%
    Serial.print(", Vueltas Totales:");
    Serial.print(vueltasTotales); // Imprimir el total acumulado de vueltas
    Serial.print(", Tiempo Transcurrido (s):");
    Serial.println(tiempoTranscurridoSegundos); // Imprimir el tiempo transcurrido

    tiempoUltimaLectura = tiempoActual; // Actualizar el tiempo
  }
}

void contarPulso() {
  unsigned long tiempoActual = millis();
  if (tiempoActual - tiempoUltimoPulso > tiempoAntiRebote) {
    contadorPulsos++;
    tiempoUltimoPulso = tiempoActual;
  }
}

