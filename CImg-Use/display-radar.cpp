#include "radar/SA-SBDataStruct.h"

#define M_PI 3.14159265358979323846

#define cimg_use_png
#include "CImg/CImg.h"

using namespace cimg_library;

#include <algorithm>
#include <numeric>


//获取雷达发射率对应的RGBA值
void get_reflectivity_color(double ref, std::vector<unsigned char> &color)
{

	unsigned char r=0, g=0, b=0, alpha=255;
	if(ref < -5)
	{
		alpha=0;
	}
	else if(ref < 0)
	{
		r = 0, g=172, b=164;
	}
	else if(ref < 5)
	{
		r = 192, g=192, b=254;
	}
	else if(ref < 10)
	{
		r = 112, g=114, b=238;
	}
	else if(ref < 15)
	{
		r = 30, g=38, b=208;
	}
	else if(ref < 20)
	{
		r = 166, g=252, b=168;
	}
	else if(ref < 25)
	{
		r = 0, g=234, b=0;
	}
	else if(ref < 30)
	{
		r = 16, g=146, b=26;
	}
	else if(ref < 35)
	{
		r = 252, g=244, b=100;
	}
	else if(ref < 40)
	{
		r = 200, g=200, b=2;
	}
	else if(ref < 45)
	{
		r = 140, g=140, b=0;
	}
	else if(ref < 50)
	{
		r = 254, g=172, b=172;
	}
	else if(ref < 55)
	{
		r = 254, g=100, b=92;
	}
	else if(ref < 60)
	{
		r = 238, g=2, b=48;
	}
	else if(ref < 65)
	{
		r = 212, g=142, b=254;
	}
	else if(ref < 900)
	{
		r = 170, g=36, b=250;
	}
	else
	{
		alpha=0;
	}

	color.resize(4, 0);
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = alpha;

	return;
}

//填充一个区域 像素x坐标属于[x1, x2) 像素y坐标属于[y1, y2)
void fill_rect(CImg<unsigned char> &img, 
	unsigned x1,unsigned y1, unsigned x2,unsigned y2, 
	const std::vector<unsigned char> &color)
{
	if(x1 > x2) std::swap(x1, x2);
	if(y1 > y2) std::swap(y1, y2);

	for(unsigned i=x1; i<x2; i++)
	{
		for(unsigned j=y1; j<y2; j++)
		{
			for(std::size_t v=0; v<color.size() && v<4; v++)
			{
				img.atXY(i, j, 0, v) = color[v];//越界将忽略
			}
		}
	}
}
//填充一组指定坐标的像素点
void fill_rect(CImg<unsigned char> &img, 
	std::vector<std::pair<int, int> > &inrect,
	const std::vector<unsigned char> &color)
{
	for(std::size_t i = 0; i<inrect.size(); i++)
	{
		for(std::size_t v=0; v<color.size() && v<4; v++)
		{
			img.atXY(inrect[i].first, inrect[i].second, 0, v) = color[v];//越界将忽略
		}
	}
}

//填充像素坐标的颜色
void fill_rect(CImg<unsigned char> &img,
	const std::vector<std::vector<double> > &pixel_coord)
{
	std::vector<unsigned char> color;
	
	for(std::size_t i = 0; i<pixel_coord.size(); i++)
	{
		for(std::size_t j = 0; j<pixel_coord[i].size(); j++)
		{
			get_reflectivity_color(pixel_coord[i][j], color);
			for(std::size_t v=0; v<color.size() && v<4; v++)
			{
				img.atXY(i, j, 0, v) = color[v];//越界将忽略
			}
		}
	}
}


class Vector
{
public:
	double x,y;

	Vector(){}
	Vector(double inx, double iny)
	{
		x = inx;
		y = iny;
	}

	double operator* (Vector & right)
	{
		return x*right.y - right.x*y;
	}

	double dot(Vector & right)
	{
		return x*right.x + y*right.y;
	}
};

//获取两个径向和两个半径包围的区域坐标
void get_inner_point(std::vector<double> &x, std::vector<double> &y,
	std::vector<std::pair<int, int> > &region)
{
	//double x0,double y0,double x01,double y01,
	//double x1,double y1,double x11,double y11,
	int xmax = int(*std::max_element(x.begin(), x.end()) ),
		xmin = int(*std::min_element(x.begin(), x.end() ) ) ,
		ymax = int(*std::max_element(y.begin(), y.end() ) ) ,
		ymin =  int(*std::min_element(y.begin(), y.end() ) ) ;

	double r1r1 = x[0]*x[0] + y[0]*y[0],
		r2r2 = x[1]*x[1] + y[1]*y[1];

	region.resize(0);
	for(int xx = xmin-1; xx <= xmax+1; xx++)
	{
		for(int yy = ymin-1; yy <= ymax+1; yy++)
		{
			if(xx*xx + yy*yy >= r1r1 && xx*xx + yy*yy < r2r2)
			{
				Vector v1(x[0], y[0]), v2(xx, yy), v3(x[2],y[2]);
				if(v1*v2 == 0 || (v1*v2)*(v2*v3)>0 )
				{
					region.push_back(std::pair<int, int>(xx,yy) );
				}
			}
		}
	}

}

//将坐标值进行偏移
void set_region_offset(std::vector<std::pair<int, int> > &region, 
	const std::pair<int, int> &offset)
{
	for(std::size_t i = 0; i<region.size(); i++)
	{
		region[i].first += offset.first;
		region[i].second += offset.second;
	}
}

