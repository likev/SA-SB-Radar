#ifndef _UPLOAD_FILE_INTERVAL
#define _UPLOAD_FILE_INTERVAL

#include <string>
#include <vector>
#include <map>
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"

std::string getLastTimeStr(const std::string & dir);

std::vector<std::string> & getDirList(std::vector<std::string> & dirs);

void setLastTimeStr(const std::string & dir, const std::string & timestr);

std::string getScanPath(const std::string & dir);

std::string getUploadUrl();
std::string getProxy();
std::string getPngOutDir();


struct UploadInfo{
	std::vector<std::string> elevs, rfiles, vfiles;
	std::map<std::string, std::string> post_form;
	int date_begin, date_end, seconds_begin, seconds_end;
};

//get_datatime_from_days_milliseconds
Poco::DateTime get_datatime(int days, int milliseconds);

#endif //_UPLOAD_FILE_INTERVAL