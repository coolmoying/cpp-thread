#include <thread>
#include <vector>
#include <iostream>
#include <cstdio>
namespace NoAtomic
{
	int num = 0;
	void Sum() 
	{
		for (int i = 0; i < 100000; i++)
		{
			num++;
		}
	}
}
namespace Atomic 
{
	//std::atomic<int> num(0);
	std::atomic_int num = 0;
	void Sum() 
	{
		for (int i = 0; i < 100000; i++)
		{
			num++;
		}
	}
}
void testNoAtomic() 
{
	std::thread test[2];
	for (int i = 0; i < 2; i++) 
	{
		test[i] = std::thread(NoAtomic::Sum);
	}
	for (auto& v : test) 
	{
		v.join();
	}
	std::cout << NoAtomic::num << std::endl;
}
void testAtomic()
{
	std::thread test[2];
	for (int i = 0; i < 2; i++)
	{
		test[i] = std::thread(Atomic::Sum);
	}
	for (auto& v : test)
	{
		v.join();
	}
	std::cout << Atomic::num << std::endl;
}
namespace exOperator 
{
	std::atomic<int> foo(0);
	void set_foo(int x) 
	{
		foo.store(x, std::memory_order_relaxed);
	}
	void print_foo() 
	{
		int x;
		do 
		{
			x = foo.load(std::memory_order_relaxed);
		} while (x == 0);
		std::cout << "foo:" << x << std::endl;
	}
	void testOperator() 
	{
		std::thread t1(set_foo, 1);
		std::thread t2(print_foo);
		t1.join();
		t2.join();
	}

	std::atomic<bool> ready(false);
	std::atomic<bool> winner(false);
	void count(int n) 
	{
		while (!ready) {}  //等待
		for (int i = 0; i < 10000; i++) {}
		if (!winner.exchange(true)) 
		{
			std::cout << "thread:" << n << "完成！\n";
		}
	}
	void testExchange() 
	{
		/*	
			这个例子是同时产生10个线程，让他们开始执行for循环，
			谁先执行完谁就能够第一次调用exchange，从而进入if语句打印信息，其余的都不会进入if，
			因为第一个已经将winner设置为了true，其他线程再访问的时候exchange就会返回true。	
		*/
		std::vector<std::thread> threads;
		for (int i = 0; i < 10; i++) 
		{
			threads.push_back(std::thread(count, i + 1));
		}
		ready.store(true);  //开始
		for (auto& v : threads) 
		{
			v.join();
		}
	}

}
namespace testAtomicFlag 
{
	std::atomic_flag  lock = ATOMIC_FLAG_INIT;
	//std::atomic_flag  lock = {};
	void fun1(int n) 
	{
		while (lock.test_and_set(std::memory_order_acquire)) 
		{
			//std::cout << "等待线程:" << n <<std::endl;
			printf("等待线程:%d\n", n);
		}
		//std::cout << "线程:" << n << "运行" <<std::endl;
		printf("线程启动:%d\n", n);
	}
	void fun2(int n) 
	{
		//std::cout << "线程:" << n << "开始运行\n" << std::endl;
		printf("线程开始启动:%d\n", n);
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
		//可见在thread 2将锁clear之前，线程1一直在执行while
		lock.clear();
		//std::cout << "线程:" << n << "运行中..." << std::endl;
		printf("线程运行结束:%d\n", n);
	}
	void testAtomic_Flag() 
	{
		lock.test_and_set();		//set lock
		std::thread t1(fun1, 1);
		std::thread t2(fun2, 2);
		t1.join();
		t2.join();
	}
}
int main() 
{
	testNoAtomic();
	testAtomic();
	exOperator::testOperator();
	exOperator::testExchange();
	testAtomicFlag::testAtomic_Flag();
	return 0;
}