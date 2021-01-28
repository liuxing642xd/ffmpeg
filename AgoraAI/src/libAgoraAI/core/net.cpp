

#include "net.h"
#include "layer.h"
#include "utils.h"

bool Net::run()
{
	bool res = true;
	for (size_t i = 0; i < _layers.size(); i++)
	{
		TimeProfiler t(_layers[i]->get_name());
		res &= _layers[i]->run();
	}

	return res;
}

bool Net::warmup()
{
	bool res = true;
	for (size_t i = 0; i < _layers.size(); i++)
	{
		res &= _layers[i]->warmup();
	}

	return res;
}

bool Net::release()
{
	for (size_t i = 0; i < _layers.size(); i++)
	{
		delete _layers[i];
	}
	
	_layers.clear();

	_dynamic_tensors.clear();	
	_static_tensors.clear();

	return true;
}