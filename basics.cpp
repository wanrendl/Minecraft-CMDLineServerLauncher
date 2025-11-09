#include "basics.h"
#include "gjson.h"
#include <fstream>
#include <iomanip>
#include <sstream>

static std::string GetLocalTime() {
	time_t now = time(NULL);
	tm* tm_t = localtime(&now);
	int date = tm_t->tm_mday;
	int hh = tm_t->tm_hour;
	int mm = tm_t->tm_min;
	int ss = tm_t->tm_sec;
	std::stringstream dateStream;
	dateStream << std::setw(4) << std::setfill('0') << tm_t->tm_year + 1900 << "-"
		<< std::setw(2) << std::setfill('0') << tm_t->tm_mon + 1 << "-"
		<< std::setw(2) << std::setfill('0') << tm_t->tm_mday << " "
		<< std::setw(2) << std::setfill('0') << tm_t->tm_hour << ":"
		<< std::setw(2) << std::setfill('0') << tm_t->tm_min << ":"
		<< std::setw(2) << std::setfill('0') << tm_t->tm_sec;
	return dateStream.str();
}

std::string utol(std::string str) {
	for (int i = 0; i <= str.length(); i += 1)
		if (str[i] <= 90 && str[i] >= 65)
			str[i] += 32;
	return str;
}

void Logger::LogINFO(std::string& Log) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
	std::cout << "[" << GetLocalTime() << "]" << "[INFO] ";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::cout << Log;
}
void Logger::LogINFO(std::stringstream& Log) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
	std::cout << "[" << GetLocalTime() << "]" << "[INFO] ";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::cout << Log.str();
	Log.str("");
}

void Logger::LogWARN(std::string& Log) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
	std::cout << "[" << GetLocalTime() << "]";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
	std::cout << "[WARN] ";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::cout << Log;
}
void Logger::LogWARN(std::stringstream& Log) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
	std::cout << "[" << GetLocalTime() << "]";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
	std::cout << "[WARN] ";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::cout << Log.str();
	Log.str("");
}

void Logger::LogERROR(std::string& Log) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
	std::cout << "[" << GetLocalTime() << "]";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
	std::cout << "[ERROR] ";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::cout << Log;
}
void Logger::LogERROR(std::stringstream& Log) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
	std::cout << "[" << GetLocalTime() << "]";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
	std::cout << "[ERROR] ";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::cout << Log.str();
	Log.str("");
}

void _cDeviceInfo::getAllInfo() {
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	WORD architecture = systemInfo.wProcessorArchitecture;

	if (architecture == PROCESSOR_ARCHITECTURE_INTEL) dInfo.CPU_architecture = X86;
	else if (architecture == PROCESSOR_ARCHITECTURE_AMD64) dInfo.CPU_architecture = AMD64;
	else if (architecture == PROCESSOR_ARCHITECTURE_ARM) dInfo.CPU_architecture = ARM64;

	dInfo.CPU_core_num = (unsigned int)systemInfo.dwNumberOfProcessors;

	std::string memoryInfo;
	MEMORYSTATUSEX statusex;
	statusex.dwLength = sizeof(statusex);
	if (GlobalMemoryStatusEx(&statusex)) {
		dInfo.MEMORY_total_byte = statusex.ullTotalPhys;
		dInfo.MEMORY_total_gb = statusex.ullTotalPhys / static_cast<double>(1073741824);

		dInfo.MEMORY_available_byte = statusex.ullAvailPhys;
		dInfo.MEMORY_available_gb = statusex.ullAvailPhys / static_cast<double>(1073741824);
	}
}

void _cDeviceInfo::updateMemoryInfo() {
	std::string memoryInfo;
	MEMORYSTATUSEX statusex;
	statusex.dwLength = sizeof(statusex);
	if (GlobalMemoryStatusEx(&statusex)) {
		dInfo.MEMORY_available_byte = statusex.ullAvailPhys;
		dInfo.MEMORY_available_gb = statusex.ullAvailPhys / static_cast<double>(1073741824);
	}
}

void basicSetup(std::string path, std::string core, std::string javapath = "") {
	std::ofstream fsArguments(path + "/args.txt", std::ios::out);
	fsArguments << "# Arguments." << std::endl
		<< "# You can type in your custom argument." << std::endl << std::endl;
	fsArguments.close();
	std::ofstream fsLaunchbat(path + "/start.bat", std::ios::out);
	fsLaunchbat << "@ECHO OFF" << std::endl
		<< "\"" << javapath << "java\" @args.txt -jar " << core << " %*" << std::endl
		<< "PAUSE";
	fsLaunchbat.close();
	std::ofstream fsEula(path + "/eula.txt", std::ios::out);
	fsEula << "# By changing the setting below to TRUE you are indicating your agreement to our EULA(https://aka.ms/MinecraftEULA)." << std::endl
		<< "eula=true";
	fsEula.close();
}

void argumentSetup(std::string server, int corenum) {
	std::ofstream fsArguments("./server/" + server + "/args.txt", std::ios::out | std::ios::app);
	std::string profileRead;
	int minmem;
	std::string memtype;

	LoadJson("profile.json", minmem, "serverlist/" + server + "/jvmargs/minmem");
	fsArguments << "-Xms" << minmem;
	LoadJson("profile.json", memtype, "serverlist/" + server + "/jvmargs/minmemu");
	fsArguments << memtype << std::endl;

	LoadJson("profile.json", profileRead, "serverlist/" + server + "/jvmargs/maxmem");
	fsArguments << "-Xmx" << profileRead;
	LoadJson("profile.json", profileRead, "serverlist/" + server + "/jvmargs/maxmemu");
	fsArguments << profileRead << std::endl;

	if ((minmem >= 16 && memtype == "G") || (minmem >= 16000 && memtype == "M")) fsArguments << "-XX:+UseZGC" << std::endl;
	else {
		fsArguments << "-XX:+UseG1GC" << std::endl
			<< "-XX:MaxGCPauseMillis=200" << std::endl
			<< "-XX:+ParallelRefProcEnabled" << std::endl
			<< "-XX:G1HeapRegionSize=16M" << std::endl;
	}

	fsArguments << "-XX:ParallelGCThreads=" << corenum << std::endl;
	fsArguments << "-XX:ConcGCThreads=" << (int)(corenum / 4) << std::endl;

	fsArguments << "-Dfile.encoding=UTF-8" << std::endl;
	fsArguments << "-XX:+DisableExplicitGC" << std::endl;
	fsArguments << "-XX:+AlwaysPreTouch";

	fsArguments.close();
}

bool isDigitString(std::string str) {
	if (str.empty()) return false;
	for (unsigned char c : str) if (!isdigit(c)) return false;
	return true;
}

