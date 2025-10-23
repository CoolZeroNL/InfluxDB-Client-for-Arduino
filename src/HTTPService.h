/**
 * 
 * HTTPService.h: HTTP Service
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

#ifndef _HTTP_SERVICE_H_
#define _HTTP_SERVICE_H_

#include <Arduino.h>
#include <Ethernet.h>
#include "Options.h"
#include <functional>

class Test;
typedef std::function<bool(EthernetClient&)> httpResponseCallback;

// External declaration for Transfer-Encoding
extern const char *TransferEncoding;

/**
 * Struct to hold connection information
 */
struct ConnectionInfo {
    String serverUrl;
    String bucket;
    String org;
    String authToken;
    uint8_t dbVersion;
    String user;
    String password;
    const char *certInfo; 
    bool insecure;
    String lastError;
    HTTPOptions httpOptions;
};

/**
 * HTTPService class providing HTTP methods
 */
class HTTPService {
friend class Test;  
private:
    ConnectionInfo *_pConnInfo;    
    String _apiURL;
    uint32_t _lastRequestTime = 0;
    int _lastStatusCode = 0;
    EthernetClient _client;
    int _lastRetryAfter = 0;     

    // Define the extended callback type
    using HttpResponseDataCallback = std::function<bool(EthernetClient&, const String& body, const String& headers, int statusCode)>;

protected:
    // Core function to send HTTP request with extended callback
    bool sendHttpRequest(
        const String &method,
        const String &url,
        const String &headers,
        const String &body,
        int expectedCode,
        HttpResponseDataCallback cb
    );

    // Helper to parse URL into host, port, and path
    bool parseURL(const char *url, String &host, int &port, String &path);
    // Read response headers and body
    bool readResponse(String &responseStr, int &statusCode, String &headers);

public:
    // Constructor
    HTTPService(ConnectionInfo *pConnInfo);
    ~HTTPService() {}

    // Set HTTP options if needed
    void setHTTPOptions() { /* No-op for Ethernet */ }
    HTTPOptions &getHTTPOptions() { return _pConnInfo->httpOptions; }

    // Perform HTTP GET
    // bool doGET(const char* url, int expectedStatus, std::function<bool(EthernetClient&)> cb);
    bool doGET(const char *url, int expectedCode, HttpResponseDataCallback cb);
    
    // Perform HTTP POST with data
    bool doPOST(const char *url, const char *data, const char *contentType, int expectedCode, HttpResponseDataCallback cb);
    // Perform HTTP POST with Stream
    bool doPOST(const char *url, Stream *stream, const char *contentType, int expectedCode, HttpResponseDataCallback cb);
    // Perform HTTP DELETE
    bool doDELETE(const char *url, int expectedCode, std::function<bool(EthernetClient&)> cb);

    // Accessors
    String getServerAPIURL() const { return _apiURL; }
    int getLastRetryAfter() const { return _lastRetryAfter; }
    int getLastStatusCode() const { return _lastStatusCode; }
    uint32_t getLastRequestTime() const { return _lastRequestTime; }
    String getLastErrorMessage() const { return _pConnInfo->lastError; }
    bool isConnected() { return _client.connected(); }
};

#endif //_HTTP_SERVICE_H_