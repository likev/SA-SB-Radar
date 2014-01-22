
//SA SB雷达基数据 数据结构
#ifndef _SADATASTRUCT_
#define _SADATASTRUCT_

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>

//DataBase.h
//SA，SB雷达的距离库数
/** /
#define RGates  460				//S BAND 反射率距离库数
#define VGates  920				//S BAND 速度距离库数
#define WGates  920				//S BAND 谱宽距离库数
/ **/

//CB 雷达的距离库数
/*
#define RGates  800				//C BAND 反射率距离库数
#define VGates  1600			//C BAND 速度距离库数
#define WGates  1600			//C BAND 谱宽距离库数
*/



#define VALUE_INVALID -999.		//实际值中的特殊标记,表示无有效观测数据
#define VALUE_RANFOLD  999.		//实际值中的特殊标记,表示有距离模糊
#define VALUE_AINVALID -1;

#define RADIAN  3.14159/180.
#define PI 3.14159
//////////////////////////////////////////////////////////////////////
//雷达基数据结构
template <unsigned RGates_VGates_WGates>
struct RadarBaseData
{
	unsigned short		temp1[7];						//保留
	unsigned short		RadarStatus;					//1 - 表示为雷达数据 
	unsigned short		temp2[6];						//保留
	unsigned int		mSeconds;						//径向数据收集时间(毫秒,自00:00开始)
	unsigned short		JulianDate;						//从1970/1/1起的日期
	unsigned short		URange;							//不模糊距离（表示：数值/10.=千米）
	unsigned short		Az;								//方位角度（编码方式：[数值/8.]*[180./4096.]=度）
	unsigned short		RadialNumber;					//径向数据序号
	unsigned short		RadialStatus;					//径向数据状态
	unsigned short		El;								//仰角（编码方式：[数值/8.]*[180./4096.]=度）
	unsigned short		ElNumber;						//体扫内的仰角编号
	short				RangeToFirstGateOfRef;			//第一个反射率数据表示的实际距离(m)
	short				RangeToFirstGateOfDop;			//第一个多普勒数据表示的实际距离(m)
	unsigned short		GateSizeOfReflectivity;			//反射率数据的距离库长(m)
	unsigned short		GateSizeOfDoppler;				//多普勒数据的距离库长(m)
	unsigned short		GatesNumberOfReflectivity;		//反射率数据的距离库数
	unsigned short		GatesNumberOfDoppler;			//多普勒数据的距离库数
	unsigned short		CutSectorNumber; 				//扇区号
	unsigned int		CalibrationConst; 				//标定常数
	unsigned short		PtrOfReflectivity;				//反射率数据指针
	unsigned short		PtrOfVelocity;					//速度数据指针
	unsigned short		PtrOfSpectrumWidth;				//谱宽数据指针
	unsigned short		ResolutionOfVelocity;			//多普勒速度分辨率:2=0.5m/s;4=1.0m/s
	unsigned short		VcpNumber;						//体扫模式 11： 降水模式，16层仰角 21：降水模式，14层仰角 31：晴空模式，8层仰角 32：晴空模式，7层仰角
	unsigned short		temp4[4];						//保留
	unsigned short		PtrOfArcReflectivity;			//反射率数据指针
	unsigned short		PtrOfArcVelocity;				//速度数据指针
	unsigned short		PtrOfArcWidth;					//谱宽数据指针
	unsigned short		Nyquist;						//不模糊速度（表示：数值/100. = 米/秒）
	unsigned short      temp46;                         //保留
	unsigned short      temp47;                         //保留
	unsigned short      temp48;                         //保留
	unsigned short      CircleTotal;					//仰角数
	unsigned char		temp5[30];						//保留
	unsigned char		Echodata[RGates_VGates_WGates];	//129－2428 共2300字节 (460字节反射率数据+920字节速度数据+920字节谱宽数据)
	unsigned char		temp[4];						//保留

};

class RadarElevation{
public:
	enum CODE_MAEK{
		CODE_INVALID = 0,			//编码值中的特殊标记,表示无有效观测数据
		CODE_RANFOLD = 1          //编码值中的特殊标记,表示有距离模糊
	};

