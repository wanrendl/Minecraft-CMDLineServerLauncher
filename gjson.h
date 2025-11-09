#ifndef _GJSON_H_
#define _GJSON_H_

#include <fstream>
#include <iostream>

#include <json/json.h>

#pragma comment(lib, "jsoncpp.lib")

using namespace std;

bool TestJson(string fileName);
bool CreateJson(string fileName);

bool MakeJson(string fileName);
bool MakeJson(string fileName, string item, string value);
bool MakeJson(string fileName, string item, bool value);
bool MakeJson(string fileName, string item, int value);

void WriteJson(string fileName, Json::Value json);

bool LoadJson(string fileName, string& content, string item);
bool LoadJson(string fileName, bool& content, string item);
bool LoadJson(string fileName, int& content, string item);

bool LoadJsonToValue(string filename, Json::Value& json);

#endif