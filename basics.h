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

#define LOGINFO 0
#define WARN 1
#define EROR 2

typedef struct _DEVICEINFO_ {
	int CPU_architecture;
	unsigned int CPU_core_num;
	size_t MEMORY_total_byte;
	double MEMORY_total_gb;
	size_t MEMORY_available_byte;
	double MEMORY_available_gb;
} DeviceInfo;

static std::string utol(std::string str) {
	for (int i = 0; i <= str.length(); i += 1)
		if (str[i] <= 90 && str[i] >= 65)
			str[i] += 32;
	return str;
}

std::string GetLogger(int type = LOGINFO);

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

#endif