#include <iostream>
#include <fstream>

int main3()
{
	
	char a[20]={'a','b','c'};
	char *p1 = a,
		 
		*p3=&(a[0]),
		*p4=&a[0];
	char (*p2)[20] =&a;
	std::cout<<"a:"<<int(p1)<<"  &a:"<<int(p2)<<"  &(a[0])"<<int(p3)<<"  &a[0]"<<int(p4)<<std::endl;

	std::ifstream fin("abc.txt", std::ios_base::in|std::ios_base::binary);
	fin.read( (char *)(a), sizeof(3));
	fin.close();
	std::cout<<a<<std::endl;

	return 0;
}