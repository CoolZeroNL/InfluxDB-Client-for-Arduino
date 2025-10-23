// // /**
// //  * 
// //  * HttpStreamScanner.h:  Scannes HttpClient stream for lines. Supports chunking.
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

// // #ifndef _HTTP_STREAM_SCANNER_
// // #define _HTTP_STREAM_SCANNER_

// // #include <Stream.h> // Include Stream for EthernetClient

// // #include <Ethernet.h> // Include Ethernet

// // /** 
// //  * HttpStreamScanner parses response stream from EthernetClient for lines.
// //  * Supports chunked transfer.
// //  */
// // class HttpStreamScanner {
// // public:
// //     // Constructor now accepts EthernetClient & instead of HTTPClient *
// //     HttpStreamScanner(EthernetClient &client, bool chunked);
// //     bool next();
// //     void close();
// //     const String &getLine() const { return _line; }
// //     int getError() const { return _error; }
// //     int getLinesNum() const { return _linesNum; }
// // private:
// //     EthernetClient &_client; // Reference to EthernetClient
// //     Stream &_stream; // Stream reference for reading
// //     int _len; // Remaining length, -1 for unknown
// //     String _line;
// //     int _linesNum = 0;
// //     int _read = 0;
// //     bool _chunked;
// //     bool _chunkHeader;
// //     int _chunkLen = 0;
// //     String _lastChunkLine;
// //     int _error = 0;
// // };

// // #endif // _HTTP_STREAM_SCANNER_


// #ifndef _HTTP_STREAM_SCANNER_
// #define _HTTP_STREAM_SCANNER_

// #include <Stream.h>
// #include <Arduino.h> // For String

// class HttpStreamScanner {
// public:
//     virtual ~HttpStreamScanner() {}

//     // Reads the next line from the stream
//     virtual bool next() = 0;

//     // Closes the scanner, releasing resources if needed
//     virtual void close() = 0;

//     // Gets the current line after a successful next()
//     virtual const String &getLine() const = 0;

//     // Gets the last error code
//     virtual int getError() const = 0;
// };

// #endif // _HTTP_STREAM_SCANNER_


// StringStreamScanner.h
#ifndef _STRING_STREAM_SCANNER_H_
#define _STRING_STREAM_SCANNER_H_

#include <Arduino.h>

class HttpStreamScanner {
public:
    explicit HttpStreamScanner(const String &data)
        : _data(data), _pos(0) {}

    bool next() {
        if (_pos >= _data.length()) {
            return false; // End of input
        }
        _line = "";
        while (_pos < _data.length()) {
            char c = _data.charAt(_pos++);
            _line += c;
            if (c == '\n') break; // Stop at new line
        }
        return _line.length() > 0; // Return false if no line was read
    }

    const String &getLine() const { return _line; }
    int getError() const { return 0; } // No error for string input
    void close() {} // Nothing to do

private:
    String _data;
    size_t _pos;
    String _line;
};

#endif // _STRING_STREAM_SCANNER_H_