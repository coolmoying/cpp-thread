#include <queue>
#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <iostream> 
using namespace std;
template <class T>
class thread_safe_queue
{
private:
	mutable mutex mut;
	queue<T> data_queue;
	condition_variable data_cond;
public:
	thread_safe_queue() {}
	thread_safe_queue(thread_safe_queue const& other)
	{
		lock_guard<mutex> lk(other.mut);
		data_queue = other.data_queue;
	}
	void push(T new_value)
	{
		lock_guard<mutex> lk(mut);
		data_queue.push(new_value);
		data_cond.notify_one();			//随机唤醒一个线程
	}
	void wait_and_pop(T& value)
	{
		unique_lock<mutex> lk(mut);
		data_cond.wait(lk, [this] {return !data_queue.empty(); });
		value = data_queue.front();
		data_queue.pop();
	}
	shared_ptr<T> wait_and_pop()
	{
		unique_lock<mutex> lk(mut);
		data_cond.wait(lk, [this] {return !data_queue.emtpy(); });
		shared_ptr<T> res(make_shared<T>(data_queue.front));
		data_queue.pop();
		return res;
	}
	bool try_pop(T& value)
	{
		lock_guard<mutex> lk(mut);
		if (data_queue.empty())
		{
			return false;
		}
		value = data_queue.front();
		data_queue.pop();
		return true;
	}
	shared_ptr<T> try_pop()
	{
		lock_guard<mutex> lk(mut);
		if (data_queue.empty()) 
		{
			return shared_ptr<T>();
		}
		shared_ptr<T> res(make_shared<T>(data_queue.front()));
		data_queue.pop();
		return res;
	}
	bool empty() const 
	{
		lock_guard<mutex> lk(mut);
		return data_queue.empty();
	}

};
void insertData(thread_safe_queue<int>& safeQueue,  int data) 
{
	safeQueue.push(data);
}
int main() 
{
	thread_safe_queue<int> safeQueue;
	int num = 0;
	for (int i = 0; i < 10; i++) 
	{
		thread t1(insertData, ref(safeQueue), i);
		t1.detach();
	}
	this_thread::sleep_for(chrono::seconds(3));
	while (!safeQueue.empty()) 
	{
		safeQueue.wait_and_pop(num);
		cout << num << "\t";
	}

	return 0;
}