//  * HTTPService.cpp: InfluxDB Client for Arduino


#include "HTTPService.h"
#include "Platform.h"
#include "Version.h"

#include "util/helpers.h"
#include "util/debug.h"

static const char UserAgent[] PROGMEM = "influxdb-client-arduino/" INFLUXDB_CLIENT_VERSION " (" INFLUXDB_CLIENT_PLATFORM " " INFLUXDB_CLIENT_PLATFORM_VERSION ")";

const char *TransferEncoding = "Transfer-Encoding";

typedef std::function<bool(EthernetClient&)> httpResponseCallback; // <-- Make sure callback accepts non-const reference

HTTPService::HTTPService(ConnectionInfo *pConnInfo) : _pConnInfo(pConnInfo) {
    _apiURL = pConnInfo->serverUrl;
    _apiURL += "/api/v2/";
}

// Parse URL function remains unchanged...
bool HTTPService::parseURL(const char *url, String &host, int &port, String &path) {
    String urlStr = String(url);
    if (urlStr.startsWith("https")) {
        _pConnInfo->lastError = F("HTTPS not supported");
        return false;
    } else if (urlStr.startsWith("http")) {
        urlStr.remove(0, 7); // remove "http://"
        int slashIdx = urlStr.indexOf('/');
        if (slashIdx >= 0) {
            host = urlStr.substring(0, slashIdx);
            path = urlStr.substring(slashIdx);
        } else {
            host = urlStr;
            path = "/";
        }
        int colonIdx = host.indexOf(':');
        if (colonIdx >= 0) {
            port = host.substring(colonIdx + 1).toInt();
            host = host.substring(0, colonIdx);
        } else {
            port = 80;
        }
        return true;
    } else {
        _pConnInfo->lastError = F("Invalid URL protocol");
        return false;
    }
}

bool HTTPService::readResponse(String &responseStr, int &statusCode, String &headers) {
    // Read status line
    String line = _client.readStringUntil('\n');
    line.trim();
    if (!line.startsWith("HTTP/")) {
        _pConnInfo->lastError = F("Invalid response");
        return false;
    }
    int firstSpace = line.indexOf(' ');
    int secondSpace = line.indexOf(' ', firstSpace + 1);
    if (firstSpace < 0 || secondSpace < 0) {
        _pConnInfo->lastError = F("Malformed response");
        return false;
    }
    statusCode = line.substring(firstSpace + 1, secondSpace).toInt();

    // Read headers
    headers = "";
    while (_client.connected()) {
        line = _client.readStringUntil('\n');
        line.trim();
        if (line.length() == 0) {
            // End of headers
            break;
        }
        headers += line + "\r\n";
    }

    // Read body
    responseStr = "";
    while (_client.connected() && _client.available()) {
        responseStr += _client.readStringUntil('\n') + "\n";
    }

    return true;
}

