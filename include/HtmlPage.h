#ifndef HTML_PAGE_H
#define HTML_PAGE_H
#include <Arduino.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Comando ESP32</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; background-color: #f4f4f9; }
    .card { background: white; padding: 20px; margin: 20px auto; max-width: 400px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }
    h1 { color: #333; }
    .stat-box { border: 1px solid #ddd; padding: 10px; margin: 5px; border-radius: 5px; }
    .btn { background: #007bff; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; font-size: 16px; }
    .btn:active { background: #0056b3; }
  </style>
</head>
<body>
  <h1>Comando Universal</h1>
  
  <div class="card">
    <h3>Estatisticas (API)</h3>
    <div class="stat-box">Uptime: <span id="uptime">...</span>s</div>
    <div class="stat-box">Comandos: <span id="cmds">0</span></div>
    <div class="stat-box">Wi-Fi: <span id="rssi">...</span> dBm</div>
    <button class="btn" onclick="atualizarStats()">Atualizar Dados</button>
  </div>

  <script>
    function atualizarStats() {
      fetch('/api/stats')
        .then(response => response.json())
        .then(data => {
          document.getElementById('uptime').innerText = data.uptime_segundos;
          document.getElementById('cmds').innerText = data.total_comandos;
          document.getElementById('rssi').innerText = data.sinal_wifi;
        });
    }
    // Atualiza sozinho a cada 5 segundos
    setInterval(atualizarStats, 5000);
    atualizarStats(); 
  </script>
</body>
</html>
)rawliteral";

#endif 