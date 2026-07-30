#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LoRa.h>

ESP8266WebServer server(80);

String latestLat = "0";
String latestLon = "0";

void setup() {
  Serial.begin(9600);

  WiFi.softAP("LoRa-GPS", "12345678"); 
  Serial.println("AP Started: LoRa-GPS");

  server.on("/", handleRoot);
  server.on("/loc", handleLocation);
  server.begin();

  LoRa.setPins(D8, D3, D2);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed!");
    while (1);
  }
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String data = "";
    while (LoRa.available()) {
      data += (char)LoRa.read();
    }

    int commaIndex = data.indexOf(',');
    latestLat = data.substring(0, commaIndex);
    latestLon = data.substring(commaIndex + 1);

    Serial.println("Received: " + data);
  }

  server.handleClient();
}

void handleRoot() {
  String page = "<html><head>"
                "<script>"
                "async function updateMarker(){"
                "let res = await fetch('/loc');"
                "let d = await res.json();"
                "document.getElementById('lat').innerHTML=d.lat;"
                "document.getElementById('lon').innerHTML=d.lon;"
                "map.setView([d.lat, d.lon], 16);"
                "L.marker([d.lat, d.lon]).addTo(map);"
                "}"
                "</script>"

                "<link rel='stylesheet' href='https://unpkg.com/leaflet/dist/leaflet.css' />"
                "<script src='https://unpkg.com/leaflet/dist/leaflet.js'></script>"
                "</head><body>"

                "<h2>LoRa GPS Tracker</h2>"
                "Latitude: <span id='lat'></span><br>"
                "Longitude: <span id='lon'></span><br><br>"

                "<div id='map' style='height:400px;'></div>"

                "<script>"
                "var map = L.map('map').setView([0,0], 2);"
                "L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png').addTo(map);"
                "setInterval(updateMarker, 2000);"
                "</script></body></html>";

  server.send(200, "text/html", page);
}

void handleLocation() {
  String json = "{ \"lat\": \"" + latestLat + "\", \"lon\": \"" + latestLon + "\" }";
  server.send(200, "application/json", json);
}
