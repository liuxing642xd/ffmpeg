

#define STRINGIFY(...) #__VA_ARGS__

const char* jnd_kernel_preprocessor_string = STRINGIFY(

	\r\n #ifdef HALF_FP \r\n
	\r\n #pragma OPENCL EXTENSION cl_khr_fp16 : enable \r\n
	\r\n #define DATA_TYPE  half \r\n
	\r\n #define DATA_TYPE4 half4 \r\n
	\r\n #define DATA_TYPE8 half8 \r\n
	\r\n #define DATA_TYPE16 half16 \r\n
	\r\n #define WRITE_IMAGE write_imageh \r\n
	\r\n #define READ_IMAGE read_imageh \r\n
	\r\n #else \r\n
	\r\n #define DATA_TYPE  float \r\n
	\r\n #define DATA_TYPE4 float4 \r\n
	\r\n #define DATA_TYPE8 float8 \r\n
	\r\n #define DATA_TYPE16 float16 \r\n
	\r\n #define WRITE_IMAGE write_imagef \r\n
	\r\n #define READ_IMAGE read_imagef \r\n
	\r\n #define VLDN vldn \r\n
	\r\n #endif \r\n

	\r\n #ifdef QUALCOMM \r\n
	\r\n #define __ATTRIBUTE_MAX_CONSANT_SIZE_(size) __attribute__((max_constant_size(size))) \r\n
	\r\n #else \r\n
	\r\n #define __ATTRIBUTE_MAX_CONSANT_SIZE_(size)  \r\n
	\r\n #endif \r\n

	\r\n
);

const char* jnd_kernel_cl_string = STRINGIFY(


__kernel void jnd_conv1_3x3_s1x1_n4_c1(__read_only image2d_t srcImg, __write_only image2d_t dstImg, __constant DATA_TYPE *weight_bias __ATTRIBUTE_MAX_CONSANT_SIZE_(40*sizeof(DATA_TYPE)), int width, int height)
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
		CLK_ADDRESS_CLAMP_TO_EDGE |
		CLK_FILTER_NEAREST;

	int x = get_global_id(0);
	int y = get_global_id(1);

	DATA_TYPE4  output[4];

	if (x * 4 < width && y < height)
	{
		output[0] = vload4(9, weight_bias); // bias
		output[1] = output[0];
		output[2] = output[0];
		output[3] = output[0];

		for (int i = 0; i < 3; i++)
		{
			int pos_y = y + i - 1;

			DATA_TYPE4  weight[3];
			DATA_TYPE4  input[3];

			input[0] = READ_IMAGE(srcImg, sampler, (int2)(x - 1, pos_y));
			input[1] = READ_IMAGE(srcImg, sampler, (int2)(x + 0, pos_y));
			input[2] = READ_IMAGE(srcImg, sampler, (int2)(x + 1, pos_y));

			weight[0] = vload4(i * 3 + 0, weight_bias);
			weight[1] = vload4(i * 3 + 1, weight_bias);
			weight[2] = vload4(i * 3 + 2, weight_bias);

			output[0] = mad(weight[0], input[0].w, output[0]);
			output[1] = mad(weight[0], input[1].x, output[1]);
			output[2] = mad(weight[0], input[1].y, output[2]);
			output[3] = mad(weight[0], input[1].z, output[3]);

			output[0] = mad(weight[1], input[1].x, output[0]);
			output[1] = mad(weight[1], input[1].y, output[1]);
			output[2] = mad(weight[1], input[1].z, output[2]);
			output[3] = mad(weight[1], input[1].w, output[3]);

			output[0] = mad(weight[2], input[1].y, output[0]);
			output[1] = mad(weight[2], input[1].z, output[1]);
			output[2] = mad(weight[2], input[1].w, output[2]);
			output[3] = mad(weight[2], input[2].x, output[3]);
		}

		output[0] = fmax(output[0], (DATA_TYPE4)(0.0f));
		output[1] = fmax(output[1], (DATA_TYPE4)(0.0f));
		output[2] = fmax(output[2], (DATA_TYPE4)(0.0f));
		output[3] = fmax(output[3], (DATA_TYPE4)(0.0f));

		WRITE_IMAGE(dstImg, (int2)(x * 4 + 0, y), output[0]);
		WRITE_IMAGE(dstImg, (int2)(x * 4 + 1, y), output[1]);
		WRITE_IMAGE(dstImg, (int2)(x * 4 + 2, y), output[2]);
		WRITE_IMAGE(dstImg, (int2)(x * 4 + 3, y), output[3]);	
	}
}



