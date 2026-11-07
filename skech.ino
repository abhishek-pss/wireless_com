#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

const char* AP_SSID = "ESP32-Message";
const char* AP_PASSWORD = "hello123";  // At least 8 characters for WPA2.

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WebServer server(80);
String lastMessage = "Waiting for a message...";

void showMessage(const String& message) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Wireless message:");
  display.println();
  display.println(message);
  display.display();
}

void handleHome() {
  server.send(200, "text/html",
    "<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>Send a message</title></head><body><h2>Send to ESP32 display</h2>"
    "<form action='/send' method='get'><input name='message' maxlength='120' required>"
    "<button type='submit'>Send</button></form></body></html>");
}

void handleSend() {
  if (!server.hasArg("message")) {
    server.send(400, "text/plain", "Missing message");
    return;
  }
  lastMessage = server.arg("message");
  if (lastMessage.length() > 120) lastMessage.remove(120);
  showMessage(lastMessage);
  server.send(200, "text/html",
    "<!doctype html><html><body><p>Message sent to the display.</p>"
    "<a href='/'>Send another</a></body></html>");
}

void setup() {
  Wire.begin(21, 22);  // OLED SDA, SCL
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    while (true) delay(1000);
  }

  showMessage("Starting Wi-Fi...");
  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP(AP_SSID, AP_PASSWORD)) {
    showMessage("Wi-Fi startup failed");
    while (true) delay(1000);
  }

  server.on("/", HTTP_GET, handleHome);
  server.on("/send", HTTP_GET, handleSend);
  server.begin();
  showMessage("Connect to ESP32-Message, then open 192.168.4.1");
}

void loop() {
  server.handleClient();
}
