#define CURL_STATICLIB
#include <iostream>
#include "curl/curl.h"
#include <cstring>
#include <fstream>
#include <chrono>
#include <cstdio>
#include <Windows.h>
#include <thread>
#include <ctime>
#include <string.h>
#pragma comment (lib, "Normaliz.lib")
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Crypt32.lib")
#pragma comment (lib, "Wldap32.lib")
#pragma comment (lib, "advapi32.lib")

using namespace std;

void log(string);
void log(const char*);
string to_stringB(bool);
bool fileExists(const string&);
bool testFileContent(const char*);
bool createFileFromUrl(string, std::tm& );
string to_string2(int);
bool is_success_today(const std::tm& );

int main()
{
	log("Alustan t66d");
	/*==========================================*/
	int EleringPollCooldownMS = 1000 * 60 * 5;
	int EleringPollCooldownOffHoursMS = 1000 * 60 * 60;
	/*==========================================*/

	time_t now = time(0);
	std::tm success = { 0 };

	while (true) {

		if (is_success_today(success)) {
			log("Uus fail on olemas. Ootan " + to_string(EleringPollCooldownOffHoursMS) + " ms");
			Sleep(EleringPollCooldownOffHoursMS);
			continue;
		}

		// Get today's date
		log("Proovin t6mmata");
		time_t now = time(0);
		tm* ltm = localtime(&now);
		int today = ltm->tm_mday;

		// Get tomorrow's date
		std::time_t t = std::time(nullptr);
		std::tm ltm_tomorrow = *std::localtime(&t);
		ltm_tomorrow.tm_mday += 1;

		// Get current year and month
		int yearNow = (ltm->tm_year + 1900);
		int monthNow = (ltm->tm_mon + 1);


		// Look at .csv file
		string url = "https://dashboard.elering.ee/api/nps/price/csv?start="
			+ to_string(yearNow) + "-"
			+ to_string2(monthNow) + "-"
			+ to_string2(today) + "T21%3A00%3A00Z&end="
			+ to_string(ltm_tomorrow.tm_year + 1900) + "-"
			+ to_string2(ltm_tomorrow.tm_mon + 1) + "-"
			+ to_string2(ltm_tomorrow.tm_mday) + "T20%3A00%3A00Z&fields=ee";
		log("url: " + url);
		bool isFileSuccessful = createFileFromUrl(url, ltm_tomorrow);
		log("Kas andmed on olemas: " + to_stringB(isFileSuccessful));
		if (isFileSuccessful == true) {
			success.tm_year = ltm->tm_year;
			success.tm_mon = ltm->tm_mon;
			success.tm_mday = ltm->tm_mday;

			// See file contents in terminal
			std::time_t t = std::time(nullptr);
			std::tm tm = {};
			localtime_s(&tm, &t);
			std::ostringstream oss;
			oss << "NPS-" << to_string(ltm_tomorrow.tm_year + 1900) + "-" + to_string2(ltm_tomorrow.tm_mon + 1) + "-" + to_string2(ltm_tomorrow.tm_mday) << ".csv";
			std::string fileName = oss.str();

			ifstream file(fileName);
			string line;

			while (getline(file, line)) {
				cout << line << endl;
			}
		}
		if (ltm->tm_hour < 15)
		{
			log("Ootan " + to_string(EleringPollCooldownOffHoursMS) + " ms");
			Sleep(EleringPollCooldownOffHoursMS);
		}
		else {
			log("Ootan " + to_string(EleringPollCooldownMS) + " ms");
			Sleep(EleringPollCooldownMS);
		}
	}
	return 0;
}

bool fileExists(const string& fileName) {
	ifstream infile(fileName);
	bool a = infile.good();
	log("File eksisteeris: " + to_stringB(a));
	return a;
}

bool createFileFromUrl(string url, std::tm& ltm_tomorrow) {
	CURL* curl;
	CURLcode res;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		std::time_t t = std::time(nullptr);
		std::tm tm = {};
		localtime_s(&tm, &t);
		std::ostringstream oss;
		oss << "NPS-" << to_string(ltm_tomorrow.tm_year + 1900) + "-" + to_string2(ltm_tomorrow.tm_mon + 1) + "-" + to_string2(ltm_tomorrow.tm_mday) << ".tmp";
		std::string fileName = oss.str();


		FILE* fp;
		errno_t err = fopen_s(&fp, fileName.c_str(), "w");
		if (err != 0) {
			char buf[256];
			strerror_s(buf, sizeof(buf), err);
			std::cerr << "fopen_s() failed: " << buf << std::endl;
			return false;
		}
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			char buf[256];
			strerror_s(buf, sizeof(buf), res);
			std::cerr << "curl_easy_perform() failed: " << buf << std::endl;
		}
		fclose(fp);

		curl_easy_cleanup(curl);
		return testFileContent(fileName.c_str());
	}
	return false;
}

bool testFileContent(const char* fileName) {
	std::ifstream file(fileName);
	std::string line;
	int count = 0;
	while (std::getline(file, line)) {
		++count;
	}
	file.close();
	if (count == 25) {
		std::string newFileName = std::string(fileName).substr(0, std::string(fileName).size() - 4) + ".csv";
		if (fileExists(newFileName)) {
			return true;
		}
		if (std::rename(fileName, newFileName.c_str()) != 0) {
			return false;
		}
		return true;
	}
	else {
		return false;
	}
}

string to_string2(int eesmineNULL) {
	if (eesmineNULL >= 10) {
		return to_string(eesmineNULL);
	}
	else {
		return "0" + to_string(eesmineNULL);
	}
}

bool is_success_today(const std::tm& success0) {
	time_t now = time(0);
	tm* localtimenow = localtime(&now);
	if (localtimenow->tm_year != success0.tm_year) {
		return false;
	}
	if (localtimenow->tm_mon != success0.tm_mon) {
		return false;
	}
	if (localtimenow->tm_mday != success0.tm_mday) {
		return false;
	}
	return true;
}

void log(string txt)
{
	time_t now = time(0);
	tm* ltm = localtime(&now);
	std:cout << "[" << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << "] " << txt << endl;
}

void log(const char* txt)
{
	log(std::string(txt));
}

string to_stringB(bool val) {
	if (val)
		return("jah");
	else
		return("ei");
}