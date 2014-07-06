
#include "bzip2/bzlib.h"
#include "radar/SA-SBDataStruct.h"

#include <iostream>
#include <fstream>
#include <string>


int use_high_level_interface()
{
	FILE*   f;
	BZFILE* b;
	int     nBuf = 0;

	char buf[1000];

	int     bzerror;
	int     nWritten;


	if (fopen_s(&f, "sample1.bz2", "r")) {
		/* handle error */
		std::cout << " fopen error!";
		return 1;
	}

	b = BZ2_bzReadOpen(&bzerror, f, 4, 0, NULL, 0);
	if (bzerror != BZ_OK) {
		BZ2_bzReadClose(&bzerror, b);
		/* handle error */
		std::cout << " BZ2_bzReadOpen error!";
		return 2;
	}

	bzerror = BZ_OK;
	while (bzerror == BZ_OK) {
		nBuf = BZ2_bzRead(&bzerror, b, buf, 100);
		std::cout << nBuf;
		if (bzerror == BZ_OK) {
			/* do something with buf[0 .. nBuf-1] */
			std::cout << "sizeof(buf):" << sizeof(buf) << " nbuf:" << nBuf;
		}
		else
		{
			std::cout << "bzerror" << bzerror;
			std::cout << " BZ2_bzRead error!";
			return 3;
		}
	}
	if (bzerror != BZ_STREAM_END) {
		BZ2_bzReadClose(&bzerror, b);
		/* handle error */
		std::cout << " BZ2_bzReadEND error!";
		return 4;
	}
	else {
		BZ2_bzReadClose(&bzerror, b);
	}

	return 0;
}

int use_low_level_interface()
{
	//定义两个字符串分别用来存储压缩字符流 和 输出字符流
	std::string instr(10*1024*1024, ' '), outstr;
	//instr = std::string(100, 'a') + std::string(100, 'b');
	
	//将压缩文件读入instr 压缩前原文件包含100个a和100个b
	std::ifstream fin("Z_RADR_I_Z9398_20140704080600_O_DOR_SB_CAP.bin.bz2", std::ios_base::in | std::ios_base::binary);
	fin.read(&*instr.begin(), instr.size());
	
	instr.resize(fin.gcount());
	//std::cout <<"压缩后字符串:"<<instr;

	fin.close();

	outstr.resize(1000);

	//bz_stream 数据结构用来存储压缩流和解压流相关信息
	bz_stream bsread;
	bsread.bzalloc = NULL;
	bsread.bzfree = NULL;
	bsread.opaque = NULL;

	//初始化bsread 比如设置总输入和输出长度为0 设置压缩状态 
	//第二个参数设置解压状态输出 设为0不输出内部状态
	BZ2_bzDecompressInit(&bsread, 4, 0);
	
	int ret, last_total_out=0;

	//设置输入输出位置和字节数
	bsread.avail_in = instr.size();
	bsread.next_in = &*instr.begin();

	SA_SB_Info radar;

	SA_SB_Info::SB_Base basestruct;

	bsread.avail_out = sizeof(basestruct);//outstr.size()
	bsread.next_out = (char*)(&basestruct);
	
	
	do{//循环解压 每次解压最多70字节存入outstr并输出

	ret = BZ2_bzDecompress(&bsread);
	
	//std::cout << "\n bsread.total_out_hi32:" << bsread.total_out_hi32
	//	<< "\n bsread.total_out_lo32:" << bsread.total_out_lo32;
	//std::cout << " ret:" << ret;

	//std::cout << '\n' << outstr.substr(0, bsread.total_out_lo32 - last_total_out);

	//last_total_out = bsread.total_out_lo32;

	//BZ2_bzDecompress函数每次执行后自动设置avail_in next_in avail_out next_out
	//比如第一次执行到此位置时 avail_in==10 avail_out=0 
	bsread.avail_out = sizeof(basestruct);
	bsread.next_out = (char*)(&basestruct);

	radar.alldata.push_back(basestruct);


	} while (ret == BZ_OK);

	BZ2_bzDecompressEnd(&bsread);

	radar.init_elevs();

	std::cout << "sizeof(RadarBaseData):" << sizeof(SA_SB_Info::SB_Base) << std::endl;
	std::cout << "date_begin:" << radar.date_begin << " date_end:" << radar.date_end << std::endl;
	std::cout << "seconds_begin:" << radar.seconds_begin << " seconds_end:" << radar.seconds_begin << std::endl;

	std::ofstream fout("log2.txt");

	radar.out_info(fout);
	fout.close();
	
	return 0;
	
}

void read_base_data()
{
	std::string filename = "Z_RADR_I_Z9398_20140704080600_O_DOR_SB_CAP.bin";

	SA_SB_Info radar;

	radar.read_base_data(filename);

	std::cout << "sizeof(RadarBaseData):" << sizeof(SA_SB_Info::SB_Base) << std::endl;
	std::cout << "date_begin:" << radar.date_begin << " date_end:" << radar.date_end << std::endl;
	std::cout << "seconds_begin:" << radar.seconds_begin << " seconds_end:" << radar.seconds_begin << std::endl;

	std::ofstream fout("log1.txt");

	radar.out_info(fout);
	fout.close();
}

int main()
{
	use_low_level_interface();
	read_base_data();

	return 0;
}