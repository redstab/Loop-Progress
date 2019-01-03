#include "pch.h"
#include "loop.h"

int main()
{
	int n = 0;
	int m = 100000000000;

	long long d = 0;

	auto expression = [&](int c) {return c < m; };
	auto change = [](int &c) {c++; };
	auto body = [&](int c) {
		d += c * 2;
	};

	loop for_loop;
	for_loop.progress = true;
	for_loop(n, m, expression, change, body);
	std::cout << d << std::endl;
	//loop(n, m, [](int c) {std::cout << c << std::endl; }, [](int c, int m) {return c < m; });
	//int n = 0;
	//int max = INT_MAX;
	//std::thread prog(progress, &n, max);

	//while (n < max) {
	//	n++;
	//}

	//prog.join();
}