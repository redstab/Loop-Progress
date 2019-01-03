#pragma once
#include "pch.h"

class loop {
public:
	bool progress = false;
	std::string indicator = "Progress";
	bool new_line = false;
	bool percentage = false;
	bool infinity = false;
	void cursor(bool value);
	void prog_loop(int * active, double maximum);
	template<typename body, typename expr, typename change>
	loop operator()(int n, int m, expr expression, change expr_change, body func_body);
};

template<typename body, typename expr, typename change>
inline loop loop::operator()(int n, int m, expr expression, change expr_change, body func_body)
{
	std::thread *t1 = new std::thread();
	if (progress) {
		std::cout << "progress = true" << std::endl;
		t1 = new std::thread(&loop::prog_loop, this, &n, m); // THIS SHIT DRIVES ME INSANE
	}

	while ((infinity) ? true : expression(n)) {
		func_body(n);
		expr_change(n);
	}

	if (t1->joinable()) {t1->join();}

	return loop();
}