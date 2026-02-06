#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <IRremote.h>

const char* ssid = "MEO-99C9A0";
const char* password = "33d79a7da1";
const int PIN_EMISSOR = 14; // O fio de Sinal (S) do emissor vai para o D14
const int PIN_RECETOR = 13; // O fio de Sinal do recetor vai para o D13


WebServer server(80);

uint32_t obterCodigoMEO(String comando) {
  
  if (comando == "POWER")      return 0xF50AF708;
  if (comando == "SETTINGS")   return 0x9D62F708; 
  if (comando == "VOICE")      return 0x8B74F708; 

  if (comando == "YOUTUBE")    return 0xC13EF708;
  if (comando == "NETFLIX")    return 0xB14EF708;

  if (comando == "UP")         return 0xFF00F708;
  if (comando == "DOWN")       return 0xFE01F708;
  if (comando == "LEFT")       return 0xFC03F708;
  if (comando == "RIGHT")      return 0xFD02F708;
  if (comando == "OK")         return 0xAF50F708;

  if (comando == "BACK")       return 0xE31CF708;
  if (comando == "HOME")       return 0xF30CF708;
  if (comando == "TV_MODE")    return 0xFA05F708; 

  if (comando == "VOL_UP")     return 0xA758F708;
  if (comando == "VOL_DOWN")   return 0xA25DF708;
  if (comando == "CH_UP")      return 0xA55AF708;
  if (comando == "CH_DOWN")    return 0xA05FF708;
  
  if (comando == "PLAY")       return 0xD22DF708; 
  if (comando == "MUTE")       return 0xD12EF708;

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

  if (comando == "APPS_GRID")  return 0xF708F708; 
  if (comando == "SWAP")       return 0xAF50F708;          

  return 0; 
}

void handleIR() {
  if (server.hasArg("cmd")) {
    String comandoNome = server.arg("cmd");
    Serial.print("Recebi do site: ");
    Serial.println(comandoNome);

    uint32_t codigoHex = obterCodigoMEO(comandoNome);

    if (codigoHex != 0) {
      IrSender.sendNEC(codigoHex, 32);
      Serial.print(" -> Disparado HEX: 0x");
      Serial.println(codigoHex, HEX);
      server.send(200, "text/plain", "OK");
    } else {
      Serial.println(" -> ERRO: Botão sem código definido!");
      server.send(400, "text/plain", "Botao nao configurado");
    }
  } else {
    server.send(400, "text/plain", "Erro nos parametros");
  }
}

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
    server.send(404, "text/plain", "404: Ficheiro Nao Encontrado");
  }
}

void setup() {
  
  Serial.begin(115200);
  
  IrSender.begin(PIN_EMISSOR);
  
  IrReceiver.begin(PIN_RECETOR, ENABLE_LED_FEEDBACK);
  IrSender.begin(PIN_EMISSOR);

  if(!LittleFS.begin()){
    Serial.println("Erro LittleFS");
    return;
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\n--- SISTEMA ONLINE ---");
  Serial.println(WiFi.localIP());

  server.on("/api/ir", handleIR);
  server.onNotFound(handleFileRead);

  server.begin();
}

void loop() {
  server.handleClient();
}