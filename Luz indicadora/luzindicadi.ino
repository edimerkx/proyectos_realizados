#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Configuración de red WiFi
const char* ssid = "IEA-Wifi";          // Cambia por el nombre de tu red WiFi
const char* password = "uidetIEA372";   // Cambia por la contraseña de tu red

// Pines
#define LED_ROJO_1 D6
#define LED_ROJO_0 D0
#define LED_ROJO_7 D7
#define LED_ROJO_2 D5
#define BOTON D1  // Pin del botón físico

// Servidor web
ESP8266WebServer server(80);

// Estado de los LEDs
bool estadoLedRojo = false;

// Estado anterior del botón
bool botonAnterior = HIGH;

void setup() {
  // Configuración de pines
  pinMode(LED_ROJO_1, OUTPUT);
  pinMode(LED_ROJO_0, OUTPUT);
  pinMode(LED_ROJO_7, OUTPUT);
  pinMode(LED_ROJO_2, OUTPUT);
  pinMode(BOTON, INPUT_PULLUP);  // Usamos INPUT_PULLUP para que el botón esté LOW cuando se presiona

  // Inicializar LEDs
  digitalWrite(LED_ROJO_1, LOW);
  digitalWrite(LED_ROJO_0, LOW);
  digitalWrite(LED_ROJO_7, LOW);
  digitalWrite(LED_ROJO_2, LOW);

  // Conectar al WiFi
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado a WiFi");
  Serial.println(WiFi.localIP()); // Muestra la IP en la consola serie

  // Configuración de rutas del servidor web
  server.on("/", handleRoot);
  server.on("/toggleRojo", toggleLedRojo);  // Ruta para alternar el estado de los LEDs rojos
  server.on("/ip", handleIP);  // Ruta para mostrar la IP actual

  // Iniciar servidor
  server.begin();
  Serial.println("Servidor web iniciado");
}

void loop() {
  // Atender peticiones del servidor web
  server.handleClient();

  // Leer el estado del botón físico y alternar el estado de los LEDs
  bool boton = digitalRead(BOTON) == LOW;  // LOW cuando se presiona

  // Solo cambiar el estado cuando el botón ha sido presionado (debouncing)
  if (boton && botonAnterior == HIGH) {  // Detectamos cuando el botón es presionado
    estadoLedRojo = !estadoLedRojo;
    digitalWrite(LED_ROJO_1, estadoLedRojo ? HIGH : LOW);
    digitalWrite(LED_ROJO_0, estadoLedRojo ? HIGH : LOW);
    digitalWrite(LED_ROJO_7, estadoLedRojo ? HIGH : LOW);
    digitalWrite(LED_ROJO_2, estadoLedRojo ? HIGH : LOW);
  }

  // Actualizar el estado anterior del botón
  botonAnterior = boton;

  delay(50);  // Debouncing
}

// Página principal con un botón para controlar los LEDs rojos
void handleRoot() {
  String html = "<h1>ACCESO A LA SALA</h1>";

  // Botón circular para alternar el estado de los LEDs rojos
  html += "<p><button onclick='toggleLED()' style='"
          "background-color:red;"
          "color:white;"
          "border:none;"
          "border-radius:50%;"
          "width:100px;"
          "height:100px;"
          "font-size:16px;'>"
          "Alternar</button></p>";

  // Mostrar el estado de los LEDs rojos
  if (estadoLedRojo) {
    html += "<p> LUZ ENCENDIDA</p>";
  } else {
    html += "<p> LUZ APAGADA</p>";
  }

  // JavaScript para manejar el botón sin redirigir
  html += "<script>"
          "function toggleLED() {"
          "  var xhr = new XMLHttpRequest();"
          "  xhr.open('GET', '/toggleRojo', true);"
          "  xhr.onload = function() {"
          "    if (xhr.status == 200) {"
          "      location.reload();"
          "    }"
          "  };"
          "  xhr.send();"
          "}"
          "</script>";

  server.send(200, "text/html", html);
}

// Función para alternar el estado de los LEDs rojos
void toggleLedRojo() {
  estadoLedRojo = !estadoLedRojo;
  digitalWrite(LED_ROJO_1, estadoLedRojo ? HIGH : LOW);
  digitalWrite(LED_ROJO_0, estadoLedRojo ? HIGH : LOW);
  digitalWrite(LED_ROJO_7, estadoLedRojo ? HIGH : LOW);
  digitalWrite(LED_ROJO_2, estadoLedRojo ? HIGH : LOW);
  server.send(200, "text/plain", "LEDs Rojos alternados");
}

// Ruta para mostrar la IP actual
void handleIP() {
  String html = "<h1>Dirección IP</h1>";
  html += "<p>La dirección IP actual es: " + WiFi.localIP().toString() + "</p>";
  server.send(200, "text/html", html);
}

