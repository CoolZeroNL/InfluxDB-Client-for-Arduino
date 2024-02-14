/**
 * 
 * OrganisationsClient.cpp: InfluxDB Organisations Client
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
#include "OrganisationsClient.h"
#include "util/helpers.h"

#include "util/debug.h"

static const char *propTemplate PROGMEM = "\"%s\":";
// Finds first id property from JSON response
enum class PropType {
  String,
  Number
};

static String findProperty(const char *prop,const String &json, PropType type = PropType::String);

static String findProperty(const char *prop,const String &json, PropType type) {
  INFLUXDB_CLIENT_DEBUG("[D] Searching for %s in %s\n", prop, json.c_str());
  int propLen = strlen_P(propTemplate)+strlen(prop)-2;
  char *propSearch = new char[propLen+1];
  sprintf_P(propSearch, propTemplate, prop);
  int i = json.indexOf(propSearch);
  delete [] propSearch;
  if(i>-1) {
    INFLUXDB_CLIENT_DEBUG("[D]   Found at %d\n", i);
    switch(type) {
      case PropType::String:
        i = json.indexOf("\"", i+propLen);
        if(i>-1) {
         INFLUXDB_CLIENT_DEBUG("[D]    Found starting \" at %d\n", i);
          int e = json.indexOf("\"", i+1);
          if(e>-1) {
            INFLUXDB_CLIENT_DEBUG("[D]    Found ending \" at %d\n", e);
            return json.substring(i+1, e);
          }
        }
      break;
      case PropType::Number:
        i = i+propLen;
        while(json[i] == ' ') {
          i++;
        }
        INFLUXDB_CLIENT_DEBUG("[D]    Found beginning of number at %d\n", i);
        int e = json.indexOf(",", i+1);
        if(e>-1) {
          INFLUXDB_CLIENT_DEBUG("[D]    Found , at %d\n", e);
          return json.substring(i, e);
        }
      break;
    }
  }
  return "";
}

char *copyOrgChars(const char *str) {
  char *ret = new char[strlen(str)+1];
  strcpy(ret, str);
  return ret;
}

Organisation::Organisation():_data(nullptr) {
}

// Organisation::Organisation(const char *id, const char *name, const uint32_t expire) {
//   _data = std::make_shared<Data>(id, name, expire);
// }

Organisation::Organisation(const Organisation &other) {
    _data = other._data;
}

Organisation& Organisation::operator=(const Organisation& other) {
    if(this != &other) {
        _data = other._data;
    }
    return *this;
}

Organisation::~Organisation() {
}


// Organisation::Data::Data(const char *id, const char *name, const uint32_t expire) {
Organisation::Data::Data(const char *id, const char *name) {
  this->id = copyOrgChars(id);
  this->name = copyOrgChars(name);
  // this->expire = expire;
}

      // {
      //   "createdAt": "2022-08-24T23:05:52.881317Z",
      //   "description": "",
      //   "id": "INFLUX_ORG_ID",
      //   "links": {
      //     "buckets": "/api/v2/buckets?org=INFLUX_ORG",
      //     "dashboards": "/api/v2/dashboards?org=INFLUX_ORG",
      //     "labels": "/api/v2/orgs/INFLUX_ORG_ID/labels",
      //     "logs": "/api/v2/orgs/INFLUX_ORG_ID/logs",
      //     "members": "/api/v2/orgs/INFLUX_ORG_ID/members",
      //     "owners": "/api/v2/orgs/INFLUX_ORG_ID/owners",
      //     "secrets": "/api/v2/orgs/INFLUX_ORG_ID/secrets",
      //     "self": "/api/v2/orgs/INFLUX_ORG_ID",
      //     "tasks": "/api/v2/tasks?org=INFLUX_ORG"
      //   },
      //   "name": "INFLUX_ORG",
      //   "updatedAt": "2022-08-24T23:05:52.881318Z"
      // }


// Organisation::Data::~Data() {
//   delete [] id;
//   delete [] name;
// }

// const char *toStringTmplt PROGMEM = "Organisation: ID %s, Name %s, expire %u";
// String Organisation::toString() const {
//   int len = strlen_P(toStringTmplt) + (_data?strlen(_data->name):0) + (_data?strlen(_data->id):0) + 10 + 1; //10 is maximum length of string representation of expire
//   char *buff = new char[len];
//   sprintf_P(buff, toStringTmplt, getID(), getName(), getExpire());
//   String ret = buff;
//   return ret;
// }

OrganisationsClient::OrganisationsClient() {
  _data = nullptr;
}

OrganisationsClient::OrganisationsClient(ConnectionInfo *pConnInfo, HTTPService *service) {
  _data = std::make_shared<Data>(pConnInfo, service);
}

OrganisationsClient::OrganisationsClient(const OrganisationsClient &other) {
  _data = other._data;
}

OrganisationsClient &OrganisationsClient::operator=(const OrganisationsClient &other) {
  if(this != &other) {
        _data = other._data;
    }
    return *this;
}

OrganisationsClient &OrganisationsClient::operator=(std::nullptr_t) {
  _data = nullptr;
  return *this;
}

// String OrganisationsClient::getOrgID(const char *org) {
//   if(!_data) {
//     return "";
//   }
//   if(isValidID(org)) {
//     return org;
//   }
//   String url = _data->pService->getServerAPIURL();
//   url += "orgs?org=";
//   url += urlEncode(org);
//   String id;
//   INFLUXDB_CLIENT_DEBUG("[D] getOrgID: url %s\n", url.c_str());
//   _data->pService->doGET(url.c_str(), 200, [&id](HTTPClient *client){
//     id = findProperty("id",client->getString());
//     return true;
//   });
//   return id;
// }

// bool OrganisationsClient::checkOrganisationExists(const char *orgName) {
//   Organisation b = findOrganisation(orgName);
//   return !b.isNull();
// }

static const char *CreateOrganisationTemplate PROGMEM = "{\"name\":\"%s\",\"description\":\"%s\"}";

// /api/v2/orgs

// Organisation OrganisationsClient::createOrganisation(const char *orgName, uint32_t expiresSec) {
Organisation OrganisationsClient::createOrganisation(const char *orgName) {
  Organisation b;
  if(_data) {
    // String orgID = getOrgID(_data->pConnInfo->org.c_str());
    String description = "";
    
    // if(!orgID.length()) {
    //   return b;
    // }

    // int expireLen = 0;
    // uint32_t e = expiresSec; 
    // do {
    //   expireLen++;
    //   e /=10;
    // } while(e > 0);

    // int len = strlen_P(CreateOrganisationTemplate) + strlen(orgName) + orgID.length() + expireLen+1;
    int len = strlen_P(CreateOrganisationTemplate) + strlen(orgName);
    
    char *body = new char[len];

    // sprintf_P(body, CreateOrganisationTemplate, orgName, orgID.c_str(), expiresSec);
    sprintf_P(body, CreateOrganisationTemplate, orgName, description);

    String url = _data->pService->getServerAPIURL();
    url += "orgs";
    INFLUXDB_CLIENT_DEBUG("[D] CreateOrganisation: url %s, body %s\n", url.c_str(), body);
    
    _data->pService->doPOST(url.c_str(), body, "application/json", 201, [&b](HTTPClient *client){
      
      String resp = client->getString();

        String id = findProperty("id", resp);
        String name = findProperty("name", resp);
        // String expireStr = findProperty("everySeconds", resp, PropType::Number);

          // uint32_t expire = strtoul(expireStr.c_str(), nullptr, 10);
      
      // b = Organisation(id.c_str(), name.c_str(), expire);
      b = Organisation(id.c_str(), name.c_str());
      return true;
    
    });
    delete [] body;
  }
  return b;
}

// bool OrganisationsClient::deleteOrganisation(const char *id) {
//   if(!_data) {
    
//     return false;
//   }
//   String url = _data->pService->getServerAPIURL();
//   url += "org/";
//   url += id;
//   INFLUXDB_CLIENT_DEBUG("[D] deleteOrganisation: url %s\n", url.c_str());
//   return _data->pService->doDELETE(url.c_str(), 204, nullptr);
// }

// Organisation OrganisationsClient::findOrganisation(const char *orgName) {
//   Organisation b;
//   if(_data) {
//     String url = _data->pService->getServerAPIURL();
//     url += "org?name=";
//     url += urlEncode(orgName);
//     INFLUXDB_CLIENT_DEBUG("[D] findOrganisation: url %s\n", url.c_str());
//     _data->pService->doGET(url.c_str(), 200, [&b](HTTPClient *client){
//       String resp = client->getString();
//       String id = findProperty("id", resp);
//       if(id.length()) {
//         String name = findProperty("name", resp);
//         String expireStr = findProperty("everySeconds", resp, PropType::Number);
//         uint32_t expire = strtoul(expireStr.c_str(), nullptr, 10);
//         b = Organisation(id.c_str(), name.c_str(), expire);
//       }
//       return true;
//     });
//   }
//   return b;
// }
