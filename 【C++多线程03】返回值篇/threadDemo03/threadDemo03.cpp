#include <iostream>
#include <string>
#include <thread>
#include <future>
void mythread(std::promise<int>& temp, int data) 
{	
	data *= 10;
	//std::this_thread::get_id()：获取当前线程id
	std::cout << "ID:" << std::this_thread::get_id() << " thread start" << std::endl;
	//持续时间5秒
	std::chrono::milliseconds duration(5000);
	//延时5秒
	std::this_thread::sleep_for(duration);
	temp.set_value(data);
	std::cout << "ID:" << std::this_thread::get_id() << " thread end" << std::endl;
}
void mythread2(std::future<int>& temp) 
{
	auto result = temp.get();
	std::cout << "ID:" << std::this_thread::get_id() << " thread start" << std::endl;
	std::cout << "result:"<<result << std::endl;
	std::cout << "ID:" << std::this_thread::get_id() << " thread end" << std::endl;
}
int main() 
{
	std::cout << "main ID:" << std::this_thread::get_id() << std::endl;
	std::promise<int> data;
	std::thread t1(mythread, std::ref(data),100);
	t1.join();
	std::future<int> result = data.get_future();
	std::thread t2(mythread2, std::ref(result));
	t2.join();
	std::cout << "test.....!" << std::endl;
	std::cout << "main end" << std::endl;
	return 0;
}