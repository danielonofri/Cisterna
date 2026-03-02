¡Claro que sí! Un buen proyecto merece una documentación a la altura, especialmente considerando los desafíos técnicos de la distancia del cable y la lógica de seguridad.

Aquí tienes un **README.md** profesional y completo para tu repositorio o carpeta de proyecto.

---

# 💧 Controlador de Bomba Inteligente (NodeMCU)

Este proyecto es un sistema de control de nivel de agua basado en **NodeMCU (ESP8266)**, diseñado para gestionar el llenado de un tanque elevado desde una cisterna. Incluye protecciones críticas de hardware, filtrado de ruido para cables de larga distancia (60m), interfaz web con historial de eventos y sincronización horaria vía **NTP**.

## 🚀 Características (Features)

* **Control Automático Inteligente:** Activación por nivel bajo en tanque y desactivación por tanque lleno o cisterna vacía.
* **Protección del Motor (Timeout X):** Apagado automático si la bomba supera los 5 minutos encendida para evitar sobrecalentamiento o desbordamientos.
* **Protección de Descanso (Inactividad N):** Impide que la bomba se encienda nuevamente antes de 1 minuto de haber parado, protegiendo la vida útil del motor.
* **Filtro de Ruido Digital:** Algoritmo de confirmación de 2 segundos para las lecturas de la boya del tanque, eliminando falsos positivos causados por interferencias en el cable de 60 metros.
* **Interfaz Web Responsiva:** Control manual (On/Off) y visualización de estado en tiempo real.
* **Historial de Eventos (RAM):** Registro de los últimos 40 eventos (20 encendidos/20 apagados) con fecha, hora y causa específica del cambio de estado.
* **Descarga de Datos:** Botón para exportar el historial de eventos directamente a un archivo **CSV**.

---

## 🔌 Esquema de Conexiones

> **⚠️ NOTA DE HARDWARE:** Debido a los 60m de cable hacia el tanque, es **obligatorio** el uso de una resistencia Pull-up externa de $10\text{k}\Omega$ para estabilizar la señal.

| Componente | Pin NodeMCU | Función | Tipo |
| --- | --- | --- | --- |
| **Boya Tanque** | `D5` (GPIO14) | Sensor de Nivel Bajo | Entrada (Pull-up Ext. $10\text{k}\Omega$) |
| **Boya Cisterna** | `D6` (GPIO12) | Seguridad Cisterna | Entrada (Pull-up interno) |
| **Módulo Relay** | `D1` (GPIO5) | Control de Bomba | Salida (Low Level Trigger) |
| **Fuente StepDown** | `VIN` | Alimentación 5V | Entrada de Energía |

---

## 🌐 Descripción de la Interfaz Web

La página principal se sirve directamente desde el NodeMCU y está optimizada para móviles y escritorio:

1. **Panel de Estado:** Muestra si la bomba está actualmente `ENCENDIDA` (verde) o `APAGADA` (rojo).
2. **Controles Manuales:** Botones grandes para forzar el encendido o apagado (el sistema sigue respetando las seguridades de cisterna vacía y timeout incluso en manual).
3. **Acordeón de Historial:** Sección colapsable que organiza una tabla con:
* **Hora:** Obtenida vía NTP (Servidor `pool.ntp.org`).
* **Evento:** Tipo de acción (Encendido/Apagado).
* **Causa:** Por qué ocurrió (Manual, Tanque Bajo, Cisterna Vacía, Timeout).


4. **Botón Exportar:** Genera un archivo `.csv` al instante con todos los registros almacenados en la memoria volátil.

---

## 🛠️ Instalación y Configuración

1. **Librerías necesarias:**
* `ESP8266WiFi` (Integrada)
* `ESP8266WebServer` (Integrada)
* `NTPClient` de Fabrice Weinberg.


2. **Configuración de Red:** Cambia las variables `ssid` y `password` en el código.
3. **Zona Horaria:** El código está configurado para **GMT-3** (Argentina). Si te encuentras en otra zona, modifica la línea:
`NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600);`

---

## ⚠️ Advertencias de Seguridad

* **Aislamiento:** La bomba maneja alta tensión (220V/110V). Mantén el circuito de control (NodeMCU) físicamente separado de los contactos del relay que manejan potencia.
* **Ruido Inducido:** No pases el cable de la boya (3.3V) por el mismo caño/ducto que los cables de alimentación de la bomba (220V) para evitar interferencias electromagnéticas graves.

---

**¿Te gustaría que añada una sección de "Troubleshooting" (solución de problemas) por si el WiFi llega a desconectarse?**