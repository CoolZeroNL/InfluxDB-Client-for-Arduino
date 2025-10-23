
#include <Arduino.h>

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

#include <SPI.h>
// Custom SPI pins
#define SPI_MISO 19
#define SPI_MOSI 13
#define SPI_SCLK 18
#define SPI_CS   5

// MAC address
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Static IP configuration (if DHCP fails)
#define MYIPADDR 10,20,0,177
#define MYIPMASK 255,255,255,0
#define MYDNS 10,20,0,5
#define MYGW 10,20,0,5

// Ethernet client
EthernetClient client;

#if defined(ESP32)

    // #include <WiFi.h>
    #include <Ethernet.h>

// #elif defined(ESP8266)
// #include <ESP8266WiFi.h>
#endif

#define INFLUXDB_CLIENT_TESTING_SERVER_HOST "192.168.178.164"
#define INFLUXDB_CLIENT_TESTING_ORG "my-org"
#define INFLUXDB_CLIENT_TESTING_BUC "my-bucket"
#define INFLUXDB_CLIENT_TESTING_DB "my-db"
#define INFLUXDB_CLIENT_TESTING_TOK "my-token"
#define INFLUXDB_CLIENT_TESTING_SSID "SSID"
#define INFLUXDB_CLIENT_TESTING_PASS "password"

#include "customSettings.h"

#define INFLUXDB_CLIENT_MANAGEMENT_URL "http://" INFLUXDB_CLIENT_TESTING_SERVER_HOST ":998"
#define INFLUXDB_CLIENT_TESTING_URL "http://" INFLUXDB_CLIENT_TESTING_SERVER_HOST ":999"
#define INFLUXDB_CLIENT_E2E_TESTING_URL "http://" INFLUXDB_CLIENT_TESTING_SERVER_HOST ":8086"

#include "TestSupport.h"
#include "Test.h"
// #include "E2ETest.h"

void setup();
void loop();
void initInet();

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("Initializing tests");
    Serial.println(" Compiled on " __DATE__ " " __TIME__);

    randomSeed(123);

    Serial.println();

    initInet();

    Test::setup(INFLUXDB_CLIENT_MANAGEMENT_URL,INFLUXDB_CLIENT_TESTING_URL, INFLUXDB_CLIENT_E2E_TESTING_URL, INFLUXDB_CLIENT_TESTING_ORG, INFLUXDB_CLIENT_TESTING_BUC, INFLUXDB_CLIENT_TESTING_DB, INFLUXDB_CLIENT_TESTING_TOK );

    Serial.printf("Using server: %s\n", INFLUXDB_CLIENT_TESTING_URL);
}

void loop() {
    time_t now = time(nullptr);
    Serial.print("Start time: ");
    Serial.println(ctime(&now));
    uint32_t start = millis();
    uint32_t startRAM = ESP.getFreeHeap();
    Serial.printf("Start RAM: %d\n", startRAM);
    Test::run();
    // E2ETest::run();
    uint32_t endRAM = ESP.getFreeHeap();
    Serial.printf("End RAM %d, diff: %d\n", endRAM, endRAM-startRAM);
    now = time(nullptr);
    Serial.print("End time: ");
    Serial.print(ctime(&now));
    Serial.printf("  Took: %.1fs\n", (millis()-start)/1000.0f);


    while(1) {
        delay(1000);
    }
}

void initInet() {

    Serial.println("Begin Ethernet with custom SPI pins");

    // Initialize SPI with custom pins
    SPI.begin(SPI_SCLK, SPI_MISO, SPI_MOSI);
    Ethernet.init(SPI_CS); // Set CS pin for Ethernet

    // Attempt DHCP connection
    if (Ethernet.begin(mac)) {
        Serial.println("DHCP OK!");
    } else {
        Serial.println("Failed to configure Ethernet using DHCP");
        // Check hardware and fallback to static IP
        if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println("Ethernet shield was not found. Sorry, can't run without hardware. :(");
        while (true) { delay(1); }
        }
        if (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("Ethernet cable is not connected.");
        }
        IPAddress ip(MYIPADDR);
        IPAddress dns(MYDNS);
        IPAddress gw(MYGW);
        IPAddress sn(MYIPMASK);
        Ethernet.begin(mac, ip, dns, gw, sn);
        Serial.println("STATIC IP assigned");
    }

    
    delay(5000);

    Serial.print("Local IP : ");
    Serial.println(Ethernet.localIP());
    Serial.print("Subnet Mask : ");
    Serial.println(Ethernet.subnetMask());
    Serial.print("Gateway IP : ");
    Serial.println(Ethernet.gatewayIP());
    Serial.print("DNS Server : ");
    Serial.println(Ethernet.dnsServerIP());

    Serial.println("Ethernet Successfully Initialized");

    
    // timeSync("CET-1CEST,M3.5.0,M10.5.0/3", "0.cz.pool.ntp.org", "1.cz.pool.ntp.org", "pool.ntp.org");

    // deleteAll(INFLUXDB_CLIENT_TESTING_URL);

}
