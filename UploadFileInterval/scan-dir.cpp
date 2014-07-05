#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

#include "Poco/Glob.h"
#include "Poco/File.h"
#include "Poco/Thread.h"
#include "Poco/Exception.h"
#include "Poco/Util/XMLConfiguration.h"


	
std::string getLastTimeStr(const std::string & dir)
{
	Poco::AutoPtr<Poco::Util::XMLConfiguration>
		pConf(new Poco::Util::XMLConfiguration("radar-config.xml"));

	return pConf->getString("lasttime." + dir, "0");
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

void setLastTimeStr(const std::string & dir, const std::string & timestr)
{
	Poco::AutoPtr<Poco::Util::XMLConfiguration>
		pConf(new Poco::Util::XMLConfiguration("radar-config.xml"));

	pConf->setString("lasttime." + dir, timestr);
	pConf->save("radar-config.xml");
}


void post_file(Poco::File& cur)
{


}

template <typename T>
std::string to_string(const T& org)
{
	std::string des;
	std::ostringstream sout;

	sout << org;
	return des = sout.str();
}

bool check_file(Poco::File& cur)
{
	std::string ltime = getLastTimeStr("sanmenxia");
	if (to_string(cur.getLastModified().epochTime() ) > ltime) return true;

	return false;
}

//class CmpByLastModified
//{
	bool CmpByLastModified(const std::string& left,
							const std::string& right)
	{
		Poco::File leftf(left), rightf(right);

		return leftf.getLastModified() < rightf.getLastModified();

	}
//};






void scan_dir(const std::string & dir)
{
	Poco::AutoPtr<Poco::Util::XMLConfiguration>
		pConf(new Poco::Util::XMLConfiguration("radar-config.xml"));

	//"dirtest/*.txt";
	std::string path = pConf->getString("path." + dir, "dirtest/*.txt");


	using Poco::Glob;

	std::set<std::string> files;
	

	Glob::glob(path.c_str(), files);
	std::vector<std::string> vfiles(files.begin(), files.end());

	std::sort(vfiles.begin(), vfiles.end(), CmpByLastModified);

	// Glob::glob("/usr/include/*/*.h", files);

	//auto it = files.begin();
	//for (; it != files.end(); ++it)
	//{
	//	std::cout << *it << std::endl;
	//	Poco::File cur(*it);
	//	std::cout<<cur.getLastModified().epochTime()<<std::endl;

	//	if (check_file(cur))
	//	{
	//		post_file(cur);
	//	}

	//}

	auto it2 = vfiles.begin();
	for (; it2 != vfiles.end(); ++it2)
	{
		Poco::File cur(*it2);

		if (check_file(cur))
		{
			std::cout << *it2 << std::endl;
			
			
			time_t t = cur.getLastModified().epochTime();
			std::cout << t << std::endl;
			post_file(cur);

			setLastTimeStr("sanmenxia", to_string(t));
		}

	}

}

int main()
{
	while (true)
	{
		try
		{
			scan_dir("sanmenxia");
		}
		catch (Poco::Exception &exc)
		{
			std::cout << "\n exc:" << exc.displayText();
		}
		catch (...)
		{
		}

		int wait = 5;
		std::cout << "\n waiting " << wait << " seconds...\n";
		Poco::Thread::sleep(1000 * wait);
	}

	
	return 0;
}