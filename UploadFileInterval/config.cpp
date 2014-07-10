
#include "config.h"
#include "Poco/Util/XMLConfiguration.h"
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"

Logout applog("logtest.txt");

std::string getLastTimeStr(const std::string & dir)
{
	Poco::AutoPtr<Poco::Util::XMLConfiguration>
		pConf(new Poco::Util::XMLConfiguration("radar-config.xml"));

	return pConf->getString("lasttime." + dir, "0");
}

void setLastTimeStr(const std::string & dir, const std::string & timestr)
{
	Poco::AutoPtr<Poco::Util::XMLConfiguration>
		pConf(new Poco::Util::XMLConfiguration("radar-config.xml"));

	pConf->setString("lasttime." + dir, timestr);
	pConf->save("radar-config.xml");
}

std::vector<std::string> & getDirList(std::vector<std::string> & dirs)
{
	Poco::AutoPtr<Poco::Util::XMLConfiguration>
		pConf(new Poco::Util::XMLConfiguration("radar-config.xml"));

	int count = pConf->getInt("dircount");

	for (int i = 0; i<count; i++)
	{
		//dirs.push_back(pConf->getString("dirname[" + to_string(i) + "]"));
	}
	return dirs;
}

std::string getScanPath(const std::string & dir)
{
	Poco::AutoPtr<Poco::Util::XMLConfiguration>
		pConf(new Poco::Util::XMLConfiguration("radar-config.xml"));

	//"dirtest/*.txt";
	return pConf->getString("path." + dir, "dirtest/*.txt");
}

std::vector<std::string>& getScanStations(std::vector<std::string>& vec)
{
	Poco::AutoPtr<Poco::Util::XMLConfiguration>
		pConf(new Poco::Util::XMLConfiguration("radar-config.xml"));

	//"dirtest/*.txt";
	int count = pConf->getInt("scanstations.count", 1);

	for (int i = 0; i != count; i++)
	{
		std::string id = "scanstations.station[" + to_string(i) + "]";
		vec.push_back(pConf->getString(id));

		//applog << vec[i] << ' ';
	}

	return vec;
}

std::string getUploadUrl()
{
	Poco::AutoPtr<Poco::Util::XMLConfiguration>
		pConf(new Poco::Util::XMLConfiguration("radar-config.xml"));

	//"dirtest/*.txt";
	return pConf->getString("uploadurl", "http://radaronline.sinaapp.com/post.php");
}

std::string getProxy()
{
	Poco::AutoPtr<Poco::Util::XMLConfiguration>
		pConf(new Poco::Util::XMLConfiguration("radar-config.xml"));

	//"dirtest/*.txt";
	return pConf->getString("proxy", "0");
}

std::string getPngOutDir()
{
	Poco::AutoPtr<Poco::Util::XMLConfiguration>
		pConf(new Poco::Util::XMLConfiguration("radar-config.xml"));

	//"dirtest/*.txt";
	return pConf->getString("pngoutdir", "pngout");
	
}

//get_datatime_from_days_milliseconds
Poco::DateTime get_datatime(int days, int milliseconds)
{
	Poco::DateTime begin(Poco::Timestamp(0));

	int hours = milliseconds / (3600 * 1000), minutes, microseconds;

	milliseconds %= (3600 * 1000);
	minutes = milliseconds / (60 * 1000);
	milliseconds %= (60 * 1000);
	microseconds = milliseconds * 1000;

	return begin + Poco::Timespan(days - 1, hours, minutes, 0, microseconds);
}

std::string getCurDateTimeStr()
{
	using Poco::DateTime; using Poco::DateTimeFormat; using Poco::DateTimeFormatter;

	//"dirtest/*.txt";
	return DateTimeFormatter::format(DateTime(), DateTimeFormat::SORTABLE_FORMAT);

}