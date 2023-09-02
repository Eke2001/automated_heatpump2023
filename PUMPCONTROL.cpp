#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <cstring>
#include <Windows.h>
#include <time.h>

using namespace std;

void openWebsite(string);
bool GetCurrentChar(string);
void log(string);
void log(const char*);
void log2(string);

int main()
{
	/*==========================================*/
	int Waitms = 1000 * 60;
	/*==========================================*/
	int MarkedHour = -1;
	while (true)
	{
		time_t now = time(0);
		tm ltm;
		localtime_s(&ltm, &now);

		char date[20];
		if (MarkedHour != ltm.tm_hour && ((ltm.tm_hour == 0 && ltm.tm_min > 3) || ltm.tm_hour > 0 )) {
			strftime(date, sizeof(date), "NPS-%Y-%m-%d.txt", &ltm);
			bool ONOFF = GetCurrentChar(date);
			string URL = "";
			if (ONOFF) {
				URL = "https://tont.eu/api/pump/switch/auto/REAL1";
			}
			else {
				URL = "https://tont.eu/api/pump/switch/off/REAL1";
			}
			if (MarkedHour != ltm.tm_hour) {
				log(URL);
				openWebsite(URL);
				MarkedHour = ltm.tm_hour;
			}
		}
		log2("Sleeping  " + to_string(Waitms) + " ms...\r");
		Sleep(Waitms);
	}
	return 0;
}

void openWebsite(string website)
{
	system(("start " + website).c_str());
}

bool GetCurrentChar(string filename) {
	time_t now = time(0);
	tm localtimenow;
	localtime_s(&localtimenow, &now);
	string line;
	printf("\n");
	log("Attempting open file: " + filename);
	ifstream file(filename);
	if (file.is_open()) {
		while (getline(file, line))
		{
			int num1 = (line[2] - '0') * 10 + (line[3] - '0');
			if (num1 == localtimenow.tm_hour) {
				log("found On or Off " + to_string(num1) + " h. Value: " + line[0]);
				file.close();
				if (line[0] == '+') {
					return true;
				}
				else
					return false;
			}
		}
		file.close();
	}
	else
	{
		log("Unable to open file");
		return false;
	}
}

void log(string txt)
{
	time_t now = time(0);
	tm ltm;
	localtime_s(&ltm, &now);
	cout << "[" << ltm.tm_hour << ":" << ltm.tm_min << ":" << ltm.tm_sec << "] " << txt << endl;
}

void log(const char* txt)
{
	log(std::string(txt));
}

void log2(string txt) {
	time_t now = time(0);
	tm ltm;
	localtime_s(&ltm, &now);
	cout << "[" << ltm.tm_hour << ":" << ltm.tm_min << ":" << ltm.tm_sec << "] " << txt;
}
