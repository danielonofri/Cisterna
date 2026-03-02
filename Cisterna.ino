// +----------------+-------------+----------------------+-----------------------------------------------+
// | Componente     | Pin NodeMCU | Función              | Nota Crítica                                  |
// +================+=============+======================+===============================================+
// | Boya Tanque    | D5 (GPIO14) | Entrada Nivel Bajo   | Usar resistencia 10k entre D5 y 3.3V          |
// +----------------+-------------+----------------------+-----------------------------------------------+
// | Boya Cisterna  | D6 (GPIO12) | Seguridad Agua       | Entrada Digital (usar PULLUP interno)         |
// +----------------+-------------+----------------------+-----------------------------------------------+
// | Módulo Relay   | D1 (GPIO5)  | Activar Bomba        | Salida Digital (lógica inversa, activo en LOW)|
// +----------------+-------------+----------------------+-----------------------------------------------+
// | Fuente StepDown| VIN / GND   | Alimentación         | Salida de 5V a VIN (NodeMCU regula a 3.3V)    |
// +----------------+-------------+----------------------+-----------------------------------------------+

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// --- Configuración WiFi ---
const char* ssid = "Ali";
const char* password = "tanguito2";

// --- Configuración de Pines ---
const int pinRelay = 5;    // D1
const int pinTanque = 14;  // D5
const int pinCisterna = 12; // D6

// --- Tiempos de Seguridad (ms) ---
const unsigned long MAX_RUN_TIME = 1800000;  // X: 1/2 hora
const unsigned long MIN_REST_TIME = 600000;  // N: 10 minutos de descanso entre encendidos
const unsigned long FILTRO_RUIDO = 2000;    // 2 segundos para cable largo

// --- Variables de Estado ---
bool bombaEncendida = false;
unsigned long tiempoUltimoCambio = 0;
unsigned long timerFiltroTanque = 0;
bool estadoTanqueConfirmado = false; 

// --- NTP y Logs ---
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600); // GMT-3 Argentina

struct LogEvento {
  String fechaHora;
  String tipo;
  String causa;
};

LogEvento historial[40]; 
int logIndex = 0;

ESP8266WebServer server(80);

void agregarLog(String tipo, String causa) {
  timeClient.update();
  String hora = (WiFi.status() == WL_CONNECTED) ? timeClient.getFormattedTime() : "Sin Hora";
  
  if (logIndex < 40) {
    historial[logIndex] = {hora, tipo, causa};
    logIndex++;
  } else {
    for (int i = 0; i < 39; i++) historial[i] = historial[i + 1];
    historial[39] = {hora, tipo, causa};
  }
}

void actualizarBomba(bool estado, String causa) {
  if (estado == bombaEncendida) return;
  digitalWrite(pinRelay, estado ? LOW : HIGH); // Low Trigger Relay
  bombaEncendida = estado;
  tiempoUltimoCambio = millis();
  agregarLog(estado ? "ENCENDIDO" : "APAGADO", causa);
}

