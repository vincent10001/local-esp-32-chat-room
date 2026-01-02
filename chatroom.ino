#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

const char* ssid = "name it ur down thingy after you downloaded";
//sometimes it auto connects you but if not just put in "192.169.4.1" 
DNSServer dnsServer;
WebServer server(80);

String messages = "";

const char CHAT_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ur chat</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; background:#111; color:#fff; }
    #chat { height: 300px; overflow-y: auto; border:1px solid #444; padding:5px; }
    input, button { width:100%; margin-top:5px; padding:10px; }
  </style>
</head>
<body>
  <h2>ur chat</h2>
  <div id="chat"></div>
  <input id="msg" placeholder="Type message..." />
  <button onclick="send()">Send</button>

<script>
function load() {
  fetch('/messages').then(r => r.text()).then(t => {
    document.getElementById('chat').innerHTML = t;
    document.getElementById('chat').scrollTop = 99999;
  });
}
function send() {
  let m = document.getElementById('msg').value;
  if (m.length > 0) {
    fetch('/send?m=' + encodeURIComponent(m));
    document.getElementById('msg').value = '';
  }
}
setInterval(load, 1000);
load();
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", CHAT_PAGE);
}

void handleMessages() {
  server.send(200, "text/html", messages);
}

void handleSend() {
  if (server.hasArg("m")) {
    messages += "<p>" + server.arg("m") + "</p>";
    if (messages.length() > 6000) messages = ""; // prevent memory overflow
  }
  server.send(200, "text/plain", "ok");
}

void setup() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);

  dnsServer.start(53, "*", WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/messages", handleMessages);
  server.on("/send", handleSend);
  server.onNotFound(handleRoot);

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
