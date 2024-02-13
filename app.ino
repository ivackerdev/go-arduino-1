#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <FS.h> // Para SPIFFS
#include <time.h> // Para obtener la fecha y hora
#include <ESP8266WebServer.h>

#define SERVER_IP "127.0.0.1" // Asegúrate de que esta URL apunta a la imagen que deseas descargar

#ifndef STASSID
#define STASSID "NombreRedWifi"
#define STAPSK "ClaveWifi"
#endif

ESP8266WebServer server(80); // Servidor web en el puerto 80
bool continueLoop = true; // Controla la ejecución del loop

void handleFileList() {
  String output = "<html><body><meta name=""viewport"" content=""width=device-width, initial-scale=1.0""><h2>Archivos almacenados</h2><ul>";

  Dir dir = SPIFFS.openDir("/"); // Abre el directorio raíz
  while (dir.next()) { // Lista todos los archivos
    String fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
    output += "<li><a href=\"" + fileName + "\">" + fileName + "</a> - " + String(fileSize) + " bytes</li>";
  }
  output += "</ul>";

      // Botón para listar los archivos
  output += "<form action='/list' method='GET'><button type='submit'>Listar Archivos</button></form>";
  // Botón para eliminar todos los archivos
  output += "<form action='/delete' method='GET'><button type='submit'>Eliminar Archivos</button></form>";
  // Botón para mostrar la información de almacenamiento
  output += "<form action='/storage' method='GET'><button type='submit'>Almacenamiento</button></form>";
  // Botón para detener el loop
  output += "<form action='/stop' method='GET'><button type='submit'>Detener</button></form>";
    output += "</body></html>";
  
  server.send(200, "text/html", output); // Envía la lista de archivos como una página HTML
}

void handleDeleteAllFiles() {
  Dir dir = SPIFFS.openDir("/"); // Abre el directorio raíz
  int count = 0; // Contador para los archivos eliminados
  
  while (dir.next()) { // Recorre todos los archivos
    SPIFFS.remove(dir.fileName()); // Elimina el archivo
    count++;
  }
  
  String message = "Archivos eliminados: " + String(count);
  server.send(200, "text/plain", message); // Envía la respuesta
}

void handleStorageInfo() {
  FSInfo fs_info;
  SPIFFS.info(fs_info);

  String output = "<html><body><meta name=""viewport"" content=""width=device-width, initial-scale=1.0""><h2>Info Almacenamiento</h2>";

  output += "<p>Total: " + String(fs_info.totalBytes) + " bytes</p>";
  output += "<p>Usado: " + String(fs_info.usedBytes) + " bytes</p>";
  output += "<p>Libre: " + String(fs_info.totalBytes - fs_info.usedBytes) + " bytes</p>";
  output += "</br>";
  
    // Botón para listar los archivos
  output += "<form action='/list' method='GET'><button type='submit'>Listar Archivos</button></form>";
  // Botón para eliminar todos los archivos
  output += "<form action='/delete' method='GET'><button type='submit'>Eliminar Archivos</button></form>";
  // Botón para mostrar la información de almacenamiento
  output += "<form action='/storage' method='GET'><button type='submit'>Almacenamiento</button></form>";
  // Botón para detener el loop
  output += "<form action='/stop' method='GET'><button type='submit'>Detener</button></form>";
  
  output += "</body></html>";
  output += "</body></html>";

  server.send(200, "text/html", output);
}

void handleStopLoop() {
  continueLoop = false; // Detiene el loop
  server.send(200, "text/plain", "El loop se detendrá después de esta ejecución.");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  if (!SPIFFS.begin()) {
    Serial.println("Error al montar el sistema de archivos");
    return;
  }

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Sincronizando hora...");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Hora sincronizada");

  // Configuración del servidor web
  server.on("/list", HTTP_GET, handleFileList); // Ruta para listar los archivos
  server.on("/delete", HTTP_GET, handleDeleteAllFiles); // Ruta para borrar todos los archivos
  server.on("/storage", HTTP_GET, handleStorageInfo); // Ruta para información de almacenamiento
  server.on("/stop", HTTP_GET, handleStopLoop); // Ruta para detener el loop
  server.begin(); // Inicia el servidor web
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  if (continueLoop) {
    server.handleClient(); // Procesa las peticiones al servidor web

    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;

      Serial.print("[HTTP] begin...\n");
      http.begin(client, "http://" SERVER_IP "/p.gif"); // HTTP GET

      Serial.print("[HTTP] GET...\n");
      int httpCode = http.GET();

      if (httpCode > 0) {
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          Serial.println("[HTTP] Descargando imagen...");

          time_t now = time(nullptr);
          struct tm *timeinfo = localtime(&now);
          char filename[64];
          strftime(filename, sizeof(filename), "/image_%Y-%m-%d_%H-%M-%S.jpg", timeinfo);
          File file = SPIFFS.open(filename, "w");
          if (!file) {
            Serial.println("Error al abrir el archivo para escritura");
            return;
          }

          int length = 0;
          const size_t bufferSize = 2048;
          byte buffer[bufferSize];

          while(http.connected() && (length = http.getStream().available()) > 0) {
            size_t len = std::min((int)bufferSize, length);
            size_t n = http.getStream().readBytes(buffer, len);
            size_t written = file.write(buffer, n);
            if(written != n) {
              Serial.println("Error al escribir el archivo");
              break;
            }
          }

          file.close();
          Serial.println("Imagen guardada");
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end(); //Cierra la conexión
    }

    delay(5000); // Espera antes de la próxima petición
  }
  // No se necesita un bloque else adicional si simplemente queremos detener el loop.
}