// --- Interfaz Web ---
void handleRoot() {
  String html = "<!DOCTYPE html><html lang='es'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>body{font-family:Arial; text-align:center; background:#f4f4f4;} .card{background:white; margin:20px auto; padding:20px; width:90%; max-width:500px; border-radius:10px; box-shadow:0 4px 8px rgba(0,0,0,0.1);}";
  html += ".btn{padding:15px 25px; margin:10px; border:none; border-radius:5px; cursor:pointer; font-weight:bold;} .on{background:#2ecc71; color:white;} .off{background:#e74c3c; color:white;}";
  html += ".acc{background:#ddd; color:#444; cursor:pointer; padding:15px; width:100%; border:none; text-align:left; outline:none; margin-top:20px;} .panel{display:none; background:white; overflow-x:auto;} table{width:100%; border-collapse:collapse;} td,th{border:1px solid #ddd; padding:8px; font-size:12px;}</style></head><body>";
  
  html += "<div class='card'><h2>Control de Bomba</h2>";
  html += "<h3>Estado: <span style='color:" + String(bombaEncendida ? "#2ecc71" : "#e74c3c") + "'>" + String(bombaEncendida ? "ENCENDIDA" : "APAGADA") + "</span></h3>";
  
  html += "<button class='btn on' onclick=\"location.href='/on'\">ENCENDER</button>";
  html += "<button class='btn off' onclick=\"location.href='/off'\">APAGAR</button>";

  html += "<button class='acc'>+ Ver Historial de Eventos</button>";
  html += "<div class='panel'><table><tr><th>Hora</th><th>Evento</th><th>Causa</th></tr>";
  for (int i = logIndex - 1; i >= 0; i--) {
    html += "<tr><td>" + historial[i].fechaHora + "</td><td>" + historial[i].tipo + "</td><td>" + historial[i].causa + "</td></tr>";
  }
  html += "</table><br><button class='btn' style='background:#3498db; color:white;' onclick='descargarCSV()'>Descargar CSV</button></div></div>";

  html += "<script>document.querySelector('.acc').onclick = function(){ this.classList.toggle('active'); var p = this.nextElementSibling; p.style.display = (p.style.display==='block')?'none':'block'; };";
  html += "function descargarCSV(){ var csv = 'Hora,Evento,Causa\\n'; var rows = document.querySelectorAll('tr'); for(var i=1; i<rows.length; i++){ var cols = rows[i].querySelectorAll('td'); var r = []; for(var j=0; j<cols.length; j++) r.push(cols[j].innerText); csv += r.join(',') + '\\n'; }";
  html += "var b = new Blob([csv],{type:'text/csv'}); var a = document.createElement('a'); a.href = URL.createObjectURL(b); a.download = 'log_bomba.csv'; a.click(); }</script></body></html>";
  
  server.send(200, "text/html", html);
}

void setup() {
  pinMode(pinRelay, OUTPUT);
  digitalWrite(pinRelay, HIGH); // Apagado por defecto
  pinMode(pinTanque, INPUT_PULLUP);
  pinMode(pinCisterna, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  
  timeClient.begin();
  server.on("/", handleRoot);
  server.on("/on", []() { actualizarBomba(true, "Manual Web"); server.send(200, "text/html", "<script>location.href='/';</script>"); });
  server.on("/off", []() { actualizarBomba(false, "Manual Web"); server.send(200, "text/html", "<script>location.href='/';</script>"); });
  server.begin();
}

void loop() {
  server.handleClient();
  unsigned long ahora = millis();

  // 1. FILTRO DE RUIDO (DEBOUNCE) PARA CABLE LARGO
  bool lecturaTanqueActual = (digitalRead(pinTanque) == LOW);
  if (lecturaTanqueActual != estadoTanqueConfirmado) {
    if (ahora - timerFiltroTanque > FILTRO_RUIDO) {
      estadoTanqueConfirmado = lecturaTanqueActual;
    }
  } else {
    timerFiltroTanque = ahora;
  }

  bool cisternaOk = (digitalRead(pinCisterna) == LOW);

  // 2. LÓGICA DE CONTROL AUTOMÁTICO
  if (!bombaEncendida) {
    // Encendido: Tanque bajo confirmado + Cisterna Ok + Tiempo de descanso N cumplido
    if (estadoTanqueConfirmado && cisternaOk && (ahora - tiempoUltimoCambio > MIN_REST_TIME)) {
      actualizarBomba(true, "Automático (Tanque Bajo)");
    }
  } else {
    // Apagado: Tanque lleno (confirmado false) O Cisterna vacía O Tiempo máximo X cumplido
    if (!estadoTanqueConfirmado) {
      actualizarBomba(false, "Tanque Lleno");
    } else if (!cisternaOk) {
      actualizarBomba(false, "Cisterna Vacía");
    } else if (ahora - tiempoUltimoCambio > MAX_RUN_TIME) {
      actualizarBomba(false, "Timeout de Seguridad (X)");
    }
  }
}