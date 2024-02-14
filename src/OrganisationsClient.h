/**
 * 
 * OrganisationsClient.h: InfluxDB Organisations Client
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
#ifndef _ORG_CLIENT_H_
#define _ORG_CLIENT_H_

#include <HTTPService.h>
#include <memory>

class OrganisationsClient;
class Test;
/**
 * Organisation represents a bucket in the InfluxDB 2 server
 **/
class Organisation {
friend class OrganisationsClient;  
friend class Test;
  public:
      // Create empty, invalid, bucket instance
    Organisation();
    // Create a bucket instance   
    Organisation(const char *id, const char *name);
    // Copy constructor
    Organisation(const Organisation &other);
    // Assignment operator
    Organisation &operator=(const Organisation &other);
    // for testing validity
    operator bool() const { return !isNull(); }
    // Clean bucket 
    ~Organisation();
    // Returns Organisation ID
    const char *getID() const { return _data?_data->id:nullptr; }
    // Retuns bucket name
    const char *getName() const { return _data?_data->name:nullptr; }
    // Retention policy in sec, 0 - inifinite
    // uint32_t getExpire() const { return _data?_data->expire:0; }
    // Checks if it is null instance
    bool isNull() const { return _data == nullptr; }
    // String representation
    String toString() const;
  private:
    class Data {
      public:
        Data(const char *id, const char *name);
        ~Data();
        char *id;
        char *name;
      };
    std::shared_ptr<Data> _data;
};

class InfluxDBClient;
class E2ETest;

/**
 * OrganisationsClient is a client for managing buckets in the InfluxDB 2 server
 * A new bucket can be created, or a bucket can be checked for existence by its name.
 * A bucket can be also deleted.
 **/
class OrganisationsClient {
friend class InfluxDBClient;
friend class Test;
friend class E2ETest;
  public:
    // Copy contructor
    OrganisationsClient(const OrganisationsClient &other);
    // Assignment operator
    OrganisationsClient &operator=(const OrganisationsClient &other);
    // nullptr assignment for clearing
    OrganisationsClient &operator=(std::nullptr_t);
    // for testing validity
    operator bool() const { return !isNull(); }
    // Returns true if a bucket exists
    bool checkOrganisationExists(const char *orgName);
    // Returns a Organisation instance if a bucket is found.
    Organisation findOrganisation(const char *orgName);
    // Creates a bucket with given name and optional retention policy. 0 means infinite.
    // Organisation createOrganisation(const char *orgName, uint32_t expiresSec = 0);
    Organisation createOrganisation(const char *orgName);
    // Delete a bucket with given id. Use findOrganisation to get a bucket with id.
    // bool deleteOrganisation(const char *id);
    // Returns last error message 
    String getLastErrorMessage() { return _data?_data->pConnInfo->lastError:""; }
    // check validity
    bool isNull() const { return _data == nullptr; }
  protected:
    OrganisationsClient();
    OrganisationsClient(ConnectionInfo *pConnInfo, HTTPService *service);
    String getOrgID(const char *org);
  private:    
    class Data {
      public:
        Data(ConnectionInfo *pConnInfo, HTTPService *pService):pConnInfo(pConnInfo),pService(pService) {};
        ConnectionInfo *pConnInfo;
        HTTPService *pService;
      };
    std::shared_ptr<Data> _data;
};
#endif
