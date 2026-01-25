#include "ApiHandler.h"
#include <ArduinoJson.h>
#include <WiFi.h>

// Variáveis Privadas (O "Cofre" das estatísticas)
int totalComandos = 0;
String ultimaAcao = "Nenhum";
unsigned long tempoInicio = 0;

void iniciarMetricas() {
    tempoInicio = millis();
}

void registarAcao(String nomeAcao) {
    totalComandos++;
    ultimaAcao = nomeAcao;
}

String getStatsJSON() {
    JsonDocument doc;
    doc["uptime_segundos"] = (millis() - tempoInicio) / 1000;
    doc["sinal_wifi"] = WiFi.RSSI();
    doc["total_comandos"] = totalComandos;
    doc["ultima_acao"] = ultimaAcao;
    
    String output;
    serializeJson(doc, output);
    return output;
}