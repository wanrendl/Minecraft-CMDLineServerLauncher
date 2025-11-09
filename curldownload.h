#ifndef _CURLDOWNLOAD_H_
#define _CURLDOWNLOAD_H_

#include <curl/curl.h>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include "basics.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "libcurl.lib")

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

bool file_exists(std::string name);

curl_off_t get_file_size(const std::string filename);

static int ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);

int gDownload(std::string url, std::string filename, bool resume_mode = false);

#endif