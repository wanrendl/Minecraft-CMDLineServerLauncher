#include "curldownload.h"

using namespace std;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	size_t realsize = size * nmemb;
	ofstream* file = static_cast<ofstream*>(userp);
	if (!file) return 0;

	file->write(static_cast<char*>(contents), realsize);
	return realsize;
}

bool file_exists(string name) {
	return filesystem::exists(name);
}

curl_off_t get_file_size(const string filename) {
	try {
		return filesystem::file_size(filename);
	}
	catch (...) {
		return 0;
	}
}

static int ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
	// 获取下载开始时间
	static time_t start_time = time(0);
	time_t now = time(0);

	// 计算已用时间（秒）
	time_t duration = now - start_time;
	if (duration == 0) duration = 1; // 避免除以零

	// 计算下载速度（KB/s）
	double speed = (dlnow > 0) ? (dlnow / 1024.0) / duration : 0.0;

	if (dltotal > 0) {
		double progress = static_cast<double>(dlnow) / dltotal * 100.0;

		// 计算剩余时间
		double remaining = (dltotal - dlnow) / (speed * 1024.0); // 秒

		// 进度显示
		cout << "\r" << GetLogger(LOGINFO) << "Downloading: "
			<< fixed << setprecision(2) << progress << "% "
			<< "(" << (double)dlnow / 1024 / 1024 << "MB/"
			<< (double)dltotal / 1024 / 1024 << "MB) "
			<< "Speed: " << setprecision(2) << speed << "KB/s "
			<< "ETA: " << static_cast<int>(remaining) << "s     " << flush;
	}
	else {
		// 未知总大小的情况
		cout << "\r" << GetLogger(LOGINFO) << "Downloading: "
			<< (double)dlnow / 1024 / 1024 << "MB received "
			<< "Speed: " << setprecision(2) << speed << "KB/s     " << flush;
	}

	return 0;
}


int gDownload(string url, string filename, bool resume_mode) {
	curl_off_t resume_from = 0;

	if (resume_mode && file_exists(filename)) {
		resume_from = get_file_size(filename);
		if (resume_from > 0) cout << GetLogger(LOGINFO) << "Resuming download from " << resume_from << " bytes." << endl;
	}

	CURL* curl;
	CURLcode res;

	// 打开输出文件
	ios_base::openmode mode = ios::binary;
	if (resume_mode && resume_from > 0) mode |= ios::app; // 追加模式
	else mode |= ios::trunc; // 截断模式
	ofstream output_file(filename, mode);

	if (!output_file.is_open()) {
		cerr << GetLogger(WARN) << "Error: Failed to open file " << filename << " for writing." << endl;
		return 1;
	}

	// 初始化libcurl
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if (!curl) {
		cerr << GetLogger(WARN) << "Failed to initialize libcurl." << endl;
		output_file.close();
		return 1;
	}

	// 设置下载选项
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output_file);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);  // 跟随重定向

	if (resume_mode && resume_from > 0) {
		curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, resume_from);
	}

	// 安全设置
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
	curl_easy_setopt(curl, CURLOPT_CAINFO, "./certificates/cacert.pem");
	curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

	// 性能优化
	curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 102400L); // 100KB 缓冲区
	curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
	curl_easy_setopt(curl, CURLOPT_TCP_KEEPIDLE, 120L);
	curl_easy_setopt(curl, CURLOPT_TCP_KEEPINTVL, 60L);

	// 进度回调
	curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, ProgressCallback);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

	// 用户代理
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");

	// 超时设置
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30L);
	curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1024L); // 1KB/s
	curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 60L);    // 60秒
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 300L);  // 30秒超时
	curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L); // 最多10次重定向

	curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L); // HTTP错误码>=400视为失败

	curl_easy_setopt(curl, CURLOPT_VERBOSE, false); //打印出具体http协议字段

	// 执行下载
	res = curl_easy_perform(curl);
	output_file.close();

	// 检查结果
	if (res != CURLE_OK) {
		cerr << endl << GetLogger(WARN) << "Download failed: " << curl_easy_strerror(res) << endl;

		// 获取更多错误信息
		if (res == CURLE_HTTP_RETURNED_ERROR) {
			long response_code;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
			cerr << GetLogger(WARN) << "HTTP Error: " << response_code << endl;
		}

		// 清理
		curl_easy_cleanup(curl);

		// 删除不完整的文件（非续传模式）
		if (!resume_mode) {
			remove(filename.c_str());
			cerr << GetLogger(WARN) << "Incomplete file deleted." << endl;
		}

		return 1;
	}

	// 获取下载信息
	double total_time;
	curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time);

	curl_off_t dl_size;
	curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD_T, &dl_size);

	double speed;
	curl_easy_getinfo(curl, CURLINFO_SPEED_DOWNLOAD, &speed);

	long response_code;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

	// 清理
	curl_easy_cleanup(curl);

	// 最终输出
	cout << endl << endl << GetLogger(LOGINFO) << "Download completed successfully!" << endl
		<< "File: " << filename << endl
		<< "Size: " << dl_size / 1024 << " KB (" << dl_size << " bytes)" << endl
		<< "Time: " << fixed << setprecision(2) << total_time << " seconds" << endl
		<< "Speed: " << speed / 1024 << " KB/s" << endl
		<< "HTTP Status: " << response_code << endl;

	// 清理资源
	curl_global_cleanup();

	cout << GetLogger(LOGINFO) << "File downloaded successfully: " << filename << endl;
	return 0;
}