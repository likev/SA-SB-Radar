#include "SA-SBDataStruct.h"
#include <string>
#include <fstream>
#include <iostream>



int main()
{
	std::string filename = "Z_RADR_I_Z9398_20120717041200_O_DOR_SB_CAP.bin";

	SA_SB_Info radar;
	
	radar.read_base_data(filename);

	std::cout<<"sizeof(RadarBaseData):"<<sizeof(SA_SB_Info::SB_Base )<<std::endl;

	std::ofstream fout("log.txt");

	radar.out_info(fout);
}