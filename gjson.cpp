#include "gjson.h"

void Profile::Initialize(std::string filename) {
	fileName = filename;
}

bool Profile::TestJson() {
	std::ifstream inJson(fileName, std::ios::binary);
	if (!inJson.is_open()) {
		inJson.close();
		return false;
	}
	inJson.close();
	return true;
}
bool Profile::CreateJson() {
	std::ofstream create(fileName);
	if (create.is_open()) {
		create.close();
		return true;
	}
	create.close();
	return false;
}

bool Profile::MakeJson() {
	std::ifstream inJson(fileName, std::ios::binary);
	if (!inJson.is_open()) {
		inJson.close();
		return false;
	}
	Json::Reader reader;
	Json::Value fJson;
	if (reader.parse(inJson, fJson)) fJson["update"] = true;
	inJson.close();
	std::ofstream outJson(fileName, std::ios_base::out);
	outJson << fJson.toStyledString();
	outJson.close();
	return false;
}
bool Profile::MakeJson(std::string item, std::string value) {
	Json::Reader reader;
	Json::Value fJson;

	std::ifstream inJson(fileName, std::ios::binary);
	if (!inJson.is_open()) {
		inJson.close();
		return false;
	}
	reader.parse(inJson, fJson);

	if (item.find('/') != -1) {
		std::string sLine;
		std::vector<std::string> sItem;
		std::stringstream ssItem(item);
		while (getline(ssItem, sLine, '/')) sItem.push_back(sLine);

		std::vector<Json::Value> v_tJson;

		v_tJson.push_back(fJson[sItem[0]]);
		for (int i = 1; i < sItem.size() - 1; i += 1) v_tJson.push_back(v_tJson[i - 1][sItem[i]]);

		v_tJson[sItem.size() - 2][sItem[sItem.size() - 1]] = value;

		for (int i = v_tJson.size() - 2; i >= 0; i -= 1) v_tJson[i][sItem[i + 1]] = v_tJson[i + 1];
		fJson[sItem[0]] = v_tJson[0];
	}
	else fJson[item] = value;

	std::ofstream outJson(fileName, std::ios_base::out);
	outJson << fJson.toStyledString();
	outJson.close();

	inJson.close();
	return true;
}
bool Profile::MakeJson(std::string item, bool value) {
	Json::Reader reader;
	Json::Value fJson;

	std::ifstream inJson(fileName, std::ios::binary);
	if (!inJson.is_open()) {
		inJson.close();
		return false;
	}
	reader.parse(inJson, fJson);

	if (item.find('/') != -1) {
		std::string sLine;
		std::vector<std::string> sItem;
		std::stringstream ssItem(item);
		while (getline(ssItem, sLine, '/')) sItem.push_back(sLine);

		std::vector<Json::Value> v_tJson;

		v_tJson.push_back(fJson[sItem[0]]);
		for (int i = 1; i < sItem.size() - 1; i += 1) v_tJson.push_back(v_tJson[i - 1][sItem[i]]);

		v_tJson[sItem.size() - 2][sItem[sItem.size() - 1]] = value;

		for (int i = v_tJson.size() - 2; i >= 0; i -= 1) v_tJson[i][sItem[i + 1]] = v_tJson[i + 1];
		fJson[sItem[0]] = v_tJson[0];
	}
	else fJson[item] = value;

	std::ofstream outJson(fileName, std::ios_base::out);
	outJson << fJson.toStyledString();
	outJson.close();

	inJson.close();
	return true;
}
bool Profile::MakeJson(std::string item, int value) {
	Json::Reader reader;
	Json::Value fJson;

	std::ifstream inJson(fileName, std::ios::binary);
	if (!inJson.is_open()) {
		inJson.close();
		return false;
	}
	reader.parse(inJson, fJson);

	if (item.find('/') != -1) {
		std::string sLine;
		std::vector<std::string> sItem;
		std::stringstream ssItem(item);
		while (getline(ssItem, sLine, '/')) sItem.push_back(sLine);

		std::vector<Json::Value> v_tJson;

		v_tJson.push_back(fJson[sItem[0]]);
		for (int i = 1; i < sItem.size() - 1; i += 1) v_tJson.push_back(v_tJson[i - 1][sItem[i]]);

		v_tJson[sItem.size() - 2][sItem[sItem.size() - 1]] = value;

		for (int i = v_tJson.size() - 2; i >= 0; i -= 1) v_tJson[i][sItem[i + 1]] = v_tJson[i + 1];
		fJson[sItem[0]] = v_tJson[0];
	}
	else fJson[item] = value;

	std::ofstream outJson(fileName, std::ios_base::out);
	outJson << fJson.toStyledString();
	outJson.close();

	inJson.close();
	return true;
}