\r\n

\r\n #define CONV2_CORE(w0, w1, w2, w3, in0, in1, in2, in3, out0, out1, out2, out3) \\\n
out0 = mad(w0, in0.x, out0); \\\n
out0 = mad(w1, in0.y, out0); \\\n
out0 = mad(w2, in0.z, out0); \\\n
out0 = mad(w3, in0.w, out0); \\\n
out1 = mad(w0, in1.x, out1); \\\n
out1 = mad(w1, in1.y, out1); \\\n
out1 = mad(w2, in1.z, out1); \\\n
out1 = mad(w3, in1.w, out1); \\\n
out2 = mad(w0, in2.x, out2); \\\n
out2 = mad(w1, in2.y, out2); \\\n
out2 = mad(w2, in2.z, out2); \\\n
out2 = mad(w3, in2.w, out2); \\\n
out3 = mad(w0, in3.x, out3); \\\n
out3 = mad(w1, in3.y, out3); \\\n
out3 = mad(w2, in3.z, out3); \\\n
out3 = mad(w3, in3.w, out3); \\\n

\r\n


//__kernel  void jnd_conv2_3x3_s1x1_n4_c4(__read_only image2d_t srcImg, __write_only image2d_t dstImg, __constant DATA_TYPE *weight_bias __ATTRIBUTE_MAX_CONSANT_SIZE_(592), int width, int height)
//{
//	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
//		CLK_ADDRESS_CLAMP_TO_EDGE |
//		CLK_FILTER_NEAREST;
//
//	int x = get_global_id(0) * 4;
//	int y = get_global_id(1);
//
//	DATA_TYPE4  out[4];
//
//	if (x < width && y < height)
//	{
//		out[0] = vload4(36, weight_bias); // bias
//		out[1] = out[0];
//		out[2] = out[0];
//		out[3] = out[0];
//
//		int offset = 0;
//		for (int i = 0; i < 3; i++)
//		{
//			DATA_TYPE4  w[4];
//			DATA_TYPE4  in[6];
//
//			int pos_y = y + i - 1;
//			
//			in[0] = READ_IMAGE(srcImg, sampler, (int2)(x - 1, pos_y));
//			in[1] = READ_IMAGE(srcImg, sampler, (int2)(x + 0, pos_y));
//			in[2] = READ_IMAGE(srcImg, sampler, (int2)(x + 1, pos_y));
//			in[3] = READ_IMAGE(srcImg, sampler, (int2)(x + 2, pos_y));
//			in[4] = READ_IMAGE(srcImg, sampler, (int2)(x + 3, pos_y));
//			in[5] = READ_IMAGE(srcImg, sampler, (int2)(x + 4, pos_y));
//
//			w[0] = vload4(offset + 0, weight_bias);
//			w[1] = vload4(offset + 1, weight_bias);
//			w[2] = vload4(offset + 2, weight_bias);
//			w[3] = vload4(offset + 3, weight_bias);
//			CONV2_CORE(w[0], w[1], w[2], w[3], in[0], in[1], in[2], in[3], out[0], out[1], out[2], out[3]);
//			
//			w[0] = vload4(offset + 4, weight_bias);
//			w[1] = vload4(offset + 5, weight_bias);
//			w[2] = vload4(offset + 6, weight_bias);
//			w[3] = vload4(offset + 7, weight_bias);
//			CONV2_CORE(w[0], w[1], w[2], w[3], in[1], in[2], in[3], in[4], out[0], out[1], out[2], out[3]);
//			
//			w[0] = vload4(offset + 8, weight_bias);
//			w[1] = vload4(offset + 9, weight_bias);
//			w[2] = vload4(offset + 10, weight_bias);
//			w[3] = vload4(offset + 11, weight_bias);
//			CONV2_CORE(w[0], w[1], w[2], w[3], in[2], in[3], in[4], in[5], out[0], out[1], out[2], out[3]);
//
//			offset += 12;
//		}
//
//		out[0] = fmax(out[0], (DATA_TYPE4)(0.0f));
//		out[1] = fmax(out[1], (DATA_TYPE4)(0.0f));
//		out[2] = fmax(out[2], (DATA_TYPE4)(0.0f));
//		out[3] = fmax(out[3], (DATA_TYPE4)(0.0f));
//
//		WRITE_IMAGE(dstImg, (int2)(x + 0, y), out[0]);
//		WRITE_IMAGE(dstImg, (int2)(x + 1, y), out[1]);
//		WRITE_IMAGE(dstImg, (int2)(x + 2, y), out[2]);
//		WRITE_IMAGE(dstImg, (int2)(x + 3, y), out[3]);
//	}
//}

