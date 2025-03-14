#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "SSID";
const char* password = "password";

AsyncWebServer server(80);
int fireState = 0;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  pinMode(14, OUTPUT);
  digitalWrite(14, fireState);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String stateStr = (fireState == 1) ? "1" : "0";
    String html = R"(
      <!DOCTYPE html>
      <html lang="en">
      <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Fireplace Control</title>
        <style>
            body { font-family: Arial, sans-serif; text-align: center; padding: 20px; background-color: #111; color: #fff; }
            .fire-icon { width: 100px; height: 100px; background-color: orange; border-radius: 50%; position: relative; margin: 20px auto; box-shadow: 0 0 30px rgba(255, 165, 0, 0.7); transition: all 0.3s ease; }
            .fire-icon.off { background-color: gray; box-shadow: none; animation: none; }
            .fire-icon::after { content: ""; position: absolute; top: 10%; left: 10%; width: 80%; height: 80%; background-color: red; border-radius: 50%; animation: flame-inner 1.5s infinite alternate; }
            .fire-icon.off::after { background-color: #808080; animation: none; }
            @keyframes flame-inner { 0% { transform: scale(1); opacity: 0.9; } 100% { transform: scale(1.3); opacity: 0.7; } }
            .btn-toggle { padding: 15px 30px; background-color: #ff6347; color: white; border: none; border-radius: 10px; font-size: 18px; cursor: pointer; transition: background-color 0.3s; }
            .btn-toggle:hover { background-color: #ff4500; }
        </style>
      </head>
      <body>
        <h1>Fireplace Control</h1>
        <div class="fire-icon"></div>
        <button id="toggleBtn" class="btn-toggle">Turn Fire On</button>
        <script>
            let fireState = <%= stateStr %>;
            const button = document.getElementById('toggleBtn');
            const fireIcon = document.querySelector('.fire-icon');
            function updateUI() {
                if (fireState) {
                    button.innerText = "Turn Fire Off";
                    fireIcon.classList.remove('off');
                    fireIcon.style.animation = "flame 1s infinite alternate";
                } else {
                    button.innerText = "Turn Fire On";
                    fireIcon.classList.add('off');
                    fireIcon.style.animation = "none";
                }
            }
            button.addEventListener('click', function() {
                fireState = !fireState;
                updateUI();
                fetch(`/toggle?state=${fireState ? 1 : 0}`);
            });
            updateUI();
        </script>
      </body>
      </html>
    )";

    html.replace("<%= stateStr %>", stateStr);
    request->send(200, "text/html", html);
  });

  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request){
    String state = request->getParam("state")->value();
    fireState = state.toInt();
    digitalWrite(14, fireState);
    if (fireState == 1) {
      request->send(200, "text/plain", "Fire is ON");
    } else {
      request->send(200, "text/plain", "Fire is OFF");
    }
  });

  server.begin();
}

void loop() {}
