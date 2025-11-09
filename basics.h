#ifndef _BASICS_H_
#define _BASICS_H_

#define _CRT_SECURE_NO_WARNINGS

#include <intrin.h>
#include <iostream>
#include <Windows.h>

#pragma comment(lib, "urlmon.lib")

#define X86 0
#define X64 1
#define ARM64 2
#define AMD64 1

typedef struct _DEVICEINFO_ {
	int CPU_architecture;
	unsigned int CPU_core_num;
	size_t MEMORY_total_byte;
	double MEMORY_total_gb;
	size_t MEMORY_available_byte;
	double MEMORY_available_gb;
} DeviceInfo;

std::string utol(std::string str);

class _cDeviceInfo {
public:
	DeviceInfo dInfo;
public:
	void getAllInfo();
	void updateMemoryInfo();
};

void basicSetup(std::string path, std::string core, std::string javapath);
void argumentSetup(std::string path, int corenum);

bool isDigitString(std::string str);
static std::string GetLocalTime();

class Logger {
public:
	void LogINFO(std::string& Log);
	void LogINFO(std::stringstream& Log);
	void LogWARN(std::string& Log);
	void LogWARN(std::stringstream& Log);
	void LogERROR(std::string& Log);
	void LogERROR(std::stringstream& Log);
};

#endif