#include "CsvReader.h"
#include "util/debug.h"

const size_t MAX_FIELDS = 100;        // Maximum number of fields per row
const size_t MAX_LINE_LENGTH = 1024;  // Maximum line length to process

CsvReader::CsvReader(HttpStreamScanner *scanner) {
    _scanner = scanner;
    INFLUXDB_CLIENT_DEBUG("[D] CsvReader:: initialized with scanner at address: %p\n", (void*)scanner);
}

CsvReader::~CsvReader() {
    delete _scanner;
}

std::vector<String> CsvReader::getRow() {
    return _row;
}

void CsvReader::close() {
    clearRow();
    _scanner->close();
}

void CsvReader::clearRow() {
    _row.clear();
}

enum class CsvParsingState {
    UnquotedField,
    QuotedField,
    QuotedQuote
};

// bool CsvReader::next() {
//     INFLUXDB_CLIENT_DEBUG("[D] CsvReader:: next() - called\n");
//     clearRow();

//     bool status = _scanner->next();

//     if (!status) {
//         INFLUXDB_CLIENT_DEBUG("[D] CsvReader:: next() - Scanner next() failed, error: %d\n", _scanner->getError());
//         _error = _scanner->getError(); // Assuming _error is int
//         return false;
//     }

//     String line = _scanner->getLine();

//     // Safety check for line length
//     if (line.length() > MAX_LINE_LENGTH) {
//         INFLUXDB_CLIENT_DEBUG("[D] CsvReader:: next() - Line too long, length: %d\n", (int)line.length());
//         _error = -1; // set an error code
//         return false;
//     }

//     INFLUXDB_CLIENT_DEBUG("[D] CsvReader:: next() - Read line: %s\n", line.c_str());

//     CsvParsingState state = CsvParsingState::UnquotedField;
//     std::vector<String> fields {""};
//     size_t i = 0; // current field index

//     for (char c : line) {
//         switch (state) {
//             case CsvParsingState::UnquotedField:
//                 switch (c) {
//                     case ',': // end of field
//                         if (fields.size() >= MAX_FIELDS) {
//                             INFLUXDB_CLIENT_DEBUG("[D] CsvReader:: next() - Max fields reached\n");
//                             _error = -2; // error code for too many fields
//                             return false;
//                         }
//                         fields.push_back(""); i++;
//                         break;
//                     case '"':
//                         state = CsvParsingState::QuotedField;
//                         break;
//                     default:
//                         fields[i] += c;
//                         break;
//                 }
//                 break;
//             case CsvParsingState::QuotedField:
//                 switch (c) {
//                     case '"':
//                         state = CsvParsingState::QuotedQuote;
//                         break;
//                     default:
//                         fields[i] += c;
//                         break;
//                 }
//                 break;
//             case CsvParsingState::QuotedQuote:
//                 switch (c) {
//                     case ',':
//                         if (fields.size() >= MAX_FIELDS) {
//                             INFLUXDB_CLIENT_DEBUG("[D] CsvReader:: next() - Max fields reached after quote\n");
//                             _error = -2;
//                             return false;
//                         }
//                         fields.push_back(""); i++;
//                         state = CsvParsingState::UnquotedField;
//                         break;
//                     case '"':
//                         // Escaped quote
//                         fields[i] += '"';
//                         state = CsvParsingState::QuotedField;
//                         break;
//                     default:
//                         // End of quote
//                         state = CsvParsingState::UnquotedField;
//                         break;
//                 }
//                 break;
//         }
//     }

//     // Optional: Trim trailing empty fields if needed
//     _row = fields;

//     // Debug output
//     INFLUXDB_CLIENT_DEBUG("[D] CsvReader:: next() - Parsed %d fields\n", (int)fields.size());
//     for (size_t idx = 0; idx < fields.size() && idx < 3; ++idx) {
//         INFLUXDB_CLIENT_DEBUG("[D] CsvReader:: next() - Field %d: %s\n", (int)idx, fields[idx].c_str());
//     }

//     return true;
// }

bool CsvReader::next() {
        INFLUXDB_CLIENT_DEBUG("[D] CsvReader:: next() - called\n");

    clearRow();

    bool status = _scanner->next();

    if (!status) {
        _error = _scanner->getError();
        return false;
    }

    String line = _scanner->getLine();

    line.trim();        // ( we need to get rid of the newline at the end...)

    // Safety check
    if (line.length() > MAX_LINE_LENGTH) {
        _error = -1;
        return false;
    }

    INFLUXDB_CLIENT_DEBUG("[D] CsvReader:: next() - Read line: %s\n", line.c_str());

    CsvParsingState state = CsvParsingState::UnquotedField;
    std::vector<String> fields;
    fields.push_back(""); // start with an empty field
    size_t i = 0; // current field index

    for (char c : line) {
        switch (state) {
            case CsvParsingState::UnquotedField:
                switch (c) {
                    case ',':
                        // End of current field, start a new one
                        fields.push_back("");
                        i++;
                        break;
                    case '"':
                        state = CsvParsingState::QuotedField;
                        break;
                    default:
                        fields[i] += c;
                        break;
                }
                break;
            case CsvParsingState::QuotedField:
                switch (c) {
                    case '"':
                        state = CsvParsingState::QuotedQuote;
                        break;
                    default:
                        fields[i] += c;
                        break;
                }
                break;
            case CsvParsingState::QuotedQuote:
                switch (c) {
                    case ',':
                        // End of quoted field
                        fields.push_back("");
                        i++;
                        state = CsvParsingState::UnquotedField;
                        break;
                    case '"':
                        // Escaped quote
                        fields[i] += '"';
                        state = CsvParsingState::QuotedField;
                        break;
                    default:
                        // End of quote
                        state = CsvParsingState::UnquotedField;
                        break;
                }
                break;
        }
    }

    _row = fields;

    // Debug
    INFLUXDB_CLIENT_DEBUG("[D] CsvReader:: next() - Parsed %d fields\n", (int)fields.size());
    for (size_t idx = 0; idx < fields.size() && idx < 4; ++idx) {
        INFLUXDB_CLIENT_DEBUG("[D] CsvReader:: next() - Field %d: %s\n", (int)idx, fields[idx].c_str());
    }

    return true;
}