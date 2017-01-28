#pragma once

#include "ClUtils.h"
#include <math.h>
#include <string>
#include "utils/String.h"
#include "math/Equation.h"

class CLMath {
private:
	std::string m_eq;
	std::string m_source;
	cl_program m_program;
	cl_kernel m_kernel;
	cl_mem memOut;
	CLInfo* m_info;

	size_t m_outX, m_outY;
public:
	CLMath(Equation* eq, CLInfo* info, size_t outSizex, size_t outSizey);

	void eval(double* out, double x, double y, double xs, double ys, double t, double at);
	static String parseExp(Equation* expr);
};