	enum VELOCITY_RESOLUTION {
		RES_POINT_FIVE = 2,		//速度精度类型,代表的精度为0.5 M/S
		RES_ONE_POINT = 4		//速度精度类型,代表的精度为1.0 M/S
	};
public:
	std::vector<std::vector<double> > r, v, w;

	bool r_valid, v_valid, w_valid, 
		r_valid_this, v_valid_this, w_valid_this;

	unsigned r_pos, v_pos, w_pos;

	double Elevation;

	unsigned int r_gate_length, vw_gate_length, 
		r_gate_count, vw_gate_count; 

	double r_first_distance, vw_first_distance;

	int v_resolution;
public:
	RadarElevation():
		r_valid(false), v_valid(false), w_valid(false)
	{
		
	}

	void set_r_gate_count(unsigned rcount)
	{
		r_gate_count = rcount;

		r.assign(360, std::vector<double>(rcount, VALUE_INVALID));
	}

	void set_vw_gate_count(unsigned rcount)
	{
		vw_gate_count = rcount;
		v.assign(360, std::vector<double>(rcount, VALUE_INVALID));
		w.assign(360, std::vector<double>(rcount, VALUE_INVALID));
	}

	bool push_back(unsigned az, unsigned char* pdata)
	{
		if(az >= 360) az -= 360;

		if(r_valid_this) 
		{
			push_back_r( az,  pdata);
		}
		if(v_valid_this) 
		{
			push_back_v( az, pdata);
		}
		if(w_valid_this) 
		{
			push_back_w( az, pdata);
		}

		return true;
	}


private:

	//* 类型：实型，返回解码后的反射率（DBZ）。
	double decode_ref(unsigned char code)
	{
		if(code==CODE_INVALID)		return VALUE_INVALID;
		else if(code==CODE_RANFOLD)	return VALUE_RANFOLD;
		else						
			return (code-2.)/2.-32.5;
	}

	//* 类型：实型，返回解码后的径向速度（M/S）。
	float decode_vel(unsigned char code, short ResType)
	{
		if(code==CODE_INVALID)		return VALUE_INVALID;
		else if(code==CODE_RANFOLD)	return VALUE_RANFOLD;
		else						
		{
			if(ResType==RES_POINT_FIVE) //0.5 m/s 
				return (float((code-2.)/2.-63.5));
			else
				return (float((code-2)-127.));
		}
	}

	//* 类型：实型，返回解码后的谱宽（M/S）。
	float decode_spw(unsigned char code)
	{
		if(code==CODE_INVALID)		return VALUE_INVALID;
		else if(code==CODE_RANFOLD)	return VALUE_RANFOLD;
		else						
			return (float((code-2.)/2.-63.5));
	}
	bool push_back_r(unsigned az, unsigned char* pdata)
	{
		//Get first bin, last bin, and number of bins
		int FstBin = int(r_first_distance/r_gate_length+0.5);
		int BinNum = r_gate_count;
		if(FstBin<0)
		{
			BinNum = FstBin+BinNum;
			FstBin = -1*FstBin;
		}
		int LstBin = FstBin + BinNum;

		//Save data
		for(int BnIndex=FstBin; BnIndex<LstBin; BnIndex++)
		{
			r[az][BnIndex] = decode_ref(pdata[r_pos+BnIndex]);
		}

		return true;
	}
	bool push_back_v(unsigned az, unsigned char* pdata)
	{
		//Get first bin, last bin, and number of bins
		int FstBin = int(vw_first_distance/vw_gate_length+0.5);
		int BinNum = vw_gate_count;
		if(FstBin<0)
		{
			BinNum = FstBin+BinNum;
			FstBin = -1*FstBin;
		}
		int LstBin = FstBin + BinNum;

		//Save data
		for(int BnIndex=FstBin; BnIndex<LstBin; BnIndex++)
		{
			v[az][BnIndex] = decode_vel(pdata[v_pos+BnIndex], v_resolution);
		}
		return true;
	}
	bool push_back_w(unsigned az, unsigned char* pdata)
	{
		//Get first bin, last bin, and number of bins
		int FstBin = int(vw_first_distance/vw_gate_length+0.5);
		int BinNum = vw_gate_count;
		if(FstBin<0)
		{
			BinNum = FstBin+BinNum;
			FstBin = -1*FstBin;
		}
		int LstBin = FstBin + BinNum;

		//Save data
		for(int BnIndex=FstBin; BnIndex<LstBin; BnIndex++)
		{
			w[az][BnIndex] = decode_spw(pdata[w_pos+BnIndex]);
		}
		return true;
	}
};

