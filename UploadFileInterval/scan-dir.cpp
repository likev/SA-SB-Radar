

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
#include <sstream>
#include <algorithm>






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


bool CmpByLastModified(const std::string& left,
	const std::string& right)
{
	Poco::File leftf(left), rightf(right);

	return leftf.getLastModified() < rightf.getLastModified();

}



void handle_file(Poco::File& cur)
{
	UploadInfo upinfo;
	generate_pngdata( cur, upinfo );
	
	post_file(upinfo);
}


void scan_dir(const std::string & dir)
{
	//"dirtest/*.txt";
	std::string path = getScanPath(dir);


	using Poco::Glob;

	std::set<std::string> files;

	Glob::glob(path.c_str(), files);


	std::vector<std::string> vfiles(files.begin(), files.end());

	std::sort(vfiles.begin(), vfiles.end(), CmpByLastModified);

	auto it2 = vfiles.begin();
	for (; it2 != vfiles.end(); ++it2)
	{
		Poco::File cur(*it2);

		if (check_file(cur))
		{
			std::cout << "\n发现新文件:" << cur.path();
			handle_file(cur);
			

			setLastTimeStr("sanmenxia", to_string(cur.getLastModified().epochTime() ) );
		}

	}

}

int main()
{
	curl_global_init(CURL_GLOBAL_ALL);
	
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

	curl_global_cleanup();
	return 0;
}