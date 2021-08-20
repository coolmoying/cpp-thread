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
		while (!ready) {}  //�ȴ�
		for (int i = 0; i < 10000; i++) {}
		if (!winner.exchange(true)) 
		{
			std::cout << "thread:" << n << "��ɣ�\n";
		}
	}
	void testExchange() 
	{
		/*	
			���������ͬʱ����10���̣߳������ǿ�ʼִ��forѭ����
			˭��ִ����˭���ܹ���һ�ε���exchange���Ӷ�����if����ӡ��Ϣ������Ķ��������if��
			��Ϊ��һ���Ѿ���winner����Ϊ��true�������߳��ٷ��ʵ�ʱ��exchange�ͻ᷵��true��	
		*/
		std::vector<std::thread> threads;
		for (int i = 0; i < 10; i++) 
		{
			threads.push_back(std::thread(count, i + 1));
		}
		ready.store(true);  //��ʼ
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
			//std::cout << "�ȴ��߳�:" << n <<std::endl;
			printf("�ȴ��߳�:%d\n", n);
		}
		//std::cout << "�߳�:" << n << "����" <<std::endl;
		printf("�߳�����:%d\n", n);
	}
	void fun2(int n) 
	{
		//std::cout << "�߳�:" << n << "��ʼ����\n" << std::endl;
		printf("�߳̿�ʼ����:%d\n", n);
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
		//�ɼ���thread 2����clear֮ǰ���߳�1һֱ��ִ��while
		lock.clear();
		//std::cout << "�߳�:" << n << "������..." << std::endl;
		printf("�߳����н���:%d\n", n);
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