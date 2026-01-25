#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

// --- CONFIGURAÇÕES DE WI-FI ---
const char* ssid = "NOME_DA_TUA_NET";
const char* password = "PASSWORD_DA_NET";

// Iniciar o servidor na porta 80
WebServer server(80);

// --- FUNÇÃO AJUDANTE: Descobre o tipo de ficheiro (MIME Type) ---
// Isto é importante para o browser saber se está a receber HTML ou CSS
String getContentType(String filename) {
  if (server.hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

// --- O MOTOR INTELIGENTE: Lê ficheiros da memória ---
bool handleFileRead(String path) {
  // Se o pedido for a raiz "/", envia o index.html
  if (path.endsWith("/")) path += "index.html";

  // Verifica se o ficheiro existe na memória LittleFS
  if (LittleFS.exists(path)) {
    // Abre o ficheiro
    File file = LittleFS.open(path, "r");
    // Envia para o browser com o tipo correto (HTML ou CSS)
    size_t sent = server.streamFile(file, getContentType(path));
    file.close();
    return true;
  }
  return false; // Ficheiro não encontrado
}

void setup() {
  Serial.begin(115200);

  // 1. Iniciar o Sistema de Ficheiros
  if (!LittleFS.begin()) {
    Serial.println("Erro ao montar LittleFS! A formatar...");
    LittleFS.format(); // Formata se for a primeira vez
    LittleFS.begin();
  }
  Serial.println("LittleFS iniciado.");

  // 2. Ligar ao Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("A ligar");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Ligado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // 3. Configurar a API (Amanha tratamos disto a fundo)
  server.on("/api/ir", HTTP_GET, []() {
    String cmd = server.arg("cmd");
    Serial.println("Recebido comando API: " + cmd);
    // Amanhã metemos aqui o código dos infravermelhos!
    server.send(200, "text/plain", "Comando " + cmd + " recebido");
  });

  // 4. Configurar o "apanha-tudo" para os ficheiros HTML/CSS
  // Se não for API, o ESP tenta ler um ficheiro com esse nome
  server.onNotFound([]() {
    if (!handleFileRead(server.uri())) {
      server.send(404, "text/plain", "404: Ficheiro nao encontrado");
    }
  });

  server.begin();
  Serial.println("Servidor Web Iniciado.");
}

void loop() {
  server.handleClient();
}