class SA_SB_Info
{
public:
	typedef RadarBaseData<2300> SA_Base,SB_Base;//RGates+VGates+WGates
	enum RADIAL_STATUS {
		VOL_BEG=3,   //体扫开始状态标志
		VOL_END=4,	 //体扫结束状态标志
		ELV_BEG=0,	 //仰角开始状态标志
		ELV_END=2,   //仰角结束状态标志
		ELV_NORMAL=1
	};

	enum VOL_MODEL {
		RAIN16 = 11,  //降水模式，16层仰角
		RAIN14 = 21,  //降水模式，14层仰角
		SUN8 = 31,  // 晴空模式，8层仰角
		SUN7 = 32 //   晴空模式，7层仰角
	};

public:
	std::vector<SB_Base> alldata;

	class compare_by_double
	{
	public:
		bool operator() (const std::string &left, const std::string &right)const
		{
			double leftd, rightd;
			std::istringstream instr(left+' '+right);
			instr>>leftd>>rightd;

			return leftd < rightd;
		}
	};
	std::map<std::string, RadarElevation, compare_by_double> elevations;

public:
	SA_SB_Info()
	{};
	SA_SB_Info(const std::string& filename)
	{
		read_base_data(filename);
	};

	bool read_base_data(const std::string& filename)
	{
		//"Z_RADR_I_Z9398_20120717041200_O_DOR_SB_CAP.bin"
		std::ifstream fin(filename.c_str(), std::ios_base::in|std::ios_base::binary);

		if(!fin.is_open())
		{
			std::cout<<"打开文件"<<filename<<"失败！"<<std::endl;
		}

		SB_Base oneline;
		alldata.clear();

		while(fin)
		{
			fin.read( (char *)(&oneline), sizeof(SB_Base));
			alldata.push_back(oneline);
		}

		fin.close();

		init_elevations();//初始化各个仰角层 存储于map中

		return true;
	}

	const RadarElevation & operator() (const std::string &el)const
	{
		return elevations.find(el)->second;
	}

	

	void out_info(std::ostream &out)
	{
		out<<
			"从1970/1/1起的日期 "
			"径向数据时间  "
			"体扫模式 "
			"不模糊距离  "
			"序号 "
			"状态  "
			"仰角编号    "
			"仰角    "
			"方位角  "
			"第一个反射率数据表示的实际距离(m) "
			"第一个多普勒数据表示的实际距离(m) "
			"反射率数据的距离库长(m)  "
			"多普勒数据的距离库长(m)  "
			"反射率数据的距离库数   "
			"多普勒数据的距离库数   "
			"反射率数据指针  "
			"速度数据指针   "
			"谱宽数据指针   "
			"多普勒速度分辨率  "
			"反射率数据指针2  "
			"速度数据指针2   "
			"谱宽数据指针2   "
			"不模糊速度  "
			"仰角数  "

			"\n"<<std::endl;
		for(unsigned i=0; i!=alldata.size(); i++)
		{
			SB_Base &cur = alldata[i];
			out<<cur.JulianDate<<"  "
				<<cur.mSeconds<<"   "
				<<get_vol_model(cur.VcpNumber)<<"  "
				<<cur.URange<<"  "
				<<cur.RadialNumber<<"  "
				<<get_radial_status(cur.RadialStatus)<<"   "
				<<cur.ElNumber<<"  "
				<<cur.El<<"  "
				<<cur.Az<<"  "
				<<cur.RangeToFirstGateOfRef<<"   "			//
				<<cur.RangeToFirstGateOfDop<<"   "
				<<cur.GateSizeOfReflectivity<<"   "
				<<cur.GateSizeOfDoppler<<"   "
				<<cur.GatesNumberOfReflectivity<<"   "
				<<cur.GatesNumberOfDoppler<<"  "
				<<cur.PtrOfReflectivity<<"  "
				<<cur.PtrOfVelocity<<"   "
				<<cur.PtrOfSpectrumWidth<<"  "
				<<cur.ResolutionOfVelocity<<"  "
				<<cur.PtrOfArcReflectivity<<"  "
				<<cur.PtrOfArcVelocity<<"   "
				<<cur.PtrOfArcWidth<<"  "
				<<cur.Nyquist<<"  "
				<<cur.CircleTotal<<"  "
				<<std::endl;
		}
	}

private:

