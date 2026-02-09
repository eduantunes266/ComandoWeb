#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <IRremote.hpp> 

const char* ssid = "MEO-99C9A0";       
const char* password = "33d79a7da1";   

const int PIN_EMISSOR = 26; 

WebServer server(80);

uint32_t obterCodigoMEO(String comando) {
  if (comando == "POWER")      return 0xF50AF708;
  if (comando == "SETTINGS")   return 0x9D62F708; 
  if (comando == "VOICE")      return 0x8B74F708; 
  if (comando == "MEO")        return 0xFA05F708; 
  
  if (comando == "YOUTUBE")    return 0xC13EF708;
  if (comando == "NETFLIX")    return 0xB14EF708;
  if (comando == "UP")         return 0xFF00F708; 
  if (comando == "DOWN")       return 0xFE01F708; 
  if (comando == "LEFT")       return 0xFC03F708; 
  if (comando == "RIGHT")      return 0xFD02F708; 
  if (comando == "OK")         return 0xAF50F708; 
  if (comando == "BACK")       return 0xE31CF708;
  if (comando == "HOME")       return 0xF30CF708;
  if (comando == "APPS_GRID")  return 0xF708F708; 

  if (comando == "VOL_UP")     return 0xA758F708; 
  if (comando == "VOL_DOWN")   return 0xA25DF708; 
  if (comando == "MUTE")       return 0xD12EF708;
  if (comando == "CH_UP")      return 0xA55AF708; 
  if (comando == "CH_DOWN")    return 0xA05FF708; 
  if (comando == "PLAY")       return 0xD22DF708; 

  if (comando == "0") return 0xEF10F708;
  if (comando == "1") return 0xEE11F708;
  if (comando == "2") return 0xED12F708;
  if (comando == "3") return 0xEC13F708;
  if (comando == "4") return 0xEB14F708;
  if (comando == "5") return 0xEA15F708;
  if (comando == "6") return 0xE916F708;
  if (comando == "7") return 0xE817F708;
  if (comando == "8") return 0xE718F708;
  if (comando == "9") return 0xE619F708;

  return 0; 
}

void handleIR() {
  if (server.hasArg("cmd")) {
    String comandoNome = server.arg("cmd");
    Serial.print("Comando recebido via Wi-Fi: ");
    Serial.println(comandoNome);

    uint32_t codigoHex = obterCodigoMEO(comandoNome);

    if (codigoHex != 0) {
      Serial.printf("A enviar código HEX: 0x%08X\n", codigoHex);
      
   
      IrSender.sendNECRaw(codigoHex, 0); 
      
      server.send(200, "text/plain", "OK");
    } else {
      Serial.println(" -> Comando não existe na lista.");
      server.send(400, "text/plain", "Erro: Comando desconhecido");
    }
  } else {
    server.send(400, "text/plain", "Falta cmd");
  }
}

// Servir os ficheiros do site (LittleFS)
void handleFileRead() {
  String path = server.uri();
  if (path.endsWith("/")) path += "index.html";
  String contentType = "text/plain";
  if (path.endsWith(".html")) contentType = "text/html";
  else if (path.endsWith(".css")) contentType = "text/css";
  else if (path.endsWith(".js")) contentType = "application/javascript";
  
  if (LittleFS.exists(path)) {
    File file = LittleFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
  } else {
    server.send(404, "text/plain", "404 Not Found");
  }
}

void setup() {
  Serial.begin(115200);
  
  IrSender.begin(PIN_EMISSOR);

  if(!LittleFS.begin()){
    Serial.println("Erro ao montar LittleFS! (Lembra-te de fazer Upload Filesystem Image)");
    return;
  }

  // Ligar ao Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("A ligar ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\n--- LIGADO! ---");
  Serial.print("Acede no telemóvel a: http://");
  Serial.println(WiFi.localIP());

  // Configurar rotas do servidor
  server.on("/api/ir", handleIR); 
  server.onNotFound(handleFileRead); 
  server.begin();
}

void loop() {
  server.handleClient();
}