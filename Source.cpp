#include "basics.h"
#include "curldownload.h"
#include "gjson.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <direct.h>
#include <filesystem>

std::string version_manifest_url = "https://launchermeta.mojang.com/mc/game/version_manifest.json";
std::string version_manifest_path = "manifests/version_manifest.json";

std::string selected_server, selected_server_path, selected_server_version, selected_server_java_path;

bool do_server_selected = false;

int main() {

	Logger logger;
	std::stringstream LoggerLog;

	Profile profile;
	profile.Initialize("./profile.json");

	std::string version = "Minecraft CMDLine Server Launcher: v20251109-231300";
	LoggerLog << "Version: " << version << std::endl;
	logger.LogINFO(LoggerLog);

	_cDeviceInfo CdInfo;
	CdInfo.getAllInfo();

	LoggerLog << "Memory: " << std::fixed << std::setprecision(1) << CdInfo.dInfo.MEMORY_available_gb << "GB/"
		<< CdInfo.dInfo.MEMORY_total_gb << "GB ("
		<< 100 * (double)CdInfo.dInfo.MEMORY_available_byte / (double)CdInfo.dInfo.MEMORY_total_byte
		<< "% available)." << std::endl;
	logger.LogINFO(LoggerLog);

	LoggerLog << "CPU Architecture: ";
	logger.LogINFO(LoggerLog);

	if (CdInfo.dInfo.CPU_architecture == X86) std::cout << "x86" << std::endl;
	else if (CdInfo.dInfo.CPU_architecture == AMD64) std::cout << "x86-64/AMD64" << std::endl;
	else if (CdInfo.dInfo.CPU_architecture == ARM64) std::cout << "ARM64" << std::endl;
	else std::cout << "UNKNOWN." << std::endl;

	std::cout << std::endl;

	profile.MakeJson();

	if (!profile.TestJson()) profile.CreateJson();
	bool isup_test;
	profile.LoadJson(isup_test, "frp/passkey_set");
	if (!isup_test) {
		profile.MakeJson("frp/passkey", std::string(""));
		profile.MakeJson("frp/passkey_set", false);
	}

	std::string frp_userid;
	bool frp_userid_set = false;

	std::vector<std::string> command;
	std::string cmdPart;
	while (true) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
		std::cout << "Anonymous> ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
		char fchar = std::cin.get();
		if (fchar != '\n') {
			while (std::cin >> cmdPart) {
				command.push_back(utol(cmdPart));
				if (std::cin.get() == '\n') break;
			}
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			
			int args = command.size();
			
			//std::cout << "args: " << args << std::endl;

			command[0] = fchar + command[0];

			if (command[0] == "frp") {
				if (command[1] == "key") {
					if (command[2] == "query") {
						if (frp_userid_set == false) {
							LoggerLog << "frp Passkey Unset." << std::endl;
							logger.LogWARN(LoggerLog);
						}
						else {
							LoggerLog << "DO NOT SHOW THE PASSKEY TO ANYONE." << std::endl;
							LoggerLog << "frp Passkey: \"" << frp_userid << "\"" << std::endl;
							logger.LogINFO(LoggerLog);
						}
					}
					else if (command[2] == "set") {
						frp_userid = command[3];
						frp_userid_set = true;
						LoggerLog << "DO NOT SHOW THE PASSKEY TO ANYONE." << std::endl;
						LoggerLog << "frp Passkey Set." << std::endl;
						logger.LogINFO(LoggerLog);
					}
					else if (command[2] == "reset") {
						frp_userid.clear();
						frp_userid_set = false;
						LoggerLog << "DO NOT SHOW THE PASSKEY TO ANYONE." << std::endl;
						LoggerLog << "frp Passkey Reset." << std::endl;
						logger.LogINFO(LoggerLog);
					}
					else if (command[2] == "profile") {
						if (command[3] == "save") {
							if (frp_userid_set == false) {
								LoggerLog << "frp Passkey Unset." << std::endl;
								logger.LogWARN(LoggerLog);
							}
							else {
								profile.MakeJson("frp/passkey", frp_userid);
								profile.MakeJson("frp/passkey_set", frp_userid_set);
								LoggerLog << "frp Profile Saved." << std::endl;
								logger.LogINFO(LoggerLog);
							}
						}
						else if (command[3] == "load") {
							profile.LoadJson(isup_test, "frp/passkey_set");
							if (!isup_test) {
								LoggerLog << "frp Profile Passkey Not Saved." << std::endl;
								logger.LogWARN(LoggerLog);
							}
							else {
								profile.LoadJson(frp_userid, "frp/passkey");
								frp_userid_set = true;
								LoggerLog << "frp Profile Loaded." << std::endl;
								logger.LogINFO(LoggerLog);
							}
						}
						else {
							LoggerLog << "No such command." << std::endl;
							logger.LogWARN(LoggerLog);
						}
					}
					else {
						LoggerLog << "No such command." << std::endl;
						logger.LogWARN(LoggerLog);
					}
				}
				else {
					LoggerLog << "No such command." << std::endl;
					logger.LogWARN(LoggerLog);
				}
			}
			else if (command[0] == "server") {
				if (file_exists(version_manifest_path)) {

					Json::Value version_manifest;
					LoadJsonToValue(version_manifest_path, version_manifest);
					std::string latest_release, latest_snapshot;
					latest_release = version_manifest["latest"]["release"].asString();
					latest_snapshot = version_manifest["latest"]["snapshot"].asString();
					
					LoggerLog << "Latest Release: " << latest_release << ", Latest Snapshot: " << latest_snapshot << std::endl;
					logger.LogINFO(LoggerLog);

					if (do_server_selected) {
						LoggerLog << "Current Selected Server: " << selected_server << ", Version: " << selected_server_version << std::endl;
						logger.LogINFO(LoggerLog);
					}

					if (command[1] == "download") {
						if (command[2] == "officialversion") {
							if (args <= 3) {
								LoggerLog << "Version?" << std::endl;
								logger.LogWARN(LoggerLog);
							}
							else if (do_server_selected) {
								std::string version = command[3];
								LoggerLog << "Download version: " << version << std::endl;
								logger.LogINFO(LoggerLog);

								bool dofound = false;
								if (!file_exists(selected_server_path + "/server.jar")) {
									for (int i = 0; i < version_manifest["versions"].size(); i += 1) {
										if (version == version_manifest["versions"][i]["id"].asString()) {
											LoggerLog << "Version found. Downloading spacific manifest." << std::endl;
											logger.LogINFO(LoggerLog);
											std::filesystem::create_directories("./manifests/versions");
											if (!file_exists("manifests/versions/" + version + ".json"))
												gDownload(version_manifest["versions"][i]["url"].asString(), "manifests/versions/" + version + ".json");
											else {
												LoggerLog << "Version Manifest: \"" << version << ".json\" existed." << std::endl;
												logger.LogINFO(LoggerLog);
											}
											dofound = true;
										}
									}
									if (dofound) {
										Json::Value spacific_version_manifest;
										LoadJsonToValue("manifests/versions/" + version + ".json", spacific_version_manifest);
										std::filesystem::create_directories("./server/" + selected_server);
										gDownload(spacific_version_manifest["downloads"]["server"]["url"].asString(), selected_server_path + "/server.jar");
										profile.MakeJson("serverlist/" + selected_server + "/version", version);
										profile.MakeJson("serverlist/" + selected_server + "/version_exist", true);
										profile.MakeJson("serverlist/" + selected_server + "/java_version", "default");
										selected_server_version = version;
									}
									else {
										LoggerLog << "Version not found." << std::endl;
										logger.LogWARN(LoggerLog);
									}
								}
								else {
									LoggerLog << "Server Core (server.jar) Existed." << std::endl;
									logger.LogWARN(LoggerLog);
								}
							}
							else {
								LoggerLog << "Server not selected." << std::endl;
								logger.LogWARN(LoggerLog);
							}
						}
						else if (command[2] == "manifest") {
							if (command[3] == "all") {
								if (args >= 5 && (command[4] == "release" || command[4] == "snapshot")) {
									LoggerLog << "Downloading " << command[4] << " versions..." << std::endl;
									logger.LogINFO(LoggerLog);
									std::string version;
									for (int i = 0; i < version_manifest["versions"].size(); i += 1) {
										if (version_manifest["versions"][i]["type"].asString() == command[4]) {
											version = version_manifest["versions"][i]["id"].asString();
											if (!file_exists("manifests/versions/" + version + ".json"))
												gDownload(version_manifest["versions"][i]["url"].asString(), "manifests/versions/" + version + ".json");
											else {
												LoggerLog << "Version: \"" << version << "\" existed." << std::endl;
												logger.LogINFO(LoggerLog);
											}
										}
									}
								}
								else if (args < 5) {
									LoggerLog << "Downloading all versions..." << std::endl;
									logger.LogINFO(LoggerLog);
									std::string version;
									for (int i = 0; i < version_manifest["versions"].size(); i += 1) {
										version = version_manifest["versions"][i]["id"].asString();
										if (!file_exists("manifests/versions/" + version + ".json"))
											gDownload(version_manifest["versions"][i]["url"].asString(), "manifests/versions/" + version + ".json");
										else {
											LoggerLog << "Version: \"" << version << "\" existed." << std::endl;
											logger.LogINFO(LoggerLog);
										}
									}
								}
								else {
									LoggerLog << "No version type: \"" << command[4] << "\"." << std::endl;
									logger.LogWARN(LoggerLog);
								}
							}
							else if (args > 3) {
								std::string version = command[3];

								bool dofound = false;
								for (int i = 0; i < version_manifest["versions"].size(); i += 1) {
									if (version == version_manifest["versions"][i]["id"].asString()) {
										LoggerLog << "Version found. Downloading spacific manifest." << std::endl;
										logger.LogINFO(LoggerLog);
										if (!file_exists("manifests/versions/" + version + ".json"))
											gDownload(version_manifest["versions"][i]["url"].asString(), "manifests/versions/" + version + ".json");
										else {
											LoggerLog << "Version: \"" << version << "\" existed." << std::endl;
											logger.LogINFO(LoggerLog);
										}
										dofound = true;
									}
								}
								if (!dofound) {
									LoggerLog << "Version not found." << std::endl;
									logger.LogWARN(LoggerLog);
								}
							}
							else {
								LoggerLog << "Version?" << std::endl;
								logger.LogWARN(LoggerLog);
							}
						}
						else {
							LoggerLog << "No such command." << std::endl;
							logger.LogWARN(LoggerLog);
						}
					}
					else if (command[1] == "create") {
						if (args <= 2) {
							LoggerLog << "ServerID?" << std::endl;
							logger.LogWARN(LoggerLog);
						}
						
						bool doexist = false;
						std::string server_create = command[2];

						profile.LoadJson(doexist, "serverlist/" + server_create + "/exist");
						if (doexist) {
							LoggerLog << "Server \"" << server_create << "\" existed." << std::endl;
							logger.LogWARN(LoggerLog);
						}
						else {
							profile.MakeJson("serverlist/" + server_create + "/path", "./server/" + server_create);
							profile.MakeJson("serverlist/" + server_create + "/exist", true);
							std::filesystem::create_directories("./server/" + server_create);
							LoggerLog << "Server: \"" << server_create << "\" created." << std::endl;
							logger.LogINFO(LoggerLog);
						}
					}
					else if (command[1] == "select") {
						if (args <= 2) {
							LoggerLog << "ServerID?" << std::endl;
							logger.LogWARN(LoggerLog);
						}
						
						bool doexist = false;
						
						profile.LoadJson(doexist, "serverlist/" + command[2] + "/exist");
						if (doexist) {
							do_server_selected = true;
							selected_server = command[2];
							profile.LoadJson(selected_server_path, "serverlist/" + command[2] + "/path");
							profile.LoadJson(selected_server_version, "serverlist/" + command[2] + "/version");
							profile.LoadJson(selected_server_java_path, "serverlist/" + command[2] + "/java_version");
							LoggerLog << "Server: \"" << command[2] << "\" selected." << std::endl;
							logger.LogINFO(LoggerLog);
						}
						else {
							LoggerLog << "Server: \"" << command[2] << "\" not exist." << std::endl;
							logger.LogWARN(LoggerLog);
						}
					}
					else if (command[1] == "unload") {
						do_server_selected = false;
						selected_server_path.clear();
						selected_server.clear();
					}
					else if (command[1] == "list") {
						Json::Value serverList;
						profile.LoadJsonToValue(serverList);
						int count = 0;
						for (auto iterator = serverList["serverlist"].begin(); iterator != serverList["serverlist"].end(); ++iterator) {
							count += 1;
							LoggerLog << "Server " << count << ": \"" << iterator.name() << "\"." << std::endl;
							logger.LogINFO(LoggerLog);
						}
					}
					else if (command[1] == "delete") {
						if (args <= 2) {
							LoggerLog << "ServerID?" << std::endl;
							logger.LogWARN(LoggerLog);
						}
						else {
							Json::Value JsonProfile;
							profile.LoadJsonToValue(JsonProfile);
							
							bool dofound = false;
							for (auto iterator = JsonProfile["serverlist"].begin(); iterator != JsonProfile["serverlist"].end(); ++iterator) {
								if (command[2] == iterator.name()) {
									dofound = true;
									std::filesystem::remove_all(JsonProfile["serverlist"][iterator.name()]["path"].asString());
									JsonProfile["serverlist"].removeMember(command[2]);
									profile.WriteJson(JsonProfile);
									LoggerLog << "Server Deleted." << std::endl;
									logger.LogINFO(LoggerLog);
									break;
								}
							}
							if (!dofound) {
								LoggerLog << "Server: \"" << command[2] << "\" Not Found." << std::endl;
								logger.LogWARN(LoggerLog);
							}
						}
					}
					else if (command[1] == "java64") {
						if (command[2] == "select" && do_server_selected && args == 4) {
							std::string javaVersion = command[3];
							Json::Value javaListJson;
							profile.LoadJsonToValue(javaListJson);
							for (int i = 0; i < javaListJson["java"]["x64"].size(); i += 1) {
								if (javaListJson["java"]["x64"][i]["name"].asString() == command[3]) {
									selected_server_java_path = javaListJson["java"]["x64"][i]["path"].asString();
									LoggerLog << "Java Selected." << std::endl;
									logger.LogINFO(LoggerLog);
									break;
								}
							}
							profile.MakeJson("serverlist/" + selected_server + "/java_version", selected_server_java_path);
						}
					}
					else if (command[1] == "make" && do_server_selected) {
						basicSetup(selected_server_path, "server.jar", selected_server_java_path + "\\bin\\");
						argumentSetup(selected_server, CdInfo.dInfo.CPU_core_num);
						LoggerLog << "Server File Make" << std::endl;
						logger.LogINFO(LoggerLog);
					}
					else if (command[1] == "core") {
						if (args <= 2) {
							LoggerLog << "Core?" << std::endl;
							logger.LogWARN(LoggerLog);
						}
						else {

						}
					}
					else if (command[1] == "argument") {
						if (command[2] == "memory") {
							if (command[3] == "maxgb" && do_server_selected) {
								if (args <= 4) {
									LoggerLog << "Memory?" << std::endl;
									logger.LogWARN(LoggerLog);
								}
								else if (isDigitString(command[4])) {
									profile.MakeJson("serverlist/" + selected_server + "/jvmargs/maxmem", stoi(command[4]));
									profile.MakeJson("serverlist/" + selected_server + "/jvmargs/maxmemu", std::string("G"));
									LoggerLog << "Maximum Memory Set to: " << command[4] << "GB" << std::endl;
									logger.LogINFO(LoggerLog);
								}
								else {
									LoggerLog << "Memory CANT be Character." << std::endl;
									logger.LogWARN(LoggerLog);
								}
							}
							else if (command[3] == "mingb" && do_server_selected) {
								if (args <= 4) {
									LoggerLog << "Memory?" << std::endl;
									logger.LogWARN(LoggerLog);
								}
								else if (isDigitString(command[4])) {
									profile.MakeJson("serverlist/" + selected_server + "/jvmargs/minmem", stoi(command[4]));
									profile.MakeJson("serverlist/" + selected_server + "/jvmargs/minmemu", std::string("G"));
									LoggerLog << "Minimum Memory Set to: " << command[4] << "GB" << std::endl;
									logger.LogINFO(LoggerLog);
								}
								else {
									LoggerLog << "Memory CANT be Character." << std::endl;
									logger.LogWARN(LoggerLog);
								}
							}
							else if (command[3] == "maxmb" && do_server_selected) {
								if (args <= 4) LoggerLog << "Memory?" << std::endl;
								else if (isDigitString(command[4])) {
									profile.MakeJson("serverlist/" + selected_server + "/jvmargs/maxmem", stoi(command[4]));
									profile.MakeJson("serverlist/" + selected_server + "/jvmargs/maxmemu", std::string("M"));
									LoggerLog << "Maximum Memory Set to: " << command[4] << "MB" << std::endl;
									logger.LogINFO(LoggerLog);
								}
								else {
									LoggerLog << "Memory CANT be Character." << std::endl;
									logger.LogWARN(LoggerLog);
								}
							}
							else if (command[3] == "minmb" && do_server_selected) {
								if (args <= 4) {
									LoggerLog << "Memory?" << std::endl;
									logger.LogWARN(LoggerLog);
								}
								else if (isDigitString(command[4])) {
									profile.MakeJson("serverlist/" + selected_server + "/jvmargs/minmem", stoi(command[4]));
									profile.MakeJson("serverlist/" + selected_server + "/jvmargs/minmemu", std::string("M"));
									LoggerLog << "Minimum Memory Set to: " << command[4] << "MB" << std::endl;
									logger.LogINFO(LoggerLog);
								}
								else {
									LoggerLog << "Memory CANT be Character." << std::endl;
									logger.LogWARN(LoggerLog);
								}
							}
							else if (command[3] == "query") {
								CdInfo.updateMemoryInfo();
								LoggerLog << "Current Available Memory: " << CdInfo.dInfo.MEMORY_available_gb << "GB." << std::endl;
								logger.LogINFO(LoggerLog);
								if (0.8 * CdInfo.dInfo.MEMORY_available_gb >= 8.0) {
									LoggerLog << "Memory Recommand: >8GB" << std::endl;
									logger.LogINFO(LoggerLog);
								}
								else if (0.8 * CdInfo.dInfo.MEMORY_available_gb <= 4.0) {
									LoggerLog << "Memory is too low, please clear it before setting." << std::endl;
									logger.LogINFO(LoggerLog);
								}
								else {
									LoggerLog << "Memory Recommand: 4GB - 8GB" << std::endl;
									logger.LogINFO(LoggerLog);
								}
							}
							else {
								LoggerLog << "No such command." << std::endl;
								logger.LogWARN(LoggerLog);
							}
						}
					}
					else if (command[1] == "manifest" && command[2] == "update") {
						gDownload(version_manifest_url, version_manifest_path);
						LoggerLog << "Manifest Updated." << std::endl;
						logger.LogINFO(LoggerLog);
					}
					else {
						LoggerLog << "No such command." << std::endl;
						logger.LogWARN(LoggerLog);
					}

				}
				else {
					LoggerLog << "File \"version_manifest.json\" not exists. Downloading..." << std::endl;
					logger.LogWARN(LoggerLog);
					std::filesystem::create_directories("./manifests/");
					gDownload(version_manifest_url, version_manifest_path);
				}
			}
			else if (command[0] == "java") {
				if (command[1] == "list") {
					Json::Value JsonProfile;
					profile.LoadJsonToValue(JsonProfile);

					LoggerLog << "x64: ";
					logger.LogINFO(LoggerLog);
					for (int i = 0; i < JsonProfile["java"]["x64"].size(); i += 1) std::cout << JsonProfile["java"]["x64"][i]["name"].asString() << " ";
					std::cout << std::endl;

					LoggerLog << "x86: ";
					logger.LogINFO(LoggerLog);
					for (int i = 0; i < JsonProfile["java"]["x86"].size(); i += 1) std::cout << JsonProfile["java"]["x86"][i]["name"].asString() << " ";
					std::cout << std::endl;
					
					JsonProfile.clear();
				}
				else if (command[1] == "update") {
					std::vector<Json::Value> javaVersion64, javaVersion32;
					Json::Value temp;
					std::string path64 = "C:\\Program Files\\Java", path32 = "C:\\Program Files (x86)\\Java", javaPath;
					for (const auto& entry : std::filesystem::directory_iterator(path64)) {
						javaPath = entry.path().string();
						if (javaPath.find("jdk") != (size_t)-1) {
							temp["name"] = javaPath.substr(javaPath.find("jdk"), javaPath.size());
							temp["path"] = javaPath;
							javaVersion64.push_back(temp);
						}
					}
					for (const auto& entry : std::filesystem::directory_iterator(path32)) {
						javaPath = entry.path().string();
						if (javaPath.find("jdk") != (size_t)-1) {
							temp["name"] = javaPath.substr(javaPath.find("jdk"), javaPath.size());
							temp["path"] = javaPath;
							javaVersion32.push_back(temp);
						}
					}
					Json::Value JsonProfile;
					profile.LoadJsonToValue(JsonProfile);
					JsonProfile["java"].clear();
					for (int i = 0; i < javaVersion64.size(); i += 1) JsonProfile["java"]["x64"].append(javaVersion64[i]);
					for (int i = 0; i < javaVersion32.size(); i += 1) JsonProfile["java"]["x86"].append(javaVersion32[i]);
					profile.WriteJson(JsonProfile);
					JsonProfile.clear();
					LoggerLog << "Java Version Updated." << std::endl;
					logger.LogINFO(LoggerLog);
				}
				else {
					LoggerLog << "No such command." << std::endl;
					logger.LogWARN(LoggerLog);
				}
			}
			else if (command[0] == "download" && args >= 4) {
				std::string url = command[1], filepath = command[2], filename = command[3];
				std::filesystem::create_directories(filepath);
				gDownload(url, filepath + "/" + filename, true);
				LoggerLog << "Downloaded." << std::endl;
				logger.LogINFO(LoggerLog);
			}
			else if (command[0] == "quit" || command[0] == "exit") break;
			else {
				LoggerLog << "No such command." << std::endl;
				logger.LogWARN(LoggerLog);
			}

			command.clear();
		}
	}
	LoggerLog << "Bye~ :D" << std::endl;
	logger.LogINFO(LoggerLog);
	return 0;
}