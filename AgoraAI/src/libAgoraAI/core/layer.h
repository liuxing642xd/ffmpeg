#pragma once

#include <vector>
#include <string>
#include "tensor.h"

using namespace std;

enum OP_TYPE
{
	CONV = 0,
	RELU,
	PAD,
	ELTWISE,
	DECONV,
	CONCAT,
	RESIZE
};


class Layer
{
public:

	Layer(string name = "Unknown") { _name = name; }

	OP_TYPE get_op_type() { return _op_type; }
	std::string get_name() { return _name;  }

	virtual bool run()    = 0;
	virtual bool warmup() = 0;

protected:
	OP_TYPE _op_type;
	std::string _name;

	std::vector<Tensor *> _inputs;
	std::vector<Tensor *> _outputs;
};