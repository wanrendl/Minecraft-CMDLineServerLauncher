#include "basics.h"
#include "curldownload.h"
#include "gjson.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <direct.h>
#include <filesystem>

string version_manifest_url = "https://launchermeta.mojang.com/mc/game/version_manifest.json";
string version_manifest_path = "manifests/version_manifest.json";

string selected_server, selected_server_path, selected_server_version, selected_server_java_path;
string selected_client, selected_client_path, selected_client_version;

bool do_server_selected = false, do_client_selected = false;

using namespace std;

int main() {
	//string version = "Minecraft CMDLine Server Launcher: v20250720-133100";
	//string version = "Minecraft CMDLine Server Launcher: v20250721-085700";
	//string version = "Minecraft CMDLine Server Launcher: v20250725-203300";
	//string version = "Minecraft CMDLine Server Launcher: v20250726-235700";
	string version = "Minecraft CMDLine Server Launcher: v20250815-164900";
	cout << GetLogger(LOGINFO) << "Version: " << version << endl;

	_cDeviceInfo CdInfo;
	CdInfo.getAllInfo();

	cout << GetLogger(LOGINFO) << "Memory: " << fixed << setprecision(1) << CdInfo.dInfo.MEMORY_available_gb << "GB/"
		<< CdInfo.dInfo.MEMORY_total_gb << "GB ("
		<< 100 * (double)CdInfo.dInfo.MEMORY_available_byte / (double)CdInfo.dInfo.MEMORY_total_byte
		<< "% available)." << endl;

	cout << GetLogger(LOGINFO) << "CPU Architecture: ";

	if (CdInfo.dInfo.CPU_architecture == X86) cout << "x86" << endl;
	else if (CdInfo.dInfo.CPU_architecture == AMD64) cout << "x86-64/AMD64" << endl;
	else if (CdInfo.dInfo.CPU_architecture == ARM64) cout << "ARM64" << endl;
	else cout << "UNKNOWN." << endl;

	cout << endl;

	MakeJson("profile.json");

	if (!TestJson("profile.json")) CreateJson("profile.json");
	bool isup_test;
	LoadJson("profile.json", isup_test, "frp/passkey_set");
	if (!isup_test) {
		MakeJson("profile.json", "frp/passkey", string(""));
		MakeJson("profile.json", "frp/passkey_set", false);
	}

	string frp_userid;
	bool frp_userid_set = false;

	vector<string> command;
	string cmdPart;
	while (true) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
		cout << "Anonymous> ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
		char fchar = cin.get();
		if (fchar != '\n') {
			while (cin >> cmdPart) {
				command.push_back(utol(cmdPart));
				if (cin.get() == '\n') break;
			}
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			
			int args = command.size();
			
			//cout << "args: " << args << endl;

			command[0] = fchar + command[0];

			if (command[0] == "frp") {
				if (command[1] == "key") {
					if (command[2] == "query") {
						if (frp_userid_set == false) cout << GetLogger(WARN) << "frp Passkey Unset." << endl;
						else {
							cout << GetLogger(WARN) << "DO NOT SHOW THE PASSKEY TO ANYONE." << endl;
							cout << GetLogger(LOGINFO) << "frp Passkey: \"" << frp_userid << "\"" << endl;
						}
					}
					else if (command[2] == "set") {
						frp_userid = command[3];
						frp_userid_set = true;
						cout << GetLogger(WARN) << "DO NOT SHOW THE PASSKEY TO ANYONE." << endl;
						cout << GetLogger(LOGINFO) << "frp Passkey Set." << endl;
					}
					else if (command[2] == "reset") {
						frp_userid.clear();
						frp_userid_set = false;
						cout << GetLogger(WARN) << "DO NOT SHOW THE PASSKEY TO ANYONE." << endl;
						cout << GetLogger(LOGINFO) << "frp Passkey Reset." << endl;
					}
					else if (command[2] == "profile") {
						if (command[3] == "save") {
							if (frp_userid_set == false) cout << GetLogger(WARN) << "frp Passkey Unset." << endl;
							else {
								MakeJson("profile.json", "frp/passkey", frp_userid);
								MakeJson("profile.json", "frp/passkey_set", frp_userid_set);
								cout << GetLogger(LOGINFO) << "frp Profile Saved." << endl;
							}
						}
						else if (command[3] == "load") {
							LoadJson("profile.json", isup_test, "frp/passkey_set");
							if (!isup_test) cout << GetLogger(WARN) << "frp Profile Passkey Not Saved." << endl;
							else {
								LoadJson("profile.json", frp_userid, "frp/passkey");
								frp_userid_set = true;
								cout << GetLogger(LOGINFO) << "frp Profile Loaded." << endl;
							}
						}
						else cout << GetLogger(WARN) << "No such command." << endl;
					}
					else cout << GetLogger(WARN) << "No such command." << endl;
				}
				else cout << GetLogger(WARN) << "No such command." << endl;
			}
			else if (command[0] == "server" || command[0] == "client") {
				if (file_exists(version_manifest_path)) {

					Json::Value version_manifest;
					LoadJsonToValue(version_manifest_path, version_manifest);
					string latest_release, latest_snapshot;
					latest_release = version_manifest["latest"]["release"].asString();
					latest_snapshot = version_manifest["latest"]["snapshot"].asString();
					
					cout << GetLogger(LOGINFO) << "Latest Release: " << latest_release << ", Latest Snapshot: " << latest_snapshot << endl;

					if (do_server_selected) cout << GetLogger(LOGINFO) << "Current Selected Server: " << selected_server << ", Version: " << selected_server_version << endl;
					if (do_client_selected) cout << GetLogger(LOGINFO) << "Current Selected Client: " << selected_client << ", Version: " << selected_client_version << endl;

					if (command[1] == "download") {
						if (command[2] == "officialversion" && command[0] == "server") {
							if (args <= 3) cout << GetLogger(WARN) << "Version?" << endl;
							else if (do_server_selected) {
								string version = command[3];
								cout << GetLogger(LOGINFO) << "Download version: " << version << endl;

								bool dofound = false;
								if (!file_exists(selected_server_path + "/server.jar")) {
									for (int i = 0; i < version_manifest["versions"].size(); i += 1) {
										if (version == version_manifest["versions"][i]["id"].asString()) {
											cout << GetLogger(LOGINFO) << "Version found. Downloading spacific manifest." << endl;
											filesystem::create_directories("./manifests/versions");
											if (!file_exists("manifests/versions/" + version + ".json"))
												gDownload(version_manifest["versions"][i]["url"].asString(), "manifests/versions/" + version + ".json");
											else cout << GetLogger(LOGINFO) << "Version Manifest: \"" << version << ".json\" existed." << endl;
											dofound = true;
										}
									}
									if (dofound) {
										Json::Value spacific_version_manifest;
										LoadJsonToValue("manifests/versions/" + version + ".json", spacific_version_manifest);
										filesystem::create_directories("./server/" + selected_server);
										gDownload(spacific_version_manifest["downloads"]["server"]["url"].asString(), selected_server_path + "/server.jar");
										MakeJson("profile.json", "serverlist/" + selected_server + "/version", version);
										MakeJson("profile.json", "serverlist/" + selected_server + "/version_exist", true);
										MakeJson("profile.json", "serverlist/" + selected_server + "/java_version", "default");
										selected_server_version = version;
									}
									else cout << GetLogger(WARN) << "Version not found." << endl;
								}
								else cout << GetLogger(WARN) << "Server Core (server.jar) Existed." << endl;
							}
							else cout << GetLogger(WARN) << "Server not selected." << endl;
						}
						else if (command[2] == "officialversion" && command[0] == "client") {
							if (args <= 3) cout << GetLogger(WARN) << "Version?" << endl;
							else if (do_client_selected) {
								string version = command[3];
								cout << GetLogger(LOGINFO) << "Download version: " << version << endl;

								bool dofound = false;
								if (!file_exists(selected_server_path + "/client.jar")) {
									for (int i = 0; i < version_manifest["versions"].size(); i += 1) {
										if (version == version_manifest["versions"][i]["id"].asString()) {
											cout << GetLogger(LOGINFO) << "Version found. Downloading spacific manifest." << endl;
											if (!file_exists("manifests/versions/" + version + ".json"))
												gDownload(version_manifest["versions"][i]["url"].asString(), "manifests/versions/" + version + ".json");
											else cout << GetLogger(LOGINFO) << "Version Manifest: \"" << version << ".json\" existed." << endl;
											dofound = true;
										}
									}
									if (dofound) {
										Json::Value spacific_version_manifest;
										LoadJsonToValue("manifests/versions/" + version + ".json", spacific_version_manifest);
										gDownload(spacific_version_manifest["downloads"]["client"]["url"].asString(), selected_client_path + "/client.jar");
										MakeJson("profile.json", "clientlist/" + selected_client + "/version", version);
										MakeJson("profile.json", "clientlist/" + selected_client + "/version_exist", true);
										selected_client_version = version;
									}
									else cout << GetLogger(WARN) << "Version not found." << endl;
								}
								else cout << GetLogger(WARN) << "Client Core (client.jar) Existed." << endl;
							}
							else cout << GetLogger(WARN) << "Client not selected." << endl;
						}
						else if (command[2] == "manifest") {
							if (command[3] == "all") {
								if (args >= 5 && (command[4] == "release" || command[4] == "snapshot")) {
									cout << GetLogger(LOGINFO) << "Downloading " << command[4] << " versions..." << endl;
									string version;
									for (int i = 0; i < version_manifest["versions"].size(); i += 1) {
										if (version_manifest["versions"][i]["type"].asString() == command[4]) {
											version = version_manifest["versions"][i]["id"].asString();
											if (!file_exists("manifests/versions/" + version + ".json"))
												gDownload(version_manifest["versions"][i]["url"].asString(), "manifests/versions/" + version + ".json");
											else cout << GetLogger(LOGINFO) << "Version: \"" << version << "\" existed." << endl;
										}
									}
								}
								else if (args < 5) {
									cout << GetLogger(LOGINFO) << "Downloading all versions..." << endl;
									string version;
									for (int i = 0; i < version_manifest["versions"].size(); i += 1) {
										version = version_manifest["versions"][i]["id"].asString();
										if (!file_exists("manifests/versions/" + version + ".json"))
											gDownload(version_manifest["versions"][i]["url"].asString(), "manifests/versions/" + version + ".json");
										else cout << GetLogger(LOGINFO) << "Version: \"" << version << "\" existed." << endl;
									}
								}
								else cout << GetLogger(WARN) << "No version type: \"" << command[4] << "\"." << endl;
							}
							else if (args > 3) {
								string version = command[3];

								bool dofound = false;
								for (int i = 0; i < version_manifest["versions"].size(); i += 1) {
									if (version == version_manifest["versions"][i]["id"].asString()) {
										cout << GetLogger(LOGINFO) << "Version found. Downloading spacific manifest." << endl;
										if (!file_exists("manifests/versions/" + version + ".json"))
											gDownload(version_manifest["versions"][i]["url"].asString(), "manifests/versions/" + version + ".json");
										else cout << GetLogger(LOGINFO) << "Version: \"" << version << "\" existed." << endl;
										dofound = true;
									}
								}
								if (!dofound) cout << GetLogger(WARN) << "Version not found." << endl;
							}
							else cout << GetLogger(WARN) << "Version?" << endl;
						}
						else cout << GetLogger(WARN) << "No such command." << endl;
					}
					else if (command[1] == "create" && command[0] == "server") {
						if (args <= 2) cout << GetLogger(WARN) << "ServerID?" << endl;
						
						bool doexist = false;
						string server_create = command[2];

						LoadJson("profile.json", doexist, "serverlist/" + server_create + "/exist");
						if (doexist) cout << GetLogger(WARN) << "Server \"" << server_create << "\" existed." << endl;
						else {
							MakeJson("profile.json", "serverlist/" + server_create + "/path", "./server/" + server_create);
							MakeJson("profile.json", "serverlist/" + server_create + "/exist", true);
							filesystem::create_directories("./server/" + server_create);
							cout << GetLogger(LOGINFO) << "Server: \"" << server_create << "\" created." << endl;
						}
					}
					else if (command[1] == "create" && command[0] == "client") {
						if (args <= 2) cout << GetLogger(WARN) << "ClientID?" << endl;

						bool doexist = false;
						string client_create = command[2];

						LoadJson("profile.json", doexist, "clientlist/" + client_create + "/exist");
						if (doexist) cout << GetLogger(WARN) << "Client \"" << client_create << "\" existed." << endl;
						else {
							MakeJson("profile.json", "clientlist/" + client_create + "/path", "./client/" + client_create);
							MakeJson("profile.json", "clientlist/" + client_create + "/exist", true);
							filesystem::create_directories("./client/" + client_create);
							cout << GetLogger(LOGINFO) << "Client: \"" << client_create << "\" created." << endl;
						}
					}
					else if (command[1] == "select" && command[0] == "server") {
						if (args <= 2) cout << GetLogger(WARN) << "ServerID?" << endl;
						
						bool doexist = false;
						
						LoadJson("profile.json", doexist, "serverlist/" + command[2] + "/exist");
						if (doexist) {
							do_server_selected = true;
							selected_server = command[2];
							LoadJson("profile.json", selected_server_path, "serverlist/" + command[2] + "/path");
							LoadJson("profile.json", selected_server_version, "serverlist/" + command[2] + "/version");
							LoadJson("profile.json", selected_server_java_path, "serverlist/" + command[2] + "/java_version");
							cout << GetLogger(LOGINFO) << "Server: \"" << command[2] << "\" selected." << endl;
						}
						else cout << GetLogger(WARN) << "Server: \"" << command[2] << "\" not exist." << endl;
					}
					else if (command[1] == "select" && command[0] == "client") {
						if (args <= 2) cout << GetLogger(WARN) << "ClientID?" << endl;

						bool doexist = false;

						LoadJson("profile.json", doexist, "clientlist/" + command[2] + "/exist");
						if (doexist) {
							do_client_selected = true;
							selected_client = command[2];
							LoadJson("profile.json", selected_client_path, "clientlist/" + command[2] + "/path");
							LoadJson("profile.json", selected_client_version, "clientlist/" + command[2] + "/version");
							cout << GetLogger(LOGINFO) << "Client: \"" << command[2] << "\" selected." << endl;
						}
						else cout << GetLogger(WARN) << "Client: \"" << command[2] << "\" not exist." << endl;
					}
					else if (command[1] == "unload" && command[0] == "server") {
						do_server_selected = false;
						selected_server_path.clear();
						selected_server.clear();
					}
					else if (command[1] == "unload" && command[0] == "client") {
						do_client_selected = false;
						selected_client_path.clear();
						selected_client.clear();
					}
					else if (command[1] == "list" && command[0] == "server") {
						Json::Value serverList;
						LoadJsonToValue("profile.json", serverList);
						int count = 0;
						for (auto iterator = serverList["serverlist"].begin(); iterator != serverList["serverlist"].end(); ++iterator) {
							count += 1;
							cout << GetLogger(LOGINFO) << "Server " << count << ": \"" << iterator.name() << "\"." << endl;
						}
					}
					else if (command[1] == "list" && command[0] == "client") {
						Json::Value clientList;
						LoadJsonToValue("profile.json", clientList);
						int count = 0;
						for (auto iterator = clientList["clientlist"].begin(); iterator != clientList["clientlist"].end(); ++iterator) {
							count += 1;
							cout << GetLogger(LOGINFO) << "Client " << count << ": \"" << iterator.name() << "\"." << endl;
						}
					}
					else if (command[1] == "delete" && command[0] == "server") {
						if (args <= 2) cout << GetLogger(WARN) << "ServerID?" << endl;
						else {
							Json::Value profile;
							LoadJsonToValue("profile.json", profile);
							bool dofound = false;
							for (auto iterator = profile["serverlist"].begin(); iterator != profile["serverlist"].end(); ++iterator) {
								if (command[2] == iterator.name()) {
									dofound = true;
									filesystem::remove_all(profile["serverlist"][iterator.name()]["path"].asString());
									profile["serverlist"].removeMember(command[2]);
									WriteJson("profile.json", profile);
									cout << GetLogger(LOGINFO) << "Server Deleted." << endl;
									break;
								}
							}
							if (!dofound) cout << GetLogger(WARN) << "Server: \"" << command[2] << "\" Not Found." << endl;
						}
					}
					else if (command[1] == "delete" && command[0] == "client") {
						if (args <= 2) cout << GetLogger(WARN) << "ClientID?" << endl;
						else {
							Json::Value profile;
							LoadJsonToValue("profile.json", profile);
							bool dofound = false;
							for (auto iterator = profile["clientlist"].begin(); iterator != profile["clientlist"].end(); ++iterator) {
								if (command[2] == iterator.name()) {
									dofound = true;
									filesystem::remove_all(profile["clientlist"][iterator.name()]["path"].asString());
									profile["clientlist"].removeMember(command[2]);
									WriteJson("profile.json", profile);
									cout << GetLogger(LOGINFO) << "Client Deleted." << endl;
									break;
								}
							}
							if (!dofound) cout << GetLogger(WARN) << "Client: \"" << command[2] << "\" Not Found." << endl;
						}
					}
					else if (command[1] == "java64" && command[0] == "server") {
						if (command[2] == "select" && do_server_selected && args == 4) {
							string javaVersion = command[3];
							Json::Value javaListJson;
							LoadJsonToValue("profile.json", javaListJson);
							for (int i = 0; i < javaListJson["java"]["x64"].size(); i += 1) {
								if (javaListJson["java"]["x64"][i]["name"].asString() == command[3]) {
									selected_server_java_path = javaListJson["java"]["x64"][i]["path"].asString();
									cout << GetLogger(LOGINFO) << "Java Selected." << endl;
									break;
								}
							}
							MakeJson("profile.json", "serverlist/" + selected_server + "/java_version", selected_server_java_path);
						}
					}
					else if (command[1] == "make" && command[0] == "server" && do_server_selected) {
						basicSetup(selected_server_path, "server.jar", selected_server_java_path + "\\bin\\");
						argumentSetup(selected_server, CdInfo.dInfo.CPU_core_num);
						cout << GetLogger(LOGINFO) << "Server File Make" << endl;
					}
					else if (command[1] == "core" && command[0] == "server") {
						if (args <= 2) cout << GetLogger(WARN) << "Core?" << endl;
						else {

						}
					}
					else if (command[1] == "argument" && command[0] == "server") {
						if (command[2] == "memory") {
							if (command[3] == "maxgb" && do_server_selected) {
								if (args <= 4) cout << GetLogger(WARN) << "Memory?" << endl;
								else if (isDigitString(command[4])) {
									MakeJson("profile.json", "serverlist/" + selected_server + "/jvmargs/maxmem", stoi(command[4]));
									MakeJson("profile.json", "serverlist/" + selected_server + "/jvmargs/maxmemu", string("G"));
									cout << GetLogger(LOGINFO) << "Maximum Memory Set to: " << command[4] << "GB" << endl;
								}
								else cout << GetLogger(WARN) << "Memory CANT be Character." << endl;
							}
							else if (command[3] == "mingb" && do_server_selected) {
								if (args <= 4) cout << GetLogger(WARN) << "Memory?" << endl;
								else if (isDigitString(command[4])) {
									MakeJson("profile.json", "serverlist/" + selected_server + "/jvmargs/minmem", stoi(command[4]));
									MakeJson("profile.json", "serverlist/" + selected_server + "/jvmargs/minmemu", string("G"));
									cout << GetLogger(LOGINFO) << "Minimum Memory Set to: " << command[4] << "GB" << endl;
								}
								else cout << GetLogger(WARN) << "Memory CANT be Character." << endl;
							}
							else if (command[3] == "maxmb" && do_server_selected) {
								if (args <= 4) cout << GetLogger(WARN) << "Memory?" << endl;
								else if (isDigitString(command[4])) {
									MakeJson("profile.json", "serverlist/" + selected_server + "/jvmargs/maxmem", stoi(command[4]));
									MakeJson("profile.json", "serverlist/" + selected_server + "/jvmargs/maxmemu", string("M"));
									cout << GetLogger(LOGINFO) << "Maximum Memory Set to: " << command[4] << "MB" << endl;
								}
								else cout << GetLogger(WARN) << "Memory CANT be Character." << endl;
							}
							else if (command[3] == "minmb" && do_server_selected) {
								if (args <= 4) cout << GetLogger(WARN) << "Memory?" << endl;
								else if (isDigitString(command[4])) {
									MakeJson("profile.json", "serverlist/" + selected_server + "/jvmargs/minmem", stoi(command[4]));
									MakeJson("profile.json", "serverlist/" + selected_server + "/jvmargs/minmemu", string("M"));
									cout << GetLogger(LOGINFO) << "Minimum Memory Set to: " << command[4] << "MB" << endl;
								}
								else cout << GetLogger(WARN) << "Memory CANT be Character." << endl;
							}
							else if (command[3] == "query") {
								CdInfo.updateMemoryInfo();
								cout << GetLogger(LOGINFO) << "Current Available Memory: " << CdInfo.dInfo.MEMORY_available_gb << "GB." << endl;
								if (0.8 * CdInfo.dInfo.MEMORY_available_gb >= 8.0)
									cout << GetLogger(LOGINFO) << "Memory Recommand: >8GB" << endl;
								else if (0.8 * CdInfo.dInfo.MEMORY_available_gb <= 4.0)
									cout << GetLogger(WARN) << "Memory is too low, please clear it before setting." << endl;
								else cout << GetLogger(LOGINFO) << "Memory Recommand: 4GB - 8GB" << endl;
							}
							else cout << GetLogger(WARN) << "No such command." << endl;
						}
					}
					else if (command[1] == "manifest" && command[2] == "update") {
						gDownload(version_manifest_url, version_manifest_path);
						cout << GetLogger(LOGINFO) << "Manifest Updated." << endl;
					}
					else cout << GetLogger(WARN) << "No such command." << endl;

				}
				else {
					cout << GetLogger(WARN) << "File \"version_manifest.json\" not exists. Downloading..." << endl;
					filesystem::create_directories("./manifests/");
					gDownload(version_manifest_url, version_manifest_path);
				}
			}
			else if (command[0] == "java") {
				if (command[1] == "list") {
					Json::Value profile;
					LoadJsonToValue("profile.json", profile);

					cout << GetLogger(LOGINFO) << "x64: ";
					for (int i = 0; i < profile["java"]["x64"].size(); i += 1) cout << profile["java"]["x64"][i]["name"].asString() << " ";
					cout << endl;

					cout << GetLogger(LOGINFO) << "x86: ";
					for (int i = 0; i < profile["java"]["x86"].size(); i += 1) cout << profile["java"]["x86"][i]["name"].asString() << " ";
					cout << endl;
					
					profile.clear();
				}
				else if (command[1] == "update") {
					vector<Json::Value> javaVersion64, javaVersion32;
					Json::Value temp;
					string path64 = "C:\\Program Files\\Java", path32 = "C:\\Program Files (x86)\\Java", javaPath;
					for (const auto& entry : filesystem::directory_iterator(path64)) {
						javaPath = entry.path().string();
						if (javaPath.find("jdk") != (size_t)-1) {
							temp["name"] = javaPath.substr(javaPath.find("jdk"), javaPath.size());
							temp["path"] = javaPath;
							javaVersion64.push_back(temp);
						}
					}
					for (const auto& entry : filesystem::directory_iterator(path32)) {
						javaPath = entry.path().string();
						if (javaPath.find("jdk") != (size_t)-1) {
							temp["name"] = javaPath.substr(javaPath.find("jdk"), javaPath.size());
							temp["path"] = javaPath;
							javaVersion32.push_back(temp);
						}
					}
					Json::Value profile;
					LoadJsonToValue("profile.json", profile);
					profile["java"].clear();
					for (int i = 0; i < javaVersion64.size(); i += 1) profile["java"]["x64"].append(javaVersion64[i]);
					for (int i = 0; i < javaVersion32.size(); i += 1) profile["java"]["x86"].append(javaVersion32[i]);
					WriteJson("profile.json", profile);
					profile.clear();
					cout << GetLogger(LOGINFO) << "Java Version Updated." << endl;
				}
				else cout << GetLogger(WARN) << "No such command." << endl;
			}
			else if (command[0] == "download" && args >= 4) {
				string url = command[1], filepath = command[2], filename = command[3];
				filesystem::create_directories(filepath);
				gDownload(url, filepath + "/" + filename, true);
				cout << GetLogger(LOGINFO) << "Downloaded." << endl;
			}
			else if (command[0] == "quit" || command[0] == "exit") break;
			else cout << GetLogger(WARN) << "No such command." << endl;

			command.clear();
		}
	}
	cout << GetLogger(LOGINFO) << "Bye~ :D" << endl;
	return 0;
}