#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <IRremote.hpp> 
#include <BleKeyboard.h>


const char* ssid = "MEO-99C9A0";       
const char* password = "33d79a7da1";   

const int PIN_EMISSOR = 26; 
BleKeyboard bleKeyboard("Xiaomi ESP32", "Xiaomi", 100);
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
  if (comando == "OK")         return 0xE01FF708; 
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
  if (comando == "SWAP") return 0xAF50F708 ; 
  return 0; 
}

void enviarComandoBluetooth(String comando) {
  if(!bleKeyboard.isConnected()) return;

if (comando == "TV_UP")         bleKeyboard.write(KEY_UP_ARROW);
  else if (comando == "TV_DOWN")  bleKeyboard.write(KEY_DOWN_ARROW);
  else if (comando == "TV_LEFT")  bleKeyboard.write(KEY_LEFT_ARROW);
  else if (comando == "TV_RIGHT") bleKeyboard.write(KEY_RIGHT_ARROW);
  else if (comando == "TV_OK")    bleKeyboard.write(KEY_RETURN);
  else if (comando == "TV_BACK")  bleKeyboard.write(KEY_ESC);
  else if (comando == "TV_HOME")  bleKeyboard.write(KEY_MEDIA_WWW_HOME); 
  
  else if (comando == "TV_VOL_UP")   bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
  else if (comando == "TV_VOL_DOWN") bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
  else if (comando == "TV_MUTE")     bleKeyboard.write(KEY_MEDIA_MUTE);

  else if (comando == "TV_SOURCE")   bleKeyboard.write(KEY_F2); 
  else if (comando == "TV_SETTINGS") bleKeyboard.write(KEY_F4); 
  else if (comando == "TV_YOUTUBE")  bleKeyboard.write(KEY_MEDIA_WWW_SEARCH);

  else if (comando == "TV_0") bleKeyboard.write('0');
  else if (comando == "TV_1") bleKeyboard.write('1');
  else if (comando == "TV_2") bleKeyboard.write('2');
  else if (comando == "TV_3") bleKeyboard.write('3');
  else if (comando == "TV_4") bleKeyboard.write('4');
  else if (comando == "TV_5") bleKeyboard.write('5');
  else if (comando == "TV_6") bleKeyboard.write('6');
  else if (comando == "TV_7") bleKeyboard.write('7');
  else if (comando == "TV_8") bleKeyboard.write('8');
  else if (comando == "TV_9") bleKeyboard.write('9');
}

void handleIR() {
  if (server.hasArg("cmd")) {
    String comandoNome = server.arg("cmd");
    Serial.print("Comando recebido: ");
    Serial.println(comandoNome);
    if (comandoNome.startsWith("TV_")) {
        Serial.println(" -> Encaminhado para Bluetooth");
        enviarComandoBluetooth(comandoNome);
    }
    else {
        uint32_t codigoHex = obterCodigoMEO(comandoNome);
        if (codigoHex != 0) {
            Serial.printf(" -> Encaminhado para IR (0x%08X)\n", codigoHex);
            IrSender.sendNECRaw(codigoHex, 0);
        } else {
            Serial.println(" -> Comando IR desconhecido/vazio");
        }
    }
      
    server.send(200, "text/plain", "OK");
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
bleKeyboard.begin(); 
  Serial.println("Bluetooth Iniciado! Podes emparelhar.");
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