#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

const char* ssid = "MEO-99C9A0";
const char* password = "33d79a7da1";



WebServer server(80);
int total_comandos = 0;
String ultima_acao = "Nenhuma";
unsigned long boot_time = 0;


// ALGUMAS FUNCOES PARA TRATAR AS STATS E SERVIR COMO UMA MINI API 

String getUptimeString() {
  unsigned long agora = millis();
  unsigned long segundos_totais = (agora - boot_time) / 1000;
  
  int horas = segundos_totais / 3600;
  int restos = segundos_totais % 3600;
  int minutos = restos / 60;
  int segundos = restos % 60;

  char buffer[20];
  sprintf(buffer, "%02dh %02dm %02ds", horas, minutos, segundos);
  return String(buffer);
}



// funcao que funciona homageneamente com o sistema LittleFS para o ESP devolver o conteudo correto para o bom funcionamento da web page
String getContentType(String filename) {
  if (server.hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  return "text/plain";
}



// o que responde ao request do cliente (pagina web ) e trata tambem de algumas stats 
void handleIR() {
  if (server.hasArg("cmd")) {
    String comando = server.arg("cmd");
    total_comandos++;
    ultima_acao = comando;
    Serial.println(comando);
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Erro");
  }
}



// junta as stats numa única string e envia a para o servidor 
void handleStats() {
  String json = "{";
  json += "\"uptime_segundos\": \"" + getUptimeString() + "\",";
  json += "\"sinal_wifi\": \"" + String(WiFi.RSSI()) + " dBm\",";
  json += "\"memoria_livre\": \"" + String(ESP.getFreeHeap() / 1024) + " KB\",";
  json += "\"ip_local\": \"" + WiFi.localIP().toString() + "\","; 
  json += "\"total_comandos\": " + String(total_comandos) + ",";
  json += "\"ultima_acao\": \"" + ultima_acao + "\"";
  json += "}";

  server.send(200, "application/json", json);
}



// diz para onde o cliente quer ir e devolve o ficheiro correcto do sistema de ficheiros LittleFS
bool handleFileRead(String path) {
  if (path.endsWith("/")) path += "index.html";
  
  if (LittleFS.exists(path)) {
    File file = LittleFS.open(path, "r");
    server.streamFile(file, getContentType(path));
    file.close();
    return true;
  }
  return false;
}




void setup() {
  Serial.begin(115200);
  boot_time = millis();

  if (!LittleFS.begin()) {
    LittleFS.format();
    LittleFS.begin();
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println(WiFi.localIP());

  // se for recebido um pedido get para uma destas rotas , executamos a funcao correspondente 
  server.on("/api/ir", HTTP_GET, handleIR);
  server.on("/api/stats", HTTP_GET, handleStats);
  
  server.onNotFound([]() {
    if (!handleFileRead(server.uri())) {
      server.send(404, "text/plain", "404");
    }
  });

  server.begin();
}







void loop() {
  server.handleClient();
}