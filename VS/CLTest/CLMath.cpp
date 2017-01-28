#include "CLMath.h"
#include <iostream>

#define NODE_TYPE_OPP 1
#define NODE_TYPE_NUM 2
#define NODE_TYPE_FUN 3
#define NODE_TYPE_FSP 4
#define NODE_TYPE_VAR 5
#define NODE_TYPE_FCL 6
#define NODE_TYPE_EXP 7
#define NODE_TYPE_FFN 8
#define NODE_TYPE_ZRO 9

typedef struct Node {
	int type;
	int childNum;
	Node** children;
	void** value;
} Node;

String getNodeString(Node* node);

CLMath::CLMath(Equation* eq, CLInfo* info, size_t outSizex, size_t outSizey) {
	String temp = parseExp(eq);
	m_eq = temp.getstdstring();

	m_info = info;
	m_outX = outSizex;
	m_outY = outSizey;

	m_source = "";

	m_source += "double cot(double num) {\n";
	m_source += "return 1/tan(num);\n";
	m_source += "}\n";

	m_source += "double csc(double num) {\n";
	m_source += "return 1/sin(num);\n";
	m_source += "}\n";

	m_source += "double sec(double num) {\n";
	m_source += "return 1/cos(num);\n";
	m_source += "}\n";

	m_source += "double acot(double num) {\n";
	m_source += "return atan(1 / num);\n";
	m_source += "}\n";

	m_source += "double acsc(double num) {\n";
	m_source += "return atan(1 / num);\n";
	m_source += "}\n";

	m_source += "double asec(double num) {\n";
	m_source += "return atan(1 / num);\n";
	m_source += "}\n";

	m_source += "double abs_c(double num) {\n";
	m_source += "return num > 0 ? num : -(num);\n";
	m_source += "}\n";

	m_source += "double fact(double num) {\n";
	m_source += "return lgamma(num + 1);\n";
	m_source += "}\n";

	m_source += "__constant double e = 2.718281828459045;\n";
	m_source += "__constant double pi = 3.141592653589793;\n";
	m_source += "__constant double tau = 3.141592653589793 * 2;\n";

	m_source += "__kernel void math_kernel(";
	m_source += "double a_x, ";
	m_source += "double a_y, ";
	m_source += "double a_xs, ";
	m_source += "double a_ys, ";
	m_source += "double t, ";
	m_source += "double at, ";
	m_source += "__global double *Out";
	m_source += ") {\n";
	
	m_source += "double ix = get_global_id(0);\n";
	m_source += "double iy = get_global_id(1);\n";

	m_source += "double x = a_x + a_xs * (ix/(get_num_groups(0) * get_local_size(0)));\n";
	m_source += "double y = a_y + a_ys * (iy/(get_num_groups(0) * get_local_size(0)));\n";
	m_source += "int index = ix * get_num_groups(0) * get_local_size(0) + iy;\n";

	m_source += "Out[index] = sign(";
	m_source += m_eq;
	m_source += ");\n";

	m_source += "}\n";

	cl_int clStatus;

	memOut = clCreateBuffer(info->context, CL_MEM_WRITE_ONLY,
		outSizex * outSizey * sizeof(double), NULL, &clStatus);

	const char* src = m_source.c_str();

	m_program = clCreateProgramWithSource(info->context, 1,
		(const char **)&src, NULL, &clStatus);
	
	clStatus = clBuildProgram(m_program, 1, info->device_list,
		NULL, NULL, NULL);

	if (clStatus == CL_BUILD_PROGRAM_FAILURE) {
		size_t log_size;
		clGetProgramBuildInfo(m_program, info->device_list[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		char *log = (char *)malloc(log_size);
		clGetProgramBuildInfo(m_program, info->device_list[0], CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
		printf("%s\n", log);
	}

	m_kernel = clCreateKernel(m_program, "math_kernel", &clStatus);
	
	clStatus = clSetKernelArg(m_kernel, 6, sizeof(cl_mem),
		(void *)&memOut);
}

void CLMath::eval(double* out, double x, double y, double xs, double ys, double t, double at) {
	cl_int clStatus;

	clStatus = clSetKernelArg(m_kernel, 0, sizeof(double),
		(void *)&x);

	clStatus = clSetKernelArg(m_kernel, 1, sizeof(double),
		(void *)&y);
	
	clStatus = clSetKernelArg(m_kernel, 2, sizeof(double),
		(void *)&xs);

	clStatus = clSetKernelArg(m_kernel, 3, sizeof(double),
		(void *)&ys);

	clStatus = clSetKernelArg(m_kernel, 4, sizeof(double),
		(void *)&t);

	clStatus = clSetKernelArg(m_kernel, 5, sizeof(double),
		(void *)&at);

	size_t global_size[2] = { m_outX, m_outY };
	size_t local_size[2] = { 8, 8 };

	clStatus = clEnqueueNDRangeKernel(m_info->command_queue, m_kernel, 2,
		NULL, global_size, local_size, 0, NULL, NULL);

	clStatus = clEnqueueReadBuffer(m_info->command_queue, memOut,
		CL_TRUE, 0, m_outX * m_outY * sizeof(double), out, 0, NULL, NULL);
	
	//std::cout << clStatus << "\n";
}

String CLMath::parseExp(Equation* expr) {
	return getNodeString((Node*)expr->getRootNode());
}

String getNodeString(Node* node) {
	if (node->type == NODE_TYPE_ZRO) {
		return "0";
	}
	else if (node->type == NODE_TYPE_NUM) {
		return String(std::to_string(((double*)(node->value[0]))[0]));
	}
	else if (node->type == NODE_TYPE_VAR) {
		Variable* var = (Variable*)node->value[0];
		return *var->name;
	}
	else if (node->type == NODE_TYPE_OPP) {
		Operator* op = (Operator*)node->value[1];
		Node* prev = node->children[0];
		Node* next = node->children[1];

		String prevVal = getNodeString(prev);
		String nextVal = getNodeString(next);

		String result;

		if (op->name == '+') {
			result = "(" + prevVal + ")+(" + nextVal + ")";
		} else if (op->name == '-') {
			result = "(" + prevVal + ")-(" + nextVal + ")";
		} else if (op->name == '*') {
			result = "(" + prevVal + ")*(" + nextVal + ")";
		} else if (op->name == '/') {
			result = "(" + prevVal + ")/(" + nextVal + ")";
		} else if (op->name == '=') {
			result = "(" + prevVal + ")-(" + nextVal + ")";
		} else if (op->name == '^') {
			result = "pow(" + prevVal + "," + nextVal + ")";
		}
		return result;
	}
	else if (node->type == NODE_TYPE_FFN) {
		Function* func = (Function*)node->value[0];
		if (func == NULL) {
			return getNodeString(node->children[0]);
		}

		String* values = new String[node->childNum];

		for (int i = 0; i < node->childNum; i++) {
			values[i] = getNodeString(node->children[i]);
		}

		String arg = "";
		for (int i = 0; i < node->childNum-1; i++) {
			arg = arg + values[i] + ",";
		}

		arg = arg + values[node->childNum - 1];

		String name = *func->name;

		if (name == "max") {
			name = "fmax";
		} else if (name == "min") {
			name = "fmin";
		} else if (name == "ln") {
			name = "log";
		} else if (name == "log") {
			name = "log10";
		} else if (name == "gamma") {
			name = "lgamma";
		} else if (name == "abs") {
			name = "abs_c";
		}

		String result = name + "(" + arg + ")";
		return result;
	}

	return "";
}