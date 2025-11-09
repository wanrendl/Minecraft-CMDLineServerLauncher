#ifndef _GJSON_H_
#define _GJSON_H_

#include <fstream>
#include <iostream>

#include <json/json.h>

#pragma comment(lib, "jsoncpp.lib")


bool TestJson(std::string fileName);
bool CreateJson(std::string fileName);

bool MakeJson(std::string fileName);
bool MakeJson(std::string fileName, std::string item, std::string value);
bool MakeJson(std::string fileName, std::string item, bool value);
bool MakeJson(std::string fileName, std::string item, int value);

void WriteJson(std::string fileName, Json::Value json);

bool LoadJson(std::string fileName, std::string& content, std::string item);
bool LoadJson(std::string fileName, bool& content, std::string item);
bool LoadJson(std::string fileName, int& content, std::string item);

bool LoadJsonToValue(std::string filename, Json::Value& json);

#endif