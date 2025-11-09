#include "gjson.h"

bool TestJson(string fileName) {
	ifstream inJson(fileName, ios::binary);
	if (!inJson.is_open()) {
		inJson.close();
		return false;
	}
	inJson.close();
	return true;
}
bool CreateJson(string fileName) {
	ofstream create(fileName);
	if (create.is_open()) {
		create.close();
		return true;
	}
	create.close();
	return false;
}

bool MakeJson(string fileName) {
	ifstream inJson(fileName, ios::binary);
	if (!inJson.is_open()) {
		inJson.close();
		return false;
	}
	Json::Reader reader;
	Json::Value fJson;
	if (reader.parse(inJson, fJson)) fJson["update"] = true;
	inJson.close();
	ofstream outJson(fileName, ios_base::out);
	outJson << fJson.toStyledString();
	outJson.close();
	return false;
}
bool MakeJson(string fileName, string item, string value) {
	Json::Reader reader;
	Json::Value fJson;

	ifstream inJson(fileName, ios::binary);
	if (!inJson.is_open()) {
		inJson.close();
		return false;
	}
	reader.parse(inJson, fJson);

	if (item.find('/') != -1) {
		string sLine;
		vector<string> sItem;
		stringstream ssItem(item);
		while (getline(ssItem, sLine, '/')) sItem.push_back(sLine);

		vector<Json::Value> v_tJson;

		v_tJson.push_back(fJson[sItem[0]]);
		for (int i = 1; i < sItem.size() - 1; i += 1) v_tJson.push_back(v_tJson[i - 1][sItem[i]]);

		v_tJson[sItem.size() - 2][sItem[sItem.size() - 1]] = value;

		for (int i = v_tJson.size() - 2; i >= 0; i -= 1) v_tJson[i][sItem[i + 1]] = v_tJson[i + 1];
		fJson[sItem[0]] = v_tJson[0];
	}
	else fJson[item] = value;

	ofstream outJson(fileName, ios_base::out);
	outJson << fJson.toStyledString();
	outJson.close();

	inJson.close();
	return true;
}
bool MakeJson(string fileName, string item, bool value) {
	Json::Reader reader;
	Json::Value fJson;

	ifstream inJson(fileName, ios::binary);
	if (!inJson.is_open()) {
		inJson.close();
		return false;
	}
	reader.parse(inJson, fJson);

	if (item.find('/') != -1) {
		string sLine;
		vector<string> sItem;
		stringstream ssItem(item);
		while (getline(ssItem, sLine, '/')) sItem.push_back(sLine);

		vector<Json::Value> v_tJson;

		v_tJson.push_back(fJson[sItem[0]]);
		for (int i = 1; i < sItem.size() - 1; i += 1) v_tJson.push_back(v_tJson[i - 1][sItem[i]]);

		v_tJson[sItem.size() - 2][sItem[sItem.size() - 1]] = value;

		for (int i = v_tJson.size() - 2; i >= 0; i -= 1) v_tJson[i][sItem[i + 1]] = v_tJson[i + 1];
		fJson[sItem[0]] = v_tJson[0];
	}
	else fJson[item] = value;

	ofstream outJson(fileName, ios_base::out);
	outJson << fJson.toStyledString();
	outJson.close();

	inJson.close();
	return true;
}
bool MakeJson(string fileName, string item, int value) {
	Json::Reader reader;
	Json::Value fJson;

	ifstream inJson(fileName, ios::binary);
	if (!inJson.is_open()) {
		inJson.close();
		return false;
	}
	reader.parse(inJson, fJson);

	if (item.find('/') != -1) {
		string sLine;
		vector<string> sItem;
		stringstream ssItem(item);
		while (getline(ssItem, sLine, '/')) sItem.push_back(sLine);

		vector<Json::Value> v_tJson;

		v_tJson.push_back(fJson[sItem[0]]);
		for (int i = 1; i < sItem.size() - 1; i += 1) v_tJson.push_back(v_tJson[i - 1][sItem[i]]);

		v_tJson[sItem.size() - 2][sItem[sItem.size() - 1]] = value;

		for (int i = v_tJson.size() - 2; i >= 0; i -= 1) v_tJson[i][sItem[i + 1]] = v_tJson[i + 1];
		fJson[sItem[0]] = v_tJson[0];
	}
	else fJson[item] = value;

	ofstream outJson(fileName, ios_base::out);
	outJson << fJson.toStyledString();
	outJson.close();

	inJson.close();
	return true;
}

void WriteJson(string fileName, Json::Value json) {
	ofstream outJson(fileName, ios_base::out);
	outJson.clear();
	outJson << json.toStyledString();
	outJson.close();
}

bool LoadJson(string fileName, string& content, string item) {
	string sLine;
	vector<string> sItem;
	stringstream ssItem(item);
	
	Json::Reader reader;
	Json::Value fJson;

	ifstream Json(fileName, ios::binary);
	if (!Json.is_open()) {
		Json.close();
		return false;
	}

	if (item.find('/') != -1) {
		while (getline(ssItem, sLine, '/')) sItem.push_back(sLine);

		if (reader.parse(Json, fJson)) {
			Json::Value finalJson;
			finalJson = fJson[sItem[0]];
			if (sItem.size() >= 2) for (int i = 1; i < sItem.size() - 1; i += 1) finalJson = finalJson[sItem[i]];
			content = finalJson[sItem[sItem.size() - 1]].asString();
		}
	}
	else if (reader.parse(Json, fJson)) content = fJson[item].asString();
	Json.close();
	return true;
}
bool LoadJson(string fileName, bool& content, string item) {
	string sLine;
	vector<string> sItem;
	stringstream ssItem(item);

	Json::Reader reader;
	Json::Value fJson;

	ifstream Json(fileName, ios::binary);
	if (!Json.is_open()) {
		Json.close();
		return false;
	}

	if (item.find('/') != -1) {
		while (getline(ssItem, sLine, '/')) sItem.push_back(sLine);

		if (reader.parse(Json, fJson)) {
			Json::Value finalJson;
			finalJson = fJson[sItem[0]];
			if (sItem.size() >= 2) for (int i = 1; i < sItem.size() - 1; i += 1) finalJson = finalJson[sItem[i]];
			content = finalJson[sItem[sItem.size() - 1]].asBool();
		}
	}
	else if (reader.parse(Json, fJson)) content = fJson[item].asBool();
	Json.close();
	return true;
}
bool LoadJson(string fileName, int& content, string item) {
	string sLine;
	vector<string> sItem;
	stringstream ssItem(item);

	Json::Reader reader;
	Json::Value fJson;

	ifstream Json(fileName, ios::binary);
	if (!Json.is_open()) {
		Json.close();
		return false;
	}

	if (item.find('/') != -1) {
		while (getline(ssItem, sLine, '/')) sItem.push_back(sLine);

		if (reader.parse(Json, fJson)) {
			Json::Value finalJson;
			finalJson = fJson[sItem[0]];
			if (sItem.size() >= 2) for (int i = 1; i < sItem.size() - 1; i += 1) finalJson = finalJson[sItem[i]];
			content = finalJson[sItem[sItem.size() - 1]].asInt();
		}
	}
	else if (reader.parse(Json, fJson)) content = fJson[item].asInt();
	Json.close();
	return true;
}

bool LoadJsonToValue(string filename, Json::Value& json) {
	Json::Reader reader;
	Json::Value fJson;

	ifstream Json(filename, ios::binary);
	if (!Json.is_open()) {
		Json.close();
		return false;
	}
	reader.parse(Json, json);
	return true;
}