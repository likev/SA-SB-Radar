#ifndef _UPLOAD_FILE_INTERVAL
#define _UPLOAD_FILE_INTERVAL

#include <string>
#include <vector>
#include <map>


std::string getLastTimeStr(const std::string & dir);

std::vector<std::string> & getDirList(std::vector<std::string> & dirs);

void setLastTimeStr(const std::string & dir, const std::string & timestr);

std::string getScanPath(const std::string & dir);

std::string getUploadUrl();
std::string getProxy();
std::string getPngOutDir();


struct UploadInfo{
	std::vector<std::string> elevs, files;
	std::map<std::string, std::string> post_form;
	int date_begin, date_end, seconds_begin, seconds_end;
};

#endif //_UPLOAD_FILE_INTERVAL