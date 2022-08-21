/*
  Telemetry.ino

    Read more: https://https://github.com/dxcfl/uv-monitoring_wio-terminal#readme
    Git: https://github.com/dxcfl/uv-monitoring_wio-terminal
    (c) 2022 by dxcfl
*/

#include <rpcWiFi.h>
#include <WiFiClient.h>

// WiFi setup
const char *ssid = "SSID";
const char *password = "PASSWORD";

// ThingSpeak setup
const char *host = "api.thingspeak.com";
const char *apiKey = "APIKEY";

// WiFi connection timeout
#define WIFI_CONNECTION_TIME_OUT 30000

int wifiConnection()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    // Connecting to WiFi network ...
    unsigned long connectTime = millis();
    DEBUG_SERIAL_PRINT("WIFI: Connecting to ");
    DEBUG_SERIAL_PRINT(ssid);
    DEBUG_SERIAL_PRINT(" ");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED & millis() - connectTime < WIFI_CONNECTION_TIME_OUT )
    {
      DEBUG_SERIAL_PRINT(".");
      delay(1000);
    }
    DEBUG_SERIAL_PRINTLN("");

    if (WiFi.status() != WL_CONNECTED) {
      DEBUG_SERIAL_PRINTLN("WIFI: Connection failed. ");
      return -1;
    }

    return 0;
  }

  // Connected
  DEBUG_SERIAL_PRINTLN("WIFI: connected. ");
  DEBUG_SERIAL_PRINT("WIFI: Local IP address: ");
  DEBUG_SERIAL_PRINTLN(WiFi.localIP());
}

void telemetrySetup() {
    DEBUG_SERIAL_PRINTLN("WIFI: Preparing ...");
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
}

void telemetrySendValues(float temperature, float humidity, unsigned uv, unsigned short uvi)
{

  // Always check (& establish) WiFi connection
  if ( !wifiConnection() ) {
    return;
  }
  
  // WiFI signal strength
  long rssi = WiFi.RSSI();
  DEBUG_SERIAL_PRINT("WIFI: RSSI:");
  DEBUG_SERIAL_PRINTLN(rssi);

  // Connect to ThingSpeak ...
  DEBUG_SERIAL_PRINT("TELEMETRY: Connecting to ");
  DEBUG_SERIAL_PRINT(host);
  DEBUG_SERIAL_PRINTLN(" ...");
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort))
  {
    DEBUG_SERIAL_PRINTLN("TELEMETRY: Connection failed.");
    return;
  }
  DEBUG_SERIAL_PRINTLN("TELEMETRY: Connected.");

  // Build URI for the request ...
  String url = "/update/";
  url += "?api_key=";
  url += apiKey;
  url += "&field1=";
  url += temperature;
  url += "&field2=";
  url += humidity;
  url += "&field3=";
  url += uv;
  url += "&field4=";
  url += uvi;

  // Send request ...
  DEBUG_SERIAL_PRINT("TELEMETRY: Sending request - URL = ");
  DEBUG_SERIAL_PRINTLN(url);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();

  while (client.available() == 0)
  {
    long responseTime = millis() - timeout;
    if (responseTime > 5000)
    {
      DEBUG_SERIAL_PRINTLN("TELEMETRY: Client timeout !");
      client.stop();
      return;
    }
  }

  // Read response lines from server ...
  DEBUG_SERIAL_PRINTLN("TELEMETRY: Receiving response:");
  while (client.available())
  {
    String line = client.readStringUntil('\r');
    DEBUG_SERIAL_PRINT(line);
  }
  DEBUG_SERIAL_PRINTLN();

  // Close connection ...
  DEBUG_SERIAL_PRINTLN("TELEMETRY: Closing connection ...");
  client.stop();
  DEBUG_SERIAL_PRINTLN("TELEMETRY: Disconnected.");
}
