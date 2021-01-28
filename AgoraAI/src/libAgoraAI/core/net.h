#pragma once

#include <vector>
#include <map>
#include <string>

#include "tensor.h"
#include "layer.h"


class Net
{
public:

	virtual ~Net() { release(); }

	virtual bool run();
	virtual bool warmup();

	bool add_layer(Layer *layer)
	{ 
		_layers.push_back(layer); 
		return true;
	}

	std::vector<Tensor>& get_dynamic_tensors() { return _dynamic_tensors; }
	std::vector<Tensor>& get_static_tensors() { return  _static_tensors; }

protected:
	bool release();

protected:
	std::vector<Layer *>  _layers;
	std::vector<Tensor>   _dynamic_tensors;
	std::vector<Tensor>   _static_tensors;
};