// // /**
// //  * 
// //  * HttpStreamScanner.cpp: Scannes HttpClient stream for lines. Supports chunking.
// //  * 
// //  * MIT License
// //  * 
// //  * Copyright (c) 2020 InfluxData
// //  * 
// //  * Permission is hereby granted, free of charge, to any person obtaining a copy
// //  * of this software and associated documentation files (the "Software"), to deal
// //  * in the Software without restriction, including without limitation the rights
// //  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// //  * copies of the Software, and to permit persons to whom the Software is
// //  * furnished to do so, subject to the following conditions:
// //  * 
// //  * The above copyright notice and this permission notice shall be included in all
// //  * copies or substantial portions of the Software.
// //  * 
// //  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// //  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// //  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// //  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// //  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// //  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// //  * SOFTWARE.
// // */
// #include "HttpStreamScanner.h"

// // #define HTTPC_ERROR_READ_TIMEOUT -1
// // #define HTTPC_ERROR_CONNECTION_LOST -2

// // // Uncomment below for debug
// // //#define INFLUXDB_CLIENT_DEBUG_ENABLE

// #include "util/helpers.h"
// #include "util/debug.h"

// // HttpStreamScanner::HttpStreamScanner(EthernetClient &client, bool chunked)
// //     : _client(client), _stream(_client), _chunked(chunked), _chunkHeader(chunked)
// // {
// //     _len = -1; // Unknown length, since EthernetClient does not provide getSize()
// //     INFLUXDB_CLIENT_DEBUG("[D] HttpStreamScanner:: chunked: %s\n", bool2string(_chunked));
// // }



// // bool HttpStreamScanner::next() {
// //     INFLUXDB_CLIENT_DEBUG("[D] HttpStreamScanner:: next(): called\n");
    
// //     INFLUXDB_CLIENT_DEBUG("[D] HttpStreamScanner:: next(): - while: - connected=%s, _len=%d\n", bool2string(_client.connected()), _len);

// //     while (_client.connected() && (_len > 0 || _len == -1)) {
// //         _line = _stream.readStringUntil('\n');
// //         INFLUXDB_CLIENT_DEBUG("[D] HttpStreamScanner:: line: %s\n", _line.c_str());
// //         ++_linesNum;

// //         int lineLen = _line.length();
// //         if (lineLen == 0) {
// //             _error = HTTPC_ERROR_READ_TIMEOUT;
// //             return false;
// //         }

// //         int r = lineLen + 1; // +1 for the newline character
// //         _line.trim(); // remove \r

// //         if (!_chunked || !_chunkHeader) {
// //             _read += r;
// //             if (_lastChunkLine.length() > 0) { // fix broken line
// //                 _line = _lastChunkLine + _line;
// //                 _lastChunkLine = "";
// //             }
// //         }

// //         if (_chunkHeader && r == 2) { // empty line at end of chunk
// //             _line = _lastChunkLine;
// //             _lastChunkLine = "";
// //             return true;
// //         }

// //         if (_chunkHeader) {
// //             // Parse chunk length from hex string
// //             _chunkLen = (int) strtol((const char *) _line.c_str(), NULL, 16);
// //             INFLUXDB_CLIENT_DEBUG("[D] HttpStreamScanner:: chunk len: %d\n", _chunkLen);
// //             _chunkHeader = false;
// //             _read = 0;
// //             if (_chunkLen == 0) { // last chunk
// //                 _error = 0;
// //                 _line = "";
// //                 return false;
// //             } else {
// //                 continue;
// //             }
// //         } else if (_chunked && _read >= _chunkLen) { // end of chunk
// //             _lastChunkLine = _line;
// //             _chunkHeader = true;
// //             continue;
// //         }

// //         if (_len > 0) {
// //             _len -= r;
// //             INFLUXDB_CLIENT_DEBUG("[D] HttpStreamScanner:: new len: %d\n", _len);
// //         }

// //         return true;
// //     }

// //     if (!_client.connected() && ((_chunked && _chunkLen > 0) || (!_chunked && _len > 0))) {
// //         _error = HTTPC_ERROR_CONNECTION_LOST;
// //         INFLUXDB_CLIENT_DEBUG("HttpStreamScanner:: connection lost\n");
// //     }
// //     return false;
// // }

// // void HttpStreamScanner::close() {
// //     _client.stop();
// // }


// // #include "HttpStreamScanner.h"

// #ifndef _STRING_HTTP_STREAM_SCANNER_
// #define _STRING_HTTP_STREAM_SCANNER_

// #include "HttpStreamScanner.h"
// #include <Arduino.h>

// class StringHttpStreamScanner : public HttpStreamScanner {
// public:
//     explicit StringHttpStreamScanner(const String &body);
//     virtual ~StringHttpStreamScanner() {}

//     bool next() override;
//     void close() override;
//     const String &getLine() const override { return _line; }
//     int getError() const override { return _error; }

// private:
//     String _body;
//     size_t _pos;
//     String _line;
//     int _error = 0;
// };

// #endif // _STRING_HTTP_STREAM_SCANNER_