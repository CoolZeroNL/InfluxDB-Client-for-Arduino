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

Organisation::Organisation(const char *id, const char *name) {
  _data = std::make_shared<Data>(id, name);
}

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


Organisation::Data::Data(const char *id, const char *name) {
  this->id = copyOrgChars(id);
  this->name = copyOrgChars(name);
}

Organisation::Data::~Data() {
  delete [] id;
  delete [] name;
}

const char *toOrgStringTmplt PROGMEM = "Organisation: ID %s, Name %s";

String Organisation::toString() const {
  int len = strlen_P(toOrgStringTmplt) + (_data?strlen(_data->name):0) + (_data?strlen(_data->id):0) + 1;
  char *buff = new char[len];
  sprintf_P(buff, toOrgStringTmplt, getID(), getName());
  String ret = buff;
  return ret;
}

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

bool OrganisationsClient::checkOrganisationExists(const char *orgName) {
  Organisation b = findOrganisation(orgName);
  return !b.isNull();
}

static const char *CreateOrganisationTemplate PROGMEM = "{\"name\":\"%s\",\"description\":\"%s\"}";
Organisation OrganisationsClient::createOrganisation(const char *orgName) {
  Organisation b;
  if(_data) {
    
    String description = "";    // ?
    
    int len = strlen_P(CreateOrganisationTemplate) + description.length() + strlen(orgName) + 1;
    
    char *body = new char[len];
    sprintf_P(body, CreateOrganisationTemplate, orgName, description);
    String url = _data->pService->getServerAPIURL();
    url += "orgs";
    INFLUXDB_CLIENT_DEBUG("[D] CreateOrganisation: url %s, body %s\n", url.c_str(), body);    
    _data->pService->doPOST(url.c_str(), body, "application/json", 201, [&b](HTTPClient *client){
      String resp = client->getString();
      String id = findProperty("id", resp);
      String name = findProperty("name", resp);
      b = Organisation(id.c_str(), name.c_str());
      return true;
    });
    delete [] body;
  }
  return b;
}

bool OrganisationsClient::deleteOrganisation(const char *id) {
  if(!_data) {
    return false;
  }
  String url = _data->pService->getServerAPIURL();
  url += "orgs/";
  url += id;
  INFLUXDB_CLIENT_DEBUG("[D] deleteOrganisation: url %s\n", url.c_str());
  return _data->pService->doDELETE(url.c_str(), 204, nullptr);
}

Organisation OrganisationsClient::findOrganisation(const char *orgName) {
  Organisation b;
  if(_data) {
    String url = _data->pService->getServerAPIURL();
    url += "orgs?org=";
    url += urlEncode(orgName);
    INFLUXDB_CLIENT_DEBUG("[D] findOrganisation: url %s\n", url.c_str());
    _data->pService->doGET(url.c_str(), 200, [&b](HTTPClient *client){
      String resp = client->getString();
      String id = findProperty("id", resp);
      if(id.length()) {
        String name = findProperty("name", resp);
        b = Organisation(id.c_str(), name.c_str());
      }
      return true;
    });
  }
  return b;
}
