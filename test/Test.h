/**
 * 
 * Test.h: Unit and integration tests for InfluxDB client for Arduino
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
#ifndef _TEST_H_
#define _TEST_H_

#include <Arduino.h>

#include "TestBase.h"

class Test : public  TestBase {
public:
    static void run();
private: //helpers
    static void setServerUrl(InfluxDBClient &client, String serverUrl);
private: // tests
    // NO SERVER
        static void testUtils();
        // static void testOptions();
        static void testEscaping();
        static void testPoint();                    //<-- has issue. but time sync is also been disabled...
        static void testOldAPI();
        static void testBatch();
        static void testLineProtocol();
        static void testUrlEncode();
        static void testIsValidID();
        static void testFluxTypes();
        static void testFluxTypesSerialization();
        static void testQueryParams();
    
        // static void testTimestampAdjustment();
    
    // FluxParser
        // static void testFluxParserEmpty();
        // static void testFluxParserSingleTable();
        // static void testFluxParserNilValue();
        // static void testFluxParserMultiTables(bool chunked);
        // static void testFluxParserErrorDifferentColumnsNum();
        // static void testFluxParserFluxError();
        // static void testFluxParserInvalidDatatype();
        // static void testFluxParserMissingDatatype();
        // static void testFluxParserErrorInRow();

    
    // SERVER
        static void testInit();
        static void testRepeatedInit();

        static void testBasicFunction();
        static void testFlushing();
        static void testV1();

        // static void testUserAgent();
        // static void testHTTPReadTimeout();
        // static void testDefaultTags();
        // static void testQueryWithParams();

        // static void testLargeBatch();
        static void testFailedWrites();
        // static void testTimestamp();
        // static void testRetryOnFailedConnection();
        // static void testRetryOnFailedConnectionWithFlush();
        // static void testNonRetry();
        // static void testBufferOverwriteBatchsize1();
        // static void testBufferOverwriteBatchsize5();
        // static void testServerTempDownBatchsize5();
        static void testRetriesOnServerOverload();
        static void testRetryInterval();
        // static void testUseServerTimestamp();
    

    // MAIN FUNCTIONS
        static void testBuckets();
        static void testOrganisations();
    
    
};

#endif //_TEST_H_