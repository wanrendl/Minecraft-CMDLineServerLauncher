#ifndef _GJSON_H_
#define _GJSON_H_

#include <fstream>
#include <iostream>

#include <json/json.h>

#pragma comment(lib, "jsoncpp.lib")

class Profile {
private:
	std::string fileName;
public:
	void Initialize(std::string filename);
	bool TestJson();
	bool CreateJson();

	bool MakeJson();
	bool MakeJson(std::string item, std::string value);
	bool MakeJson(std::string item, bool value);
	bool MakeJson(std::string item, int value);

	void WriteJson(Json::Value& json);

	bool LoadJson(std::string& content, std::string item);
	bool LoadJson(bool& content, std::string item);
	bool LoadJson(int& content, std::string item);

	bool LoadJsonToValue(Json::Value& json);
};

bool LoadJsonToValue(std::string fileName, Json::Value& json);

#endif