//对像素坐标的发射率进行插值以消除大反射率中间的空白区
void radar_interpolation(std::vector<std::vector<double> > &pixel_coord)
{
	for(std::size_t i = 1; i<pixel_coord.size()-1; i++)
	{
		for(std::size_t j = 1; j<pixel_coord[i].size()-1; j++)
		{
			if(pixel_coord[i][j] < -900)
			{
				std::vector<double> value;

				if(pixel_coord[i][j-1] > 5) value.push_back(pixel_coord[i][j-1]);
				if(pixel_coord[i][j+1] > 5) value.push_back(pixel_coord[i][j+1]);
				if(pixel_coord[i-1][j] > 5) value.push_back(pixel_coord[i-1][j]);
				if(pixel_coord[i+1][j] > 5) value.push_back(pixel_coord[i+1][j]);

				if(value.size() >= 3)
				{
					pixel_coord[i][j] = std::accumulate(value.begin(), value.end(), 0.0)/value.size();
				}
			}
		}
	}
}

//将一个仰角层保存为图片
bool save_png(const RadarElevation &el, const std::string &filename)
{
	const double RADIAN = M_PI/180.0;

	CImg<unsigned char> img(el.r_gate_count*2,el.r_gate_count*2,1,4,0);

	std::vector<std::vector<double> > pixel_coord(el.r_gate_count*2, std::vector<double>(el.r_gate_count*2, -999) );


	std::vector<std::pair<int, int> > region;
	std::vector<double> x(4), y(4);

	double sini, sini1=0, cosi, cosi1=1;
	double  lcosel = el.r_gate_length*cos(el.Elevation*RADIAN)/1000.0;//将库长转换为地面上对应的长度(Km)

	for(std::size_t i=0; i<el.r.size(); i++)//方位角
	{
		double r = 0 ;
		sini = sini1,  cosi = cosi1;
		sini1 = sin((i+1)*RADIAN), cosi1 = cos((i+1)*RADIAN);

		for(std::size_t j=0; j<el.r[i].size(); j++)//第j个库
		{

			if(el.r[i][j] > 900 || el.r[i][j] < 0)
			{
					r += lcosel;
					continue;
			}

			x[0] =  r*sini, 
			y[0] = -r*cosi;

			
			if(j<60)
			{
				int posx = int(x[0]+0.5) + el.r_gate_count, posy = int(y[0]+0.5) + el.r_gate_count;

				if(el.r[i][j] > pixel_coord[posx][posy])//当多个库落在同一坐标时选取最大的值
				{
					pixel_coord[posx][posy] = el.r[i][j];
				}
			}

			else
			{
				x[1]= x[0] + lcosel*sini, 
				y[1] = y[0] - lcosel*cosi,

				x[2] = r*sini1,
				y[2] = -r*cosi1,
				x[3]= x[2] + lcosel*sini1, 
				y[3] = y[2] - lcosel*cosi1;

				get_inner_point(x, y, region);
				
				set_region_offset(region, std::pair<int, int>(el.r_gate_count, el.r_gate_count));
				
				//fill_rect(img, region, color);
				for(std::size_t k = 0; k<region.size(); k++)
				{
					pixel_coord[region[k].first ][region[k].second ] = el.r[i][j];
				}
			}

			r += lcosel;
		}
	}

	//radar_interpolation(pixel_coord);

	fill_rect(img, pixel_coord);
	img.save(filename.c_str());

	return true;
}

int main3()
{
	double a = -1.2, b=-1.8, c=1.2, d=1.8;

	std::cout<<a<<' '<<int(a)<<' '
		<<b<<' '<<int(b)<<' '
		<<c<<' '<<int(c)<<' '
		<<d<<' '<<int(d)<<std::endl;
	return 0;
}

int main2()
{
	CImg<unsigned char> img(920,920,1,4,100);

	std::vector<double> x, y;
	double s3 = std::sqrt(3.0);
	x.push_back(s3*200);x.push_back(s3*400);x.push_back(200);x.push_back(400);
	y.push_back(200);y.push_back(400);y.push_back(s3*200);y.push_back(s3*400);

	std::vector<std::pair<int, int> > result;

	get_inner_point(x,y,result);

	for(std::size_t i = 0; i<result.size(); i++)
	{
		img(result[i].first, result[i].second, 2) = 250;
	}

	img.display("My first CImg code");

	return 0;
}

//将一个仰角层保存为包含数据的图片
bool save_png_data(const RadarElevation &el, const std::string &filename)
{
	CImg<unsigned char> img(360, el.r_gate_count, 1, 1, 0);

	for(std::size_t i=0; i<el.r.size(); i++)//方位角
	{
		for(std::size_t j=0; j<el.r[i].size(); j++)//第j个库
		{
			int ref = int(el.r[i][j]+0.5);
			unsigned char val;
			
			if(ref < 0) val=254; //无有效观测数据
			else if(ref > 255) val = 255; //表示有距离模糊
			else val = ref;

			img.atXY(i, j, 0, 0) = val;
		}
	}

	img.save(filename.c_str());

	return true;
}

int main()
{
	std::string filename = "Z_RADR_I_Z9398_20120724103900_O_DOR_SB_CAP.bin";

	while(std::cin>>filename)
	{
		SA_SB_Info radar(filename);

		for(SA_SB_Info::r_it it = radar.elevations.begin();
			it != radar.elevations.end(); it++)
		{
			if(it->second.r_valid)
			{
				save_png_data(it->second, it->first + "data.png");
			}
		}
	}

	return 0;
}