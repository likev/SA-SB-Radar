#include<iostream>
#include<fstream>


struct A
{

};

struct B
{
		
	int b;
	char c;
	short a;
	char d;
};

struct C
{
		
	int b;
	char c;
	char d;
	short a;
	
};

struct D
{
	char a;
};

template<unsigned int N>
struct E
{
	int a[N];
};

int main()
{
	//std::cout<<"sizeof(A):"<<sizeof(A)
	//	<<"    sizeof(B):"<<sizeof(B)
	//	<<"  sizeof(C):"<<sizeof(C)
	//	<<"  sizeof(D):"<<sizeof(D)<<std::endl;

	//C b={97,98,99,100};	

	//std::cout<<"  sizeof(E<10>):"<<sizeof(E<10>)<<std::endl;
	//	
	//std::ofstream fout("C", std::ios_base::binary);

	//for(int i=0; i<100; i++)
	//{
	//	fout.write((char *)(&b),sizeof(b));
	//}
	//fout.close();

	std::ofstream fout("E:/TDDOWNLOAD/Game.of.Thrones.S03E09.720p.HDTV.x264-EVOLVE.mkv/Game.of.Thrones.S03E09.720p.HDTV.x264-EVOLVE.mkv", std::ios_base::app);

	fout<<"aaaaa";

	return 0;
}