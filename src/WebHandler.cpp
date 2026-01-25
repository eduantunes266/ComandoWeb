#include "WebHandler.h"
#include "HtmlPage.h" 
#include "ApiHandler.h" 

void setupWeb(AsyncWebServer &server) {
    
    // Rota 1: O Site (HTML)
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", index_html);
    });

    // Rota 2: A API (Dados)
    server.on("/api/stats", HTTP_GET, [](AsyncWebServerRequest *request){
        String json = getStatsJSON();
        request->send(200, "application/json", json);
    });
}