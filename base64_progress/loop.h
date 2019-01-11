#pragma once
#include "pch.h"

class loop {
public:
	bool progress = false;
	std::string indicator = "Progress";
	bool new_line = !one_line;
	bool one_line = !new_line;
	bool infinity = false;
	void cursor();
	void loop_progress(int * active, double maximum);
	template<typename body, typename expr, typename change>
	loop operator()(int n, int m, expr expression, change expr_change, body func_body);
private:
	bool active_cursor = true;
};

template<typename body, typename expr, typename change>
inline loop loop::operator()(int n, int m, expr expression, change expr_change, body func_body)
{
	std::thread *progress_thread = new std::thread();
	if (progress) {
		if (!infinity) {
			progress_thread = new std::thread(&loop::loop_progress, this, &n, m);
		}
		else {
			std::cout << "error: cant show progress to infinity" << std::endl;
		}
	}

	while ((infinity) ? true : expression(n)) {
		func_body(n);
		expr_change(n);
	}

	if (progress_thread->joinable()) { progress_thread->join(); }

	return loop();
}