__kernel  void jnd_conv2_3x3_s1x1_n4_c4(__read_only image2d_t srcImg, __write_only image2d_t dstImg, __constant DATA_TYPE *weight_bias __ATTRIBUTE_MAX_CONSANT_SIZE_(148*sizeof(DATA_TYPE)), int width, int height)
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
		CLK_ADDRESS_CLAMP_TO_EDGE |
		CLK_FILTER_NEAREST;

	int x = get_global_id(0) * 4;
	int y = get_global_id(1);

	DATA_TYPE4  out[4];

	if (x < width && y < height)
	{
		out[0] = vload4(36, weight_bias); // bias
		out[1] = out[0];
		out[2] = out[0];
		out[3] = out[0];

		int offset = 0;
		for (int i = 0; i < 3; i++)
		{
			DATA_TYPE8  w[2];
			DATA_TYPE4  in[6];

			int pos_y = y + i - 1;

			in[0] = READ_IMAGE(srcImg, sampler, (int2)(x - 1, pos_y));
			in[1] = READ_IMAGE(srcImg, sampler, (int2)(x + 0, pos_y));
			in[2] = READ_IMAGE(srcImg, sampler, (int2)(x + 1, pos_y));
			in[3] = READ_IMAGE(srcImg, sampler, (int2)(x + 2, pos_y));
			in[4] = READ_IMAGE(srcImg, sampler, (int2)(x + 3, pos_y));
			in[5] = READ_IMAGE(srcImg, sampler, (int2)(x + 4, pos_y));

			w[0] = vload8(offset + 0, weight_bias);	
			w[1] = vload8(offset + 1, weight_bias);
			CONV2_CORE(w[0].s0123, w[0].s4567, w[1].s0123, w[1].s4567, in[0], in[1], in[2], in[3], out[0], out[1], out[2], out[3]);

			w[0] = vload8(offset + 2, weight_bias);
			w[1] = vload8(offset + 3, weight_bias);
			CONV2_CORE(w[0].s0123, w[0].s4567, w[1].s0123, w[1].s4567, in[1], in[2], in[3], in[4], out[0], out[1], out[2], out[3]);

			w[0] = vload8(offset + 4, weight_bias);
			w[1] = vload8(offset + 5, weight_bias);
			CONV2_CORE(w[0].s0123, w[0].s4567, w[1].s0123, w[1].s4567,  in[2], in[3], in[4], in[5], out[0], out[1], out[2], out[3]);

			offset += 6;
		}

		out[0] = fmax(out[0], (DATA_TYPE4)(0.0f));
		out[1] = fmax(out[1], (DATA_TYPE4)(0.0f));
		out[2] = fmax(out[2], (DATA_TYPE4)(0.0f));
		out[3] = fmax(out[3], (DATA_TYPE4)(0.0f));

		WRITE_IMAGE(dstImg, (int2)(x + 0, y), out[0]);
		WRITE_IMAGE(dstImg, (int2)(x + 1, y), out[1]);
		WRITE_IMAGE(dstImg, (int2)(x + 2, y), out[2]);
		WRITE_IMAGE(dstImg, (int2)(x + 3, y), out[3]);
	}
}


                                                                                                  

\r\n #ifdef QUALCOMM \r\n

 //kernel for qualcomm Adreno 512, which avoid L1 cache thrashing