bool HTTPService::sendHttpRequest(
    const String &method,
    const String &url,
    const String &headers,
    const String &body,
    int expectedCode,
    HttpResponseDataCallback cb
) {
    String host, path;
    int port;

    // Parse URL into host, port, and path
    if (!parseURL(url.c_str(), host, port, path)) {
        return false;
    }

    // Connect to server
    if (!_client.connect(host.c_str(), port)) {
        _pConnInfo->lastError = F("Connection failed");
        return false;
    }

    // Build HTTP request string
    String request = method + " " + path + " HTTP/1.1\r\n";
    request += "Host: " + host + "\r\n";
    request += "User-Agent: " INFLUXDB_CLIENT_VERSION " (" INFLUXDB_CLIENT_PLATFORM " " INFLUXDB_CLIENT_PLATFORM_VERSION ")\r\n";

    // Add Authorization header if token exists
    if (_pConnInfo->authToken.length() > 0) {
        request += "Authorization: Token " + _pConnInfo->authToken + "\r\n";
    }

    // Add custom headers
    if (headers.length() > 0) {
        request += headers + "\r\n";
    }

    // Add body headers if body exists
    if (body.length() > 0) {
        request += "Content-Length: " + String(body.length()) + "\r\n";
        request += "Content-Type: application/json\r\n"; // assuming JSON
        request += "\r\n" + body;
    } else {
        request += "\r\n";
    }

    // Send the request
    _client.print(request);

    // Read the response
    String responseStr;
    int statusCode;
    String respHeaders;

    if (!readResponse(responseStr, statusCode, respHeaders)) {
        _client.stop();
        return false;
    }

    INFLUXDB_CLIENT_DEBUG("[D] HTTPService::sendHttpRequest():: Response String: \n\n%s\n", responseStr.c_str());
    INFLUXDB_CLIENT_DEBUG("[D] HTTPService::sendHttpRequest():: Status Code: %d\n", statusCode);
    INFLUXDB_CLIENT_DEBUG("[D] HTTPService::sendHttpRequest():: Response Headers: \n\n%s\n", respHeaders.c_str());

    // Save last status code and handle "Retry-After" header
    _lastStatusCode = statusCode;
    _lastRetryAfter = 0;

    INFLUXDB_CLIENT_DEBUG("[D] HTTPService::sendHttpRequest():: Retry-After:\n");
    int index = respHeaders.indexOf("Retry-After:");
    if (index >= 0) {
        int endIdx = respHeaders.indexOf("\r\n", index);
        String retryStr = respHeaders.substring(index + 12, endIdx);
        _lastRetryAfter = retryStr.toInt();
    }

    _lastRequestTime = millis();

    // Call the callback with the response data
    
    bool success = false;
    if (cb) {
        INFLUXDB_CLIENT_DEBUG("[D] HTTPService::sendHttpRequest():: YES cb:\n");
        success = cb(_client, responseStr, respHeaders, statusCode);
    }else{
        // if no cb is used, we want only true/false to return of success based on: statusCode == expectedCode
        INFLUXDB_CLIENT_DEBUG("[D] HTTPService::sendHttpRequest():: NO cb:\n");
        if(statusCode == expectedCode){
            success = true;
        }else{
            success = false;
        }

    }

    INFLUXDB_CLIENT_DEBUG("[D] HTTPService::sendHttpRequest():: client stop:\n");
    // Close connection
    _client.stop();

    Serial.println("true/fale, success");
    Serial.println(statusCode);
    Serial.println(expectedCode);
    Serial.println(success);
    Serial.println((statusCode == expectedCode) && success);

    // Return whether the request was successful based on status code
    INFLUXDB_CLIENT_DEBUG("[D] HTTPService::sendHttpRequest():: return:\n");
    return (statusCode == expectedCode) && success;
}

bool HTTPService::doGET(const char* url, int expectedCode, HttpResponseDataCallback cb) {
    // Pass the callback directly to sendHttpRequest
    return sendHttpRequest("GET", String(url), "", "", expectedCode, cb);
}

// For POST with data
bool HTTPService::doPOST(const char *url, const char *data, const char *contentType, int expectedCode, HttpResponseDataCallback cb) {
    String headers = "Content-Type: " + String(contentType);
    String bodyStr = String(data);
    return sendHttpRequest("POST", String(url), headers, bodyStr, expectedCode, cb);
}

// For POST with stream
bool HTTPService::doPOST(const char *url, Stream *stream, const char *contentType, int expectedCode, HttpResponseDataCallback cb) {
    String bodyStr = "";
    while (stream->available()) {
        bodyStr += (char)stream->read();
    }
    String headers = "Content-Type: " + String(contentType);
    return sendHttpRequest("POST", String(url), headers, bodyStr, expectedCode, cb);
}

bool HTTPService::doDELETE(const char* url, int expectedStatus, std::function<bool(EthernetClient&)> cb) {
    // Wrap the simple callback into the extended callback
    HttpResponseDataCallback extendedCb = [cb](EthernetClient &client, const String &body, const String &headers, int statusCode) {
        return cb(client);
    };
    return sendHttpRequest("DELETE", String(url), "", "", expectedStatus, extendedCb);
}