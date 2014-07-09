#include "radar.h"

#include "bzip2/bzlib.h"
#include "radar/SA-SBDataStruct.h"

#define cimg_use_png
#include "CImg/CImg.h"

using namespace cimg_library;


#include "Poco/File.h"
#include "Poco/Path.h"

#include <iostream>
#include <fstream>
#include <string>


bool read_bz2_file(const std::string& filename, SA_SB_Info& radar)
{
	//定义两个字符串分别用来存储压缩字符流 和 输出字符流
	std::string instr(10 * 1024 * 1024, ' ');

	//将压缩文件读入instr "Z_RADR_I_Z9398_20140704080600_O_DOR_SB_CAP.bin.bz2"
	std::ifstream fin(filename, std::ios_base::in | std::ios_base::binary);
	fin.read(&*instr.begin(), instr.size());

	instr.resize(fin.gcount());
	//std::cout <<"压缩后字符串:"<<instr;

	fin.close();

	//bz_stream 数据结构用来存储压缩流和解压流相关信息
	bz_stream bsread;
	bsread.bzalloc = NULL;
	bsread.bzfree = NULL;
	bsread.opaque = NULL;

	//第二个参数设置解压状态输出 设为0不输出内部状态
	int error = BZ2_bzDecompressInit(&bsread, 0, 0);
	//std::cout << "BZ2_bzDecompressInit return code:" << error << '\n';

	int ret, last_total_out = 0;

	//设置输入输出位置和字节数
	bsread.avail_in = instr.size();
	bsread.next_in = &*instr.begin();


	SA_SB_Info::SB_Base basestruct;

	do{//循环解压 每次解压最多70字节存入outstr并输出

		bsread.avail_out = sizeof(basestruct);//outstr.size()
		bsread.next_out = (char*)(&basestruct);

		ret = BZ2_bzDecompress(&bsread);

		if (ret == BZ_OK || ret == BZ_STREAM_END)
		{
			radar.alldata.push_back(basestruct);
		}
		else
		{
			applog << "\n BZ2_bzDecompress error code:" << ret << '\n';
			return false;
		}


	} while (ret == BZ_OK);

	BZ2_bzDecompressEnd(&bsread);

	radar.init_elevs();

	//std::cout << "sizeof(RadarBaseData):" << sizeof(SA_SB_Info::SB_Base) << '\n';
	//std::cout << "date_begin:" << radar.date_begin << " date_end:" << radar.date_end << '\n';
	//std::cout << "seconds_begin:" << radar.seconds_begin << " seconds_end:" << radar.seconds_begin << '\n';

	//std::ofstream fout("log2.txt");

	//radar.out_info(fout);
	//fout.close();

	return true;

}

void read_base_data()
{
	std::string filename = "Z_RADR_I_Z9398_20140704080600_O_DOR_SB_CAP.bin";

	SA_SB_Info radar;

	radar.read_base_data(filename);

	applog << "sizeof(RadarBaseData):" << sizeof(SA_SB_Info::SB_Base) << '\n';
	applog << "date_begin:" << radar.date_begin << " date_end:" << radar.date_end << '\n';
	applog << "seconds_begin:" << radar.seconds_begin << " seconds_end:" << radar.seconds_begin << '\n';

	std::ofstream fout("log1.txt");

	radar.out_info(fout);
	fout.close();
}

//将一个仰角层保存为包含数据的图片
bool save_png_data(const RadarElevation &el, const std::string &filename)
{
	CImg<unsigned char> img(360, el.r_gate_count, 1, 1, 0);

	for (std::size_t i = 0; i < el.r.size(); i++)//方位角
	{
		for (std::size_t j = 0; j < el.r[i].size(); j++)//第j个库
		{
			int ref = int(el.r[i][j] + 0.5);
			unsigned char val;

			if (ref < 0) val = 254; //无有效观测数据
			else if (ref > 255) val = 255; //表示有距离模糊
			else val = ref;

			img.atXY(i, j, 0, 0) = val;
		}
	}

	img.save(filename.c_str());

	return true;
}

bool generate_pngdata(Poco::File& cur, UploadInfo& upinfo)
{
	bool result = true;
	SA_SB_Info radar;
	std::string pathstr = cur.path();

	Poco::Path curpath(pathstr);
	if (curpath.getExtension() == "bz2")
	{
		if (!read_bz2_file(pathstr, radar)) return false;
	}
	else
	{
		if (!radar.read_base_data(pathstr))
		{
			applog << "\n read radar base data error!:" << '\n';
			return false;
		}
	}

	upinfo.date_begin = radar.date_begin;
	upinfo.date_end = radar.date_end;
	upinfo.seconds_begin = radar.seconds_begin;
	upinfo.seconds_end = radar.seconds_end;

	for (SA_SB_Info::r_it it = radar.elevations.begin();
		it != radar.elevations.end(); it++)
	{
		if (it->second.r_valid)
		{
			std::string outname = getPngOutDir() + '/' + it->first + "datar.png";

			save_png_data(it->second, outname);

			upinfo.elevs.push_back(it->first);
			upinfo.rfiles.push_back(outname);

		}
	}

	return result;
}