__kernel void jnd_conv3_3x3_s1x1_n1_c4(__read_only image2d_t srcImg, __write_only image2d_t dstImg, __constant DATA_TYPE *weight_bias __ATTRIBUTE_MAX_CONSANT_SIZE_(40 * sizeof(DATA_TYPE)), int width, int height)
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
		CLK_ADDRESS_CLAMP_TO_EDGE |
		CLK_FILTER_NEAREST;

	int x = get_global_id(0) * 4;
	int y = get_global_id(1);

	DATA_TYPE4  output;

	if (x < width && y < height)
	{
		output = vload4(9, weight_bias); //bias
		for (int i = 0; i < 3; i++)
		{
			int pos_y = y + i - 1;

			DATA_TYPE4  input[6];
			DATA_TYPE4  w[3];

			input[0] = READ_IMAGE(srcImg, sampler, (int2)(x - 1, pos_y));
			input[1] = READ_IMAGE(srcImg, sampler, (int2)(x + 0, pos_y));
			input[2] = READ_IMAGE(srcImg, sampler, (int2)(x + 1, pos_y));
			input[3] = READ_IMAGE(srcImg, sampler, (int2)(x + 2, pos_y));
			input[4] = READ_IMAGE(srcImg, sampler, (int2)(x + 3, pos_y));
			input[5] = READ_IMAGE(srcImg, sampler, (int2)(x + 4, pos_y));

			w[0] = vload4(i * 3 + 0, weight_bias);
			w[1] = vload4(i * 3 + 1, weight_bias);
			w[2] = vload4(i * 3 + 2, weight_bias);

			output += (DATA_TYPE4)(dot(w[0], input[0]), dot(w[0], input[1]), dot(w[0], input[2]), dot(w[0], input[3]));
			output += (DATA_TYPE4)(dot(w[1], input[1]), dot(w[1], input[2]), dot(w[1], input[3]), dot(w[1], input[4]));
			output += (DATA_TYPE4)(dot(w[2], input[2]), dot(w[2], input[3]), dot(w[2], input[4]), dot(w[2], input[5]));		
		}

		WRITE_IMAGE(dstImg, (int2)(x / 4, y), output);
	}
}

\r\n #else \r\n

__kernel void jnd_conv3_3x3_s1x1_n1_c4(__read_only image2d_t srcImg, __write_only image2d_t dstImg, __constant DATA_TYPE *weight_bias __ATTRIBUTE_MAX_CONSANT_SIZE_(40*sizeof(DATA_TYPE)), int width, int height)
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
		CLK_ADDRESS_CLAMP_TO_EDGE |
		CLK_FILTER_NEAREST;

	int x = get_global_id(0) * 4;
	int y = get_global_id(1);

	DATA_TYPE4  output[4];

	if (x < width && y < height)
	{
		output[0] = (DATA_TYPE4)(0.0f);
		output[1] = output[0];
		output[2] = output[0];
		output[3] = output[0];
		
		for (int i = 0; i < 3; i++)
		{
			int pos_y = y + i - 1;

			DATA_TYPE4  input[6];
			DATA_TYPE4  w[3];

			input[0] = READ_IMAGE(srcImg, sampler, (int2)(x - 1, pos_y));
			input[1] = READ_IMAGE(srcImg, sampler, (int2)(x + 0, pos_y));			
			input[2] = READ_IMAGE(srcImg, sampler, (int2)(x + 1, pos_y));
			input[3] = READ_IMAGE(srcImg, sampler, (int2)(x + 2, pos_y));			
			input[4] = READ_IMAGE(srcImg, sampler, (int2)(x + 3, pos_y));
			input[5] = READ_IMAGE(srcImg, sampler, (int2)(x + 4, pos_y));

			w[0] = vload4(i * 3 + 0, weight_bias);
			w[1] = vload4(i * 3 + 1, weight_bias);
			w[2] = vload4(i * 3 + 2, weight_bias);

			output[0] = mad(w[0], input[0], output[0]);
			output[1] = mad(w[0], input[1], output[1]);
			output[2] = mad(w[0], input[2], output[2]);
			output[3] = mad(w[0], input[3], output[3]);

			output[0] = mad(w[1], input[1], output[0]);
			output[1] = mad(w[1], input[2], output[1]);
			output[2] = mad(w[1], input[3], output[2]);
			output[3] = mad(w[1], input[4], output[3]);
							
			output[0] = mad(w[2], input[2], output[0]);
			output[1] = mad(w[2], input[3], output[1]);
			output[2] = mad(w[2], input[4], output[2]);
			output[3] = mad(w[2], input[5], output[3]);
		}

		output[0].x = dot((DATA_TYPE4)(1.0f), output[0]);
		output[0].y = dot((DATA_TYPE4)(1.0f), output[1]);
		output[0].z = dot((DATA_TYPE4)(1.0f), output[2]);
		output[0].w = dot((DATA_TYPE4)(1.0f), output[3]);

		output[0] += vload4(9, weight_bias); //bias

		WRITE_IMAGE(dstImg, (int2)(x / 4, y), output[0]);
	}
}

\r\n #endif  \r\n

);
