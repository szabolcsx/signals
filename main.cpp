#include "signal.h"

#include <iostream>
#include <cstdlib>

void func(int x)
{
	std::cout << "void func(" << x << ")" << std::endl;
}

class foo : public szabi::signals::auto_disconnect
{
public:
	foo() {}
	~foo() {}

	void func(int x)
	{
		std::cout << "void foo::func(" << x << ")" << std::endl;
	}

	void func(int x, int y)
	{
		std::cout << "void foo::func(" << x << ", " << y << ")" << std::endl;
	}

	void func0()
	{
		std::cout << "void foo::func0()" << std::endl;
	}

	void func1(int x)
	{
		std::cout << "void foo::func1(" << x << ")" << std::endl;
	}

	void func2(int x, int y)
	{
		std::cout << "void foo::func2(" << x << ", " << y << ")" << std::endl;
	}
};

class bad : public szabi::signals::auto_disconnect
{
public:
	bad()
	{
		std::cout << "bad constructed" << std::endl;
	}
	~bad()
	{
		std::cout << "bad destructed" << std::endl;
	}

	void func(int)
	{
		std::cout << std::endl << "void bad::func()" << std::endl << std::endl;
	}
};

void should_not_fire()
{
	std::cout << "void should_not_fire()" << std::endl;
}

int main()
{
	szabi::signal<> signal0;
	szabi::signal<int> signal1;
	szabi::signal<int, int> signal2;
	foo f;
	signal0.connect(&foo::func0, f);
	signal1.connect([](int x)
	{
		std::cout << "Lambda func, x = " << x << std::endl;
	});

	signal1.connect(&func);

	// overloaded slots
	signal1.connect(static_cast<void(foo::*)(int)>(&foo::func), f);
	signal2.connect(static_cast<void(foo::*)(int, int)>(&foo::func), f);
	// using overload<...>::of(&foo::bar) helper
	signal1.connect(szabi::overload<int>::of(&foo::func), f);
	signal2.connect(szabi::overload<int, int>::of(&foo::func), f);

	signal1.connect(&foo::func1, &f);
	signal2.connect(&foo::func2, &f);

	{
		bad b;
		auto conn = signal0.connect(&should_not_fire);
		conn.disconnect();

		if (!conn.connected())
		{
			std::cout << "disconnected" << std::endl;
		}

		signal1.connect(&bad::func, b);
	}

	signal0();
	signal1(1);
	signal2(1, 2);

	return EXIT_SUCCESS;
}
