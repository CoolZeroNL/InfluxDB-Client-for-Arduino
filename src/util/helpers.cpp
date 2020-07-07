/**
 * 
 * helpers.cpp: InfluxDB Client util functions
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
#include "helpers.h"

void timeSync(const char *tzInfo, const char* ntpServer1, const char* ntpServer2, const char* ntpServer3) {
  // Accurate time is necessary for certificate validion
  
  configTzTime(tzInfo,ntpServer1, ntpServer2, ntpServer3);

  // Wait till time is synced
  Serial.print("Syncing time");
  int i = 0;
  while (time(nullptr) < 1000000000ul && i < 40) {
    Serial.print(".");
    delay(500);
    i++;
  }
  Serial.println();

  // Show time
  time_t tnow = time(nullptr);
  Serial.print("Synchronized time: ");
  Serial.println(ctime(&tnow));
}

String escapeKey(String key, bool escapeEqual) {
    String ret;
    ret.reserve(key.length()+5); //5 is estimate of  chars needs to escape,
    
    for (char c: key)
    {
        switch (c)
        {
            case '\r':
            case '\n':
            case '\t':
            case ' ':
            case ',':
                ret += '\\';
                break;
            case '=':
                if(escapeEqual) {
                    ret += '\\';
                }
                break;
        }
        ret += c;
    }
    return ret;
}

String escapeValue(const char *value) {
    String ret;
    int len = strlen_P(value);
    ret.reserve(len+5); //5 is estimate of max chars needs to escape,
    for(int i=0;i<len;i++)
    {
        switch (value[i])
        {
            case '\\':
            case '\"':
                ret += '\\';
                break;
        }

        ret += value[i];
    }
    return ret;
}
