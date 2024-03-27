#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "SPSETUP-C28E";
const char* password = "almost8875chart";

ESP8266WebServer server(80);

int currentTimeSeconds = 0; // Default time in seconds
bool alarmSet = false;
bool timeSet = false;

const char index_html[] PROGMEM = R"(
<!DOCTYPE html>
<html>
<head>
  <title>Set Time</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      padding: 20px;
      background-color: #f2f2f2;
    }

    h1 {
      color: #333333;
    }

    form {
      margin-bottom: 20px;
    }

    input[type='time'] {
      padding: 8px;
      font-size: 16px;
    }

    input[type='submit'] {
      padding: 10px 20px;
      background-color: #4CAF50;
      color: white;
      border: none;
      border-radius: 4px;
      cursor: pointer;
    }

    input[type='submit']:hover {
      background-color: #45a049;
    }

    .button-container {
      margin-top: 20px;
    }

    .button-container button {
      padding: 10px 20px;
      margin-right: 10px;
    }
  </style>
</head>
<body>
  <h1>Set Time</h1>
  <form action="/settime" method="GET">
    Time: <input type="time" name="time" value="00:00" min="00:00" max="23:59"><br><br>
     <input type="submit" value="Set Time" name = "timeSet">
     <input type="submit" value="Set Alarm" name = "alarmSet">
  </form>
</body>
</html>
)";

void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleSetTime() {
  if (server.hasArg("timeSet")) {
    String timeStr = server.arg("time");
    int hours = timeStr.substring(0, 2).toInt();
    int minutes = timeStr.substring(3).toInt();
    currentTimeSeconds = hours * 3600 + minutes * 60;
    Serial.print("Received time in seconds: ");
    Serial.println(currentTimeSeconds);
    timeSet = true;
    alarmSet = false;
    Serial.println(alarmSet);
    Serial.println(timeSet);
    // Here you can process the received time as needed
  }
  else if (server.arg("alarmSet")) {
    String timeStr = server.arg("time");
    int hours = timeStr.substring(0, 2).toInt();
    int minutes = timeStr.substring(3).toInt();
    currentTimeSeconds = hours * 3600 + minutes * 60;
    Serial.print("Received time in seconds: ");
    Serial.println(currentTimeSeconds);
    timeSet = false;
    alarmSet = true;
    Serial.println(alarmSet);
    Serial.println(timeSet);
  }
  server.send(200, "text/plain", "Time set successfully");

  Serial.println(currentTimeSeconds);
  Serial.println(alarmSet);
  Serial.println(timeSet);

}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/settime", HTTP_GET, handleSetTime);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
