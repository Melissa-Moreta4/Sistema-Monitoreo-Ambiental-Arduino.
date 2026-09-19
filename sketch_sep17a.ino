// ==========================================================
// PROYECTO: Sistema Inteligente de Monitoreo Ambiental
// ASIGNATURA: Sistemas Digitales - UEA
// ==========================================================

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Definición de pines y constantes
#define DHTPIN 2            // Pin digital asignado para el sensor DHT11
#define DHTTYPE DHT11       // Definición del tipo de sensor utilizado
const int PIN_LDR = A0;     // Entrada analógica para medir la iluminación
const int PIN_BUZZER = 3;   // Salida digital asignada al zumbador de alarma
const int LED_VERDE = 4;    // Salida digital para indicador de estado normal
const int LED_ROJO = 5;     // Salida digital para indicador de estado crítico

DHT dht(DHTPIN, DHTTYPE);            // Inicialización del objeto para el sensor DHT
LiquidCrystal_I2C lcd(0x27, 16, 2);   // Configuración de la pantalla LCD con dirección I2C 0x27

void setup() {
  Serial.begin(9600);       // Inicialización de la comunicación serial UART a 9600 baudios
  
  dht.begin();              // Activación de la lectura del sensor DHT11
  lcd.init();               // Inicialización física de la pantalla LCD
  lcd.backlight();          // Encendido de la luz de fondo del display
  
  pinMode(PIN_BUZZER, OUTPUT); // Configuración del pin del buzzer como salida digital
  pinMode(LED_VERDE, OUTPUT);  // Configuración del pin del LED verde como salida digital
  pinMode(LED_ROJO, OUTPUT);   // Configuración del pin del LED rojo como salida digital
  
  // Mensaje de bienvenida inicial en la pantalla
  lcd.setCursor(0, 0);
  lcd.print("SISTEMA MONITOR");
  lcd.setCursor(0, 1);
  lcd.print("AMBIENTAL v2.0");
  delay(2000);              // Pausa de 2 segundos en el inicio
  lcd.clear();              // Limpieza total del display
}

void loop() {
  // Lectura de variables desde los sensores
  float temp = dht.readTemperature();  // Captura del valor de temperatura en Celsius
  float hum = dht.readHumidity();      // Captura del valor de humedad relativa en porcentaje
  int luz = analogRead(PIN_LDR);       // Captura del nivel analógico de iluminación (0 a 1023)

  // Validación básica por si existe falla de conexión en el hardware físico
  if (isnan(temp) || isnan(hum)) {
    Serial.println("ERROR UART: Fallo en la lectura física del sensor DHT11.");
    lcd.setCursor(0, 0);
    lcd.print("ERROR SENSOR    ");
    return;
  }

  // Monitoreo Serial (UART) para depuración en computadora
  Serial.println("--- REGISTRO DE DATOS UART ---");
  Serial.print("Temp: "); Serial.print(temp, 1); Serial.println(" C");
  Serial.print("Hum: "); Serial.print(hum, 1); Serial.println(" %");
  Serial.print("Luz: "); Serial.println(luz);

  // Variable de texto para registrar y transferir el estado actual
  String estado = "";

  // Lógica de Estados y Control
  if (temp >= 15.0 || hum >= 80.0 || luz < 150) { 
    // ESTADO DE ALARMA (Condición Crítica)
    estado = "ALARMA!";
    digitalWrite(LED_ROJO, HIGH);    // Activación visual del indicador de peligro
    digitalWrite(LED_VERDE, LOW);    // Desactivación del indicador normal
    tone(PIN_BUZZER, 1000, 200);     // Generación de tono acústico intermitente
  }
  else if ((temp >= 30.0 && temp < 35.0) || (hum >= 70.0 && hum < 80.0) || (luz >= 150 && luz < 300)) {
    // ESTADO DE ADVERTENCIA (Límites preventivos - Alerta solo por texto en LCD y UART)
    estado = "ADVERTENCIA";
    digitalWrite(LED_ROJO, LOW);     // Apagado de alerta crítica
    digitalWrite(LED_VERDE, LOW);    // Apagado de estado normal
    noTone(PIN_BUZZER);              // Zumbador apagado por prevención
  }
  else {
    // ESTADO NORMAL (Condiciones óptimas de operación)
    estado = "NORMAL";
    digitalWrite(LED_ROJO, LOW);     // Apagado de alerta crítica
    digitalWrite(LED_VERDE, HIGH);   // Activación visual del indicador de seguridad
    noTone(PIN_BUZZER);              // Mantener silenciado el zumbador
  }

  // Transmisión del estado del sistema por el canal UART
  Serial.print("Estado Sistema: "); Serial.println(estado);
  Serial.println("------------------------------\n");

  // Visualización rotativa en pantalla LCD - PANTALLA 1 (Variables Meteorológicas)
  lcd.setCursor(0, 0);
  lcd.print("Temp: "); lcd.print(temp, 1); lcd.print("C   ");
  lcd.setCursor(0, 1);
  lcd.print("Hum:  "); lcd.print(hum, 1); lcd.print("%   ");
  delay(2000); // Retención de pantalla por 2 segundos para lectura cómoda

  // Visualización rotativa en pantalla LCD - PANTALLA 2 (Óptica y Diagnóstico)
  lcd.setCursor(0, 0);
  lcd.print("Luz:  "); lcd.print(luz); lcd.print("      ");
  lcd.setCursor(0, 1);
  lcd.print("Edo:  "); lcd.print(estado); lcd.print("      ");
  delay(2000); // Retención de pantalla por 2 segundos antes de reiniciar el ciclo
}