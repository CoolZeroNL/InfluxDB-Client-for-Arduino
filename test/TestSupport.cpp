/**
 * 
 * TestSupport.cpp: Supporting functions for tests
 * 
 * MIT License
 * 
 * Copyright (c) 2020 InfluxData
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include <Arduino.h>
#if defined(ESP32)
  // # include <HTTPClient.h>
  #include <Ethernet.h>

#include "util/debug.h"

// #elif defined(ESP8266)
// # include <ESP8266HTTPClient.h>

#endif

#include "TestSupport.h"

// EthernetClient client;

// Remove HTTPClient object
// static HTTPClient httpClient;


void printFreeHeap() {
  Serial.print("[TD] Free heap: ");  
  Serial.println(ESP.getFreeHeap());
}

// Helper function to check Ethernet link status
bool isEthernetUp() {
  return Ethernet.linkStatus() == EthernetLinkStatus::LinkON;
}

// Helper function to create and connect EthernetClient
// This will be used in place of httpClient.begin()

// Replacing httpPOST with raw EthernetClient POST implementation
int httpPOST(const String &url, const String &mess) {
  // Parse URL to extract host and port
  // Expect URL format: http://host:port/path
  // For simplicity, assume URL is like "http://hostname:port"
  String urlCopy = url;
  if (urlCopy.startsWith("http://")) {
    urlCopy.remove(0, 7);
  } else {
    return 0; // invalid URL
  }

  int pathIndex = urlCopy.indexOf('/');
  String hostPort = (pathIndex >= 0) ? urlCopy.substring(0, pathIndex) : urlCopy;
  String path = (pathIndex >= 0) ? urlCopy.substring(pathIndex) : "/";

  int colonIndex = hostPort.indexOf(':');
  String host = (colonIndex >= 0) ? hostPort.substring(0, colonIndex) : hostPort;
  uint16_t port = (colonIndex >= 0) ? hostPort.substring(colonIndex + 1).toInt() : 80;

  EthernetClient client;
  if (!client.connect(host.c_str(), port)) {
    return 0; // connection failed
  }

  // Prepare HTTP POST request
  String request = "POST " + path + " HTTP/1.1\r\n";
  request += "Host: " + host + "\r\n";
  request += "Content-Type: text/plain\r\n";
  request += "Content-Length: " + String(mess.length()) + "\r\n";
  request += "Connection: close\r\n\r\n";
  request += mess;

  client.print(request);

  // Wait for server response (basic)
  unsigned long timeout = millis() + 5000;
  while (!client.available() && millis() < timeout) {
    delay(10);
  }

  int responseCode = 0;
  if (client.available()) {
    String line = client.readStringUntil('\r');
    // Parse response code
    int index = line.indexOf(' ');
    if (index >= 0) {
      responseCode = line.substring(index + 1).toInt();
    }
  }

  client.stop();
  return responseCode;
}

// Replacing httpGET with raw EthernetClient GET implementation
int httpGET(const String &url) {
  // Parse URL
  String urlCopy = url;
  if (urlCopy.startsWith("http://")) {
    urlCopy.remove(0, 7);
  } else {
    return 0;
  }

  int pathIndex = urlCopy.indexOf('/');
  String hostPort = (pathIndex >= 0) ? urlCopy.substring(0, pathIndex) : urlCopy;
  String path = (pathIndex >= 0) ? urlCopy.substring(pathIndex) : "/";

  int colonIndex = hostPort.indexOf(':');
  String host = (colonIndex >= 0) ? hostPort.substring(0, colonIndex) : hostPort;
  uint16_t port = (colonIndex >= 0) ? hostPort.substring(colonIndex + 1).toInt() : 80;

  EthernetClient client;
  if (!client.connect(host.c_str(), port)) {
    return 0; // connection failed
  }

  String request = "GET " + path + " HTTP/1.1\r\n";
  request += "Host: " + host + "\r\n";
  request += "Connection: close\r\n\r\n";

  client.print(request);

  // Wait for response
  unsigned long timeout = millis() + 5000;
  while (!client.available() && millis() < timeout) {
    delay(10);
  }

  int responseCode = 0;
  if (client.available()) {
    String line = client.readStringUntil('\r');
    int index = line.indexOf(' ');
    if (index >= 0) {
      responseCode = line.substring(index + 1).toInt();
    }
  }

  client.stop();
  return responseCode;
}

// Now, replace all calls to httpPOST and httpGET with httpPOST and httpGET

bool deleteAll(const String &url) {
  if (isEthernetUp()) {
    return httpPOST(url + "/api/v2/delete", "") == 204;
    
  }
  return false;
}

bool serverLog(const String &url, String mess) {
  if (isEthernetUp()) {
    return httpPOST(url + "/log", mess) == 204;
  }
  return false;
}

bool isServerUp(const String &url) {
  if (isEthernetUp()) {
    return httpGET(url + "/status") == 200;
  }
  return false;
}


int countParts(const String &str, char separator) {
  int lines = 0;
  int i,from = 0;
  while((i = str.indexOf(separator, from)) >= 0) {
    ++lines;
    from = i+1;
  }
  // try last part
  if(from < str.length() && str.substring(from).length()>0) {
    ++lines;
  }
  return lines;
}

String *getParts(const String &str, char separator, int &count) {
  count = countParts(str, separator);
  String *ret = new String[count];
  int i,from = 0,p=0;
  while((i = str.indexOf(separator, from)) >= 0) {
    ret[p++] = str.substring(from,i);
    from = i+1;
  }
  // try last part
  if(from < str.length() && str.substring(from).length()>0) {
    ret[p] = str.substring(from);
  }
  return ret;
}

int countLines(FluxQueryResult flux) {
  INFLUXDB_CLIENT_DEBUG("[D] countLines - FluxQueryResult: %p\n", &flux);
  
  int lines = 0;
  while(flux.next()) {
    lines++;
    INFLUXDB_CLIENT_DEBUG("[D] count: %d", lines);
  }
  flux.close();
  return lines;
}

std::vector<String> getLines(FluxQueryResult flux) {
  std::vector<String> lines;
  while(flux.next()) {
    String line;
    int i=0;
    for(auto &val: flux.getValues()) {
      if(i>0) line += ",";
      line += val.getRawValue();
      i++;
    }
    lines.push_back(line);
  }
  flux.close();
  return lines;
}


bool compareTm(tm &tm1, tm &tm2) {
    time_t t1 = mktime(&tm1);
    time_t t2 = mktime(&tm2);
    return difftime(t1, t2) == 0;
} 

bool waitServer(const String &url, bool state) {
    int i = 0;
    while(isServerUp(url) != state && i<10 ) {
        if(!i) {
            Serial.println(state?"[TD] Starting server":"[TD] Shuting down server");
            httpGET(url + (state?"/start":"/stop"));
        }
        delay(500);
        i++;
    }
    return isServerUp(url) == state;
}


