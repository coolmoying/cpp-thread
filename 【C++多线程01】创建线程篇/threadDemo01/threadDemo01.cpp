#include <thread>
#include <iostream>
#include <thread>
class MM 
{
public:
	void print(int& num) 
	{
		num = 1001;
		std::cout << "子线程:"<<num << std::endl;
	}
};
int main() 
{
	MM mm;
	int num = 10;
	std::thread t(&MM::print,mm,std::ref(num));				
	t.join();
	std::cout << "主线程:"<< num << std::endl;
	return 0;
}