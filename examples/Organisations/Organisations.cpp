/**
 * Organisation management Example code for InfluxDBClient library for Arduino
 * Enter WiFi and InfluxDB parameters below
 *
 * This example supports only InfluxDB running from unsecure (http://...)
 * For secure (https://...) or Influx Cloud 2 connection check SecureWrite example to 
 * see how connect using secured connection (https)
 **/

#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"
#endif

#include <InfluxDbClient.h>

// WiFi AP SSID
#define WIFI_SSID "ssid"
// WiFi password
#define WIFI_PASSWORD "password"
// InfluxDB  server url. Don't use localhost, always server name or ip address.
// E.g. http://192.168.1.48:8086 (In InfluxDB 2 UI -> Load Data -> Client Libraries), 
#define INFLUXDB_URL "influxdb-url"
// InfluxDB 2 server or cloud API authentication token (Use: InfluxDB UI -> Load Data -> Tokens -> <select token>)
// This token must have the correct token permissions, see: https://docs.influxdata.com/influxdb/cloud/reference/cli/influx/auth/create/
#define INFLUXDB_TOKEN "toked-id"
// InfluxDB 2 organization id (Use: InfluxDB UI -> Settings -> Profile -> <name under tile> )
#define INFLUXDB_ORG "org"
// Bucket name that doesn't exist in the db yet
#define INFLUXDB_BUCKET "test-bucket"

void setup() {
  Serial.begin(115200);

  // Connect WiFi
  Serial.println("Connecting to " WIFI_SSID);
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
}

void testClient() {
  // InfluxDB client instance
  InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN);

  // Disable verification 
  // client.setInsecure();

  // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
    return;
  }

  // Get dedicated client for organisation management
  OrganisationsClient organisations = client.getOrganisationsClient();
  
  // Verify organisation does not exist, or delete it
  if(organisations.checkOrganisationExists(INFLUXDB_ORG)) {
    
    Serial.println("Organisation " INFLUXDB_ORG " already exists, deleting" );
    
    // get reference
    Organisation b = organisations.findOrganisation(INFLUXDB_ORG);
    Serial.println(b);
    Serial.println(b.toString());
    
    // Delete organisation
    organisations.deleteOrganisation(b.getID());
  } 

  // Create New Organisation
  Organisation b = organisations.createOrganisation(INFLUXDB_ORG);
  if(!b) {
    // some error occurred
    Serial.print("Organisation creating error: ");
    Serial.println(organisations.getLastErrorMessage());
    return;
  }
  Serial.print("Created Organisation: ");
  Serial.println(b);
  Serial.println(b.toString());
  
}

void loop() {
  // Lets do an E2E test
  // call a client test
  testClient();
  
  Serial.println("Stopping");
  // Stop here, don't loop
  while(1) delay(1);
}
