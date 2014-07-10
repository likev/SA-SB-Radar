#ifndef _UPLOAD_FILE_INTERVAL
#define _UPLOAD_FILE_INTERVAL

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Poco/DateTime.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"

template <typename T>
std::string to_string(const T& org)
{
	std::string des;
	std::ostringstream sout;

	sout << org;
	return des = sout.str();
}

std::string getLastTimeStr(const std::string & dir);

std::vector<std::string> & getDirList(std::vector<std::string> & dirs);

void setLastTimeStr(const std::string & dir, const std::string & timestr);

std::vector<std::string>& getScanStations(std::vector<std::string>& vec);
std::string getScanPath(const std::string & dir);

std::string getUploadUrl();
std::string getProxy();
std::string getPngOutDir();
std::string getCurDateTimeStr();

struct UploadInfo{
	std::vector<std::string> elevs, rfiles, vfiles;
	std::map<std::string, std::string> post_form;
	int date_begin, date_end, seconds_begin, seconds_end;
};

//get_datatime_from_days_milliseconds
Poco::DateTime get_datatime(int days, int milliseconds);

class Logout{
	std::string logfile;
	std::ofstream fout;
public:
	Logout() :logfile("log.txt")
	{ 
		openfile(logfile);
	}

	Logout (const std::string& filename)
	{
		openfile(filename);
	}

	template<typename T>
	Logout& operator << (const T& right)
	{
		fout << right << std::flush;

		std::cout << right << std::flush;

		return *this;
	}
	~Logout(){fout.close();}
private:
	bool openfile(const std::string& filename)
	{
		fout.open(filename, std::ios_base::out|std::ios_base::app);

		return fout.is_open();
	}
};

extern Logout applog;

#endif //_UPLOAD_FILE_INTERVAL