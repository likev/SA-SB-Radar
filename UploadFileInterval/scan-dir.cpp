#include <iostream>
#include <string>
#include <algorithm>

#include "Poco/Glob.h"
#include "Poco/File.h"


void post_file(Poco::File& cur)
{


}

bool check_file(Poco::File& cur)
{

	return true;
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



void scan_dir()
{
	std::string path = "dirtest/*.txt";


	using Poco::Glob;

	std::set<std::string> files;
	

	Glob::glob(path.c_str(), files);
	std::vector<std::string> vfiles(files.begin(), files.end());

	std::sort(vfiles.begin(), vfiles.end(), CmpByLastModified);

	// Glob::glob("/usr/include/*/*.h", files);

	auto it = files.begin();
	for (; it != files.end(); ++it)
	{
		std::cout << *it << std::endl;
		Poco::File cur(*it);
		std::cout<<cur.getLastModified().epochTime()<<std::endl;

		if (check_file(cur))
		{
			post_file(cur);
		}

	}

	auto it2 = vfiles.begin();
	for (; it2 != vfiles.end(); ++it2)
	{
		std::cout << *it2 << std::endl;
		Poco::File cur(*it2);
		std::cout << cur.getLastModified().epochTime() << std::endl;

		if (check_file(cur))
		{
			post_file(cur);
		}

	}

}

int main()
{
	scan_dir();
	return 0;
}