	//将仰角转换为小数点后一位的字符串
	std::string trans_to_el(double el)
	{
		std::ostringstream ostr;
		ostr.precision(1);
		ostr<<std::fixed<<el;

		return ostr.str();
	}
	//转换为实际的仰角
	double trans_angle(unsigned short angle)
	{
		return angle/8.0*180.0/4096.0;
	}


	
	bool init_elevations()
	{
		std::string cur_el;
		double angle;

		//循环各个仰角的各个径向
		for(std::vector<SB_Base>::iterator it=alldata.begin(); it != alldata.end(); it++)
		{
			switch(it->RadialStatus)
			{
			case VOL_BEG:

			case ELV_BEG:  //仰角开始
				angle = int(trans_angle(it->El)*10+0.5)/10.0;
				cur_el = trans_to_el(angle);

				elevations[cur_el].Elevation = angle;
				
				if(it->PtrOfReflectivity)
				{
					elevations[cur_el].set_r_gate_count(it->GatesNumberOfReflectivity);

					elevations[cur_el].r_gate_length = it->GateSizeOfReflectivity;
					elevations[cur_el].r_first_distance = it->RangeToFirstGateOfRef;

					elevations[cur_el].r_valid  = true;
					elevations[cur_el].r_pos = it->PtrOfReflectivity - 100;
				}
				if(it->PtrOfVelocity)
				{
					elevations[cur_el].set_vw_gate_count(it->GatesNumberOfDoppler);

					elevations[cur_el].vw_gate_length = it->GateSizeOfDoppler;
					elevations[cur_el].vw_first_distance = it->RangeToFirstGateOfDop;

					elevations[cur_el].v_valid  = true;
					elevations[cur_el].v_pos = it->PtrOfVelocity - 100;

					elevations[cur_el].v_resolution = it->ResolutionOfVelocity;
				}
				if(it->PtrOfSpectrumWidth)
				{
					elevations[cur_el].w_valid  = true;
					elevations[cur_el].w_pos = it->PtrOfSpectrumWidth - 100;
				}

				elevations[cur_el].r_valid_this  = it->PtrOfReflectivity;
				elevations[cur_el].v_valid_this  = it->PtrOfVelocity;
				elevations[cur_el].w_valid_this  = it->PtrOfSpectrumWidth;
				
				break;

			case VOL_END:
				  break;

			case ELV_END:
				  break;
			case ELV_NORMAL:
				break;
			default:
				;
			}
			//当前仰角层上加入本次方位角数据
			elevations[cur_el].push_back(int(trans_angle(it->Az) +0.5), it->Echodata);
		}

		return true;
	}

	//获取径向状态
	std::string get_radial_status(unsigned short RadialStatus)
	{
		std::string status;

		switch(RadialStatus)
		{
		case VOL_BEG:
			status = "VOL_BEG"; break;
		case VOL_END:
			status = "VOL_END"; break;
		case ELV_BEG:
			status = "ELV_BEG"; break;
		case ELV_END:
			status = "ELV_END"; break;
		case ELV_NORMAL:
			status = "Normal"; break;
		default:
			status = "UNKNOW";
		}

		return status;
	}
	//获取体扫模式
	std::string get_vol_model(unsigned short VcpNumber)
	{
		std::string status;

		switch(VcpNumber)
		{
		case RAIN16:
			status = "RAIN16"; break;
		case RAIN14:
			status = "RAIN14"; break;
		case SUN8:
			status = "SUN8"; break;
		case SUN7:
			status = "SUN7"; break;
		default:
			status = "UNKNOW";
		}

		return status;
	}
};

#endif