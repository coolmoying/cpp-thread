#include <list>
#include <iostream>
#include <thread>
#include <mutex>
class SeaKing 
{
public:
	void makeFriend()
	{
		std::lock_guard<std::mutex> sbguard(m_mutex);
		for (int i = 0; i < 100000; i++) 
		{
			std::cout << "增加一个" << std::endl;
			mm.push_back(i);
		}
	}
	bool readInfo() 
	{
		std::lock_guard<std::mutex> sbguard(m_mutex);
		if (!mm.empty())
		{
			std::cout << "减少一个:" << mm.front() << std::endl;
			mm.pop_front();
			return true;
		}
		return false;
	}
	void breakUp() 
	{
		for (int i = 0; i < 100000; i++)
		{
			int result = readInfo();
			if (result == false) 
			{
				std::cout << "已空" << std::endl;
			}
		}
	}
protected:
	std::list<int> mm;
	std::mutex m_mutex;
};
int main() 
{
	SeaKing man;
	std::thread t1(&SeaKing::makeFriend, &man);
	std::thread t2(&SeaKing::breakUp, &man);
	t1.join();
	t2.join();
	return 0;
}