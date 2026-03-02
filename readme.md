¡Claro que sí! Un buen proyecto merece una documentación a la altura, especialmente considerando los desafíos técnicos de la distancia del cable y la lógica de seguridad.

Aquí tienes un **README.md** profesional y completo para tu repositorio o carpeta de proyecto.

---

# 💧 Controlador de Bomba Inteligente (NodeMCU)

Este proyecto es un sistema de control de nivel de agua basado en **NodeMCU (ESP8266)**, diseñado para gestionar el llenado de un tanque elevado desde una cisterna. Incluye protecciones críticas de hardware, filtrado de ruido para cables de larga distancia (60m), interfaz web con historial de eventos y sincronización horaria vía **NTP**.

## 🚀 Características (Features)

* **Control Automático Inteligente:** Activación por nivel bajo en tanque y desactivación por tanque lleno o cisterna vacía.
* **Protección del Motor (Timeout X):** Apagado automático si la bomba supera los 30 minutos encendida para evitar sobrecalentamiento o desbordamientos.
* **Protección de Descanso (Inactividad N):** Impide que la bomba se encienda nuevamente antes de 10 minutos de haber parado, protegiendo la vida útil del motor.
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

## 🔧 Instalación

### 💡 Tips de Instalación 

1. **Alimentación Separada:** Conecta la fuente Step-Down de **5V al pin VIN** del NodeMCU (no al pin 3.3V directamente), ya que el regulador interno del NodeMCU filtrará mejor cualquier pequeño ruido de la fuente.
2. **El "Pull-up" Físico:** Aunque el código tiene filtro, la resistencia de **10k Ohm** es tu mejor amiga. Instálala lo más cerca posible de la placa NodeMCU, no al final del cable de 60 metros.
3. **Caja de Control:** Si vas a meter todo en una caja estanca, trata de que la antena del NodeMCU (la parte del PCB con trazas en zigzag) no quede pegada a cables de 220V o paredes metálicas para no perder señal WiFi.

### 📋 Checklist de Pruebas

* [ ] **Prueba de Secado:** Simula el tanque bajo desconectando el cable y verifica que el relay actúe tras los 2 segundos de espera.
* [ ] **Prueba de Seguridad (X):** Deja la bomba "encendida" simulada y verifica que a los 30 minutos se apague sola y registre "Timeout" en el historial.
* [ ] **Prueba de Descanso (N):** Intenta encenderla inmediatamente después de un apagado y verifica que ignore la orden hasta que pasen los 10 minutos de cortesía.

Con esto, el sistema queda blindado contra ruidos eléctricos y errores de operación.

---
