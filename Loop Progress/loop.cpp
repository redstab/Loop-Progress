#include "pch.h"
#include "loop.h"
#include <fstream>
#include <chrono>
//template<typename body, typename expr, typename change>
//inline loop::loop(int a, expr expression, change expr_change, body func_body)
//{
//	while (expression(a)) {
//		func_body(a);
//		expr_change(a);
//	}
//
//}

void loop::cursor(bool value)
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 25;
	info.bVisible = value;
	SetConsoleCursorInfo(consoleHandle, &info);
}

void loop::prog_loop(int * active, double maximum)
{
	loop::cursor(false);
	double temp = 0;
	auto start = std::chrono::high_resolution_clock::now();
	double result = 0;
	double eta = 0;
	while (*active / maximum != 1) {
		
		double progress = (100 * (*active / maximum));
		if (progress - temp >= 0.1) {
			auto finish = std::chrono::high_resolution_clock::now();
			double s = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
			eta = (s - result) * (100 / 0.1) / 1000;
			std::cout << progress << "|" << temp << " - " << s - result<< " ms | eta - " << abs(eta / 60) - ((s / 1000) / 60) << " min"<< std::endl;
			result = s;
			temp = progress;
		}
		
		//if (progress != temp) {
		//	std::cout << std::setprecision(4) << std::fixed << indicator << " " << progress << "\% - change: " << progress - temp << std::endl;
		//	
		//	temp = progress;
		//}
	}
	auto finish = std::chrono::high_resolution_clock::now();
	double s = std::chrono::duration_cast<std::chrono::seconds>(finish - start).count();
	std::cout << "Took: "<< s / 60 << "min" << std::endl;
	loop::cursor(true);
}

