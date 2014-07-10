

#include "config.h"
#include "radar.h"
#include "postfile.h"

#include "Poco/Glob.h"
#include "Poco/File.h"
#include "Poco/Thread.h"
#include "Poco/Exception.h"
#include "Poco/Util/XMLConfiguration.h"

#include <iostream>
#include <string>

#include <algorithm>


bool check_file(Poco::File& cur, const std::string & station)
{
	std::string ltime = getLastTimeStr(station);
	if (to_string(cur.getLastModified().epochTime()) > ltime) return true;

	return false;
}


bool CmpByLastModified(const std::string& left,
	const std::string& right)
{
	Poco::File leftf(left), rightf(right);

	return leftf.getLastModified() < rightf.getLastModified();

}

bool handle_file(Poco::File& cur, const std::string & station)
{
	using Poco::DateTime; using Poco::DateTimeFormat; using Poco::DateTimeFormatter;

	bool result = true;
	UploadInfo upinfo;
	result = result && generate_pngdata(cur, upinfo);

	if (!result) return false;

	DateTime begin = get_datatime(upinfo.date_begin, upinfo.seconds_begin),
		end = get_datatime(upinfo.date_end, upinfo.seconds_end);


	upinfo.post_form["radarBeginTime"] = DateTimeFormatter::format(begin, DateTimeFormat::SORTABLE_FORMAT);
	upinfo.post_form["radarEndTime"] = DateTimeFormatter::format(end, DateTimeFormat::SORTABLE_FORMAT);

	upinfo.post_form["dataPostTime"] = DateTimeFormatter::format(DateTime(), DateTimeFormat::SORTABLE_FORMAT);
	upinfo.post_form["station"] = station;

	auto it = upinfo.elevs.begin();
	std::string json("[");
	for (json += '"'+ *it + '"', it++; it != upinfo.elevs.end(); it++)
	{
		json += ",\"" + *it + '"';
	}

	json += ']';
	upinfo.post_form["elevsJson"] = json;

	result = result && post_file(upinfo);

	return result;
}

void scan_station(const std::string & station)
{
	//"dirtest/*.txt";
	std::string path = getScanPath(station);


	using Poco::Glob;

	std::set<std::string> files;

	Glob::glob(path.c_str(), files);

	std::vector<Poco::File> vfiles(files.rbegin(), files.rend());

	std::string ltime = getLastTimeStr(station);

	for (auto it = vfiles.begin(); it != vfiles.end(); ++it)
	{
		if (to_string(it->getLastModified().epochTime()) <= ltime)
		{
			vfiles.resize(it - vfiles.begin());
			break;
		}
	}


	for (auto it = vfiles.rbegin(); it != vfiles.rend(); ++it)
	{
		Poco::File &cur = *it;

		applog << '\n' << getCurDateTimeStr() << " 发现新文件:" << cur.path() << "\n";

		if (handle_file(cur, station))
		{
			setLastTimeStr(station, to_string(cur.getLastModified().epochTime()));
			applog << '\n' << getCurDateTimeStr() << " 成功处理文件:" << cur.path() << " ！ \n";
		}
		else
		{
			applog << '\n' << getCurDateTimeStr() << " 文件:" << cur.path() << "处理失败！ \n";
		}

	}

}

int start()
{
	curl_global_init(CURL_GLOBAL_ALL);

	applog << "\n           version 1.1 20140710 radarpng upload";
	applog << "\n-----------------------------------------------------\n\n";

	std::vector<std::string> stations;

	while (true)
	{
		try
		{
			stations.clear();
			
			getScanStations(stations); 
			
			for (auto it = stations.begin(); it != stations.end(); it++)
			{
				scan_station(*it);
			}
			
		}
		catch (Poco::Exception &exc)
		{
			applog << "\n exc:" << exc.displayText();
		}
		catch (...)
		{
		}

		int wait = 5;
		applog << "\n waiting " << wait << " seconds...\n";
		Poco::Thread::sleep(1000 * wait);
	}

	curl_global_cleanup();
	return 0;
}

int main()
{
	start();
}

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR    lpCmdLine,
	int       cmdShow)
{

	start();

	return 0;
}