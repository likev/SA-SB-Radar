#include <iostream>
#include <fstream>

int main1()
{
	struct A
	{
		
		int b;
		char c[10];short a;
	};

	A m = { 28, "abcde",12};

	std::cout<<m.a<<"  "<<m.b<<"  "<<m.c<<std::endl;
	std::cout<<"sizeof(m):"<<sizeof(m)<<"  sizeof(A):"<<sizeof(A)
		<<"   sizeof(m.a):"<<sizeof(m.a)<<"  sizeof(m.b):"<<sizeof(m.b)
		<<"  sizeof(m.c):"<<sizeof(m.c)<<std::endl;

	std::ofstream fout("struct.bin", std::ios_base::out|std::ios_base::binary);
	fout.write((char *)(&m), sizeof(m));
	fout.close();

	A n;
	std::ifstream fin("struct.bin", std::ios_base::in|std::ios_base::binary);
	fin.read( (char *)(&n), sizeof(n));
	fin.close();
	std::cout<<n.a<<"  "<<n.b<<"  "<<n.c<<std::endl;

	return 0;
}