#include "basics.h"
#include "gjson.h"
#include <fstream>
#include <iomanip>
#include <sstream>

static string GetLocalTime() {
	time_t now = time(NULL);
	tm* tm_t = localtime(&now);
	int date = tm_t->tm_mday;
	int hh = tm_t->tm_hour;
	int mm = tm_t->tm_min;
	int ss = tm_t->tm_sec;
	stringstream dateStream;
	dateStream << setw(4) << setfill('0') << tm_t->tm_year + 1900 << "-"
		<< setw(2) << setfill('0') << tm_t->tm_mon + 1 << "-"
		<< setw(2) << setfill('0') << tm_t->tm_mday << " "
		<< setw(2) << setfill('0') << tm_t->tm_hour << ":"
		<< setw(2) << setfill('0') << tm_t->tm_min << ":"
		<< setw(2) << setfill('0') << tm_t->tm_sec;
	return dateStream.str();
}

string GetLogger(int type) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
	cout << "[" << GetLocalTime() << "]";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	if (type == LOGINFO) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
		cout << "[INFO] ";
	}
	else if (type == WARN) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
		cout << "[WARN] ";
	}
	else if (type == EROR) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
		cout << "[ERROR] ";
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	return "";
}

void _cDeviceInfo::getAllInfo() {
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	WORD architecture = systemInfo.wProcessorArchitecture;

	if (architecture == PROCESSOR_ARCHITECTURE_INTEL) dInfo.CPU_architecture = X86;
	else if (architecture == PROCESSOR_ARCHITECTURE_AMD64) dInfo.CPU_architecture = AMD64;
	else if (architecture == PROCESSOR_ARCHITECTURE_ARM) dInfo.CPU_architecture = ARM64;

	dInfo.CPU_core_num = (unsigned int)systemInfo.dwNumberOfProcessors;

	string memoryInfo;
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
	string memoryInfo;
	MEMORYSTATUSEX statusex;
	statusex.dwLength = sizeof(statusex);
	if (GlobalMemoryStatusEx(&statusex)) {
		dInfo.MEMORY_available_byte = statusex.ullAvailPhys;
		dInfo.MEMORY_available_gb = statusex.ullAvailPhys / static_cast<double>(1073741824);
	}
}

void basicSetup(string path, string core, string javapath = "") {
	ofstream fsArguments(path + "/args.txt", ios::out);
	fsArguments << "# Arguments." << endl
		<< "# You can type in your custom argument." << endl << endl;
	fsArguments.close();
	ofstream fsLaunchbat(path + "/start.bat", ios::out);
	fsLaunchbat << "@ECHO OFF" << endl
		<< "\"" << javapath << "java\" @args.txt -jar " << core << " %*" << endl
		<< "PAUSE";
	fsLaunchbat.close();
	ofstream fsEula(path + "/eula.txt", ios::out);
	fsEula << "# By changing the setting below to TRUE you are indicating your agreement to our EULA(https://aka.ms/MinecraftEULA)." << endl
		<< "eula=true";
	fsEula.close();
}

void argumentSetup(string server, int corenum) {
	ofstream fsArguments("./server/" + server + "/args.txt", ios::out | ios::app);
	string profileRead;
	int minmem;
	string memtype;

	LoadJson("profile.json", minmem, "serverlist/" + server + "/jvmargs/minmem");
	fsArguments << "-Xms" << minmem;
	LoadJson("profile.json", memtype, "serverlist/" + server + "/jvmargs/minmemu");
	fsArguments << memtype << endl;

	LoadJson("profile.json", profileRead, "serverlist/" + server + "/jvmargs/maxmem");
	fsArguments << "-Xmx" << profileRead;
	LoadJson("profile.json", profileRead, "serverlist/" + server + "/jvmargs/maxmemu");
	fsArguments << profileRead << endl;

	if ((minmem >= 16 && memtype == "G") || (minmem >= 16000 && memtype == "M")) fsArguments << "-XX:+UseZGC" << endl;
	else {
		fsArguments << "-XX:+UseG1GC" << endl
			<< "-XX:MaxGCPauseMillis=200" << endl
			<< "-XX:+ParallelRefProcEnabled" << endl
			<< "-XX:G1HeapRegionSize=16M" << endl;
	}

	fsArguments << "-XX:ParallelGCThreads=" << corenum << endl;
	fsArguments << "-XX:ConcGCThreads=" << (int)(corenum / 4) << endl;

	fsArguments << "-Dfile.encoding=UTF-8" << endl;
	fsArguments << "-XX:+DisableExplicitGC" << endl;
	fsArguments << "-XX:+AlwaysPreTouch";

	fsArguments.close();
}

bool isDigitString(string str) {
	if (str.empty()) return false;
	for (unsigned char c : str) if (!isdigit(c)) return false;
	return true;
}