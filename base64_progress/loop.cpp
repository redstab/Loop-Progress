#include "pch.h"
#include "loop.h"

void loop::cursor()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 25;
	info.bVisible = !active_cursor;
	SetConsoleCursorInfo(consoleHandle, &info);
}

void loop::loop_progress(int * active, double maximum)
{
	loop::cursor();
	auto start = std::chrono::high_resolution_clock::now();
	double prev_progress = 0;
	double prev_result = 0;
	double eta = 0;
	double updatethresh = 0.001;
	double stepsto100 = 100 / updatethresh;
	auto FmilliTmin = [](double x) {return x / 1000 / 60; };

	while (*active / maximum != 1) {

		double progress = (100 * (*active / maximum));
		if (progress - prev_progress >= updatethresh) {
			std::string end_string = (new_line) ? "\n" : "\r";
			auto finish = std::chrono::high_resolution_clock::now();
			double diff_from_start = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
			eta = abs((FmilliTmin((diff_from_start - prev_result) * (stepsto100))) - FmilliTmin(diff_from_start));
			std::string current_eta = std::to_string(int(trunc(eta))) + ":" + std::to_string(abs(int(trunc((eta - trunc(eta)) * 60))));
			std::cout << std::setprecision(2) << std::fixed << "Progress: " << progress << "\% eta: " << current_eta << std::string(20, ' ') << ((new_line) ? "\n" : "\r");
			prev_result = diff_from_start;
			prev_progress = progress;
		}

	}
	std::cout << "Progress: " << (100 * (*active / maximum)) << "%" << " eta: zero" << std::endl;
	auto finish = std::chrono::high_resolution_clock::now();
	double s = std::chrono::duration_cast<std::chrono::seconds>(finish - start).count();
	loop::cursor();
}