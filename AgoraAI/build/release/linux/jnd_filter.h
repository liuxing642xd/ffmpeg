#pragma once

#include <string>
#include <vector>
using namespace std;


typedef struct _JND_NET_PARAS_
{
	float *conv1_weight;
	float *conv1_bias;
	float *conv2_weight;
	float *conv2_bias;
	float *conv3_weight;
	float *conv3_bias;
}JND_NET_PARAS;

class JndFilter
{
public:
	JndFilter();
	virtual ~JndFilter();

	bool init(JND_NET_PARAS net_paras, int blk_sz_x = 64, int blk_sz_y = 32, int n_threads = 2);
	
	virtual bool release();
	virtual bool process(unsigned char *in, unsigned char *out, int w, int h, int stride);

	// not thread safe
	virtual bool update_net_paras(JND_NET_PARAS net_paras);


	string get_error_str() { return _error_str; }

protected:
	string _error_str;
	bool   _b_init;

private:
	float *_p_temp_buf;
	int    _n_threads;
	int    _blk_sz_x;
	int    _blk_sz_y;

	void   *_funcs[4];
};