void Profile::WriteJson(Json::Value& json) {
	std::ofstream outJson(fileName, std::ios_base::out);
	outJson.clear();
	outJson << json.toStyledString();
	outJson.close();
}

bool Profile::LoadJson(std::string& content, std::string item) {
	std::string sLine;
	std::vector<std::string> sItem;
	std::stringstream ssItem(item);
	
	Json::Reader reader;
	Json::Value fJson;

	std::ifstream inJson(fileName, std::ios::binary);
	if (!inJson.is_open()) {
		inJson.close();
		return false;
	}

	if (item.find('/') != -1) {
		while (getline(ssItem, sLine, '/')) sItem.push_back(sLine);

		if (reader.parse(inJson, fJson)) {
			Json::Value finalJson;
			finalJson = fJson[sItem[0]];
			if (sItem.size() >= 2) for (int i = 1; i < sItem.size() - 1; i += 1) finalJson = finalJson[sItem[i]];
			content = finalJson[sItem[sItem.size() - 1]].asString();
		}
	}
	else if (reader.parse(inJson, fJson)) content = fJson[item].asString();
	inJson.close();
	return true;
}
bool Profile::LoadJson(bool& content, std::string item) {
	std::string sLine;
	std::vector<std::string> sItem;
	std::stringstream ssItem(item);

	Json::Reader reader;
	Json::Value fJson;

	std::ifstream inJson(fileName, std::ios::binary);
	if (!inJson.is_open()) {
		inJson.close();
		return false;
	}

	if (item.find('/') != -1) {
		while (getline(ssItem, sLine, '/')) sItem.push_back(sLine);

		if (reader.parse(inJson, fJson)) {
			Json::Value finalJson;
			finalJson = fJson[sItem[0]];
			if (sItem.size() >= 2) for (int i = 1; i < sItem.size() - 1; i += 1) finalJson = finalJson[sItem[i]];
			content = finalJson[sItem[sItem.size() - 1]].asBool();
		}
	}
	else if (reader.parse(inJson, fJson)) content = fJson[item].asBool();
	inJson.close();
	return true;
}
bool Profile::LoadJson(int& content, std::string item) {
	std::string sLine;
	std::vector<std::string> sItem;
	std::stringstream ssItem(item);

	Json::Reader reader;
	Json::Value fJson;

	std::ifstream inJson(fileName, std::ios::binary);
	if (!inJson.is_open()) {
		inJson.close();
		return false;
	}

	if (item.find('/') != -1) {
		while (getline(ssItem, sLine, '/')) sItem.push_back(sLine);

		if (reader.parse(inJson, fJson)) {
			Json::Value finalJson;
			finalJson = fJson[sItem[0]];
			if (sItem.size() >= 2) for (int i = 1; i < sItem.size() - 1; i += 1) finalJson = finalJson[sItem[i]];
			content = finalJson[sItem[sItem.size() - 1]].asInt();
		}
	}
	else if (reader.parse(inJson, fJson)) content = fJson[item].asInt();
	inJson.close();
	return true;
}

bool Profile::LoadJsonToValue(Json::Value& json) {
	Json::Reader reader;
	Json::Value fJson;

	std::ifstream inJson(fileName, std::ios::binary);
	if (!inJson.is_open()) {
		inJson.close();
		return false;
	}
	reader.parse(inJson, json);
	return true;
}

bool LoadJsonToValue(std::string fileName, Json::Value& json) {
	Json::Reader reader;
	Json::Value fJson;

	std::ifstream inJson(fileName, std::ios::binary);
	if (!inJson.is_open()) {
		inJson.close();
		return false;
	}
	reader.parse(inJson, json);
	return true;
}