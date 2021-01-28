#pragma OPENCL EXTENSION cl_khr_fp16 : enable


#ifdef HALF_FP
#define DATA_TYPE4 half4
#define WRITE_IMAGE write_imageh
#define READ_IMAGE read_imageh
#else
#define DATA_TYPE4 float4
#define WRITE_IMAGE write_imagef
#define READ_IMAGE read_imagef
#endif

__kernel void extend_channel(__read_only image2d_t srcImg, __write_only image2d_t dstImg, int width, int height)
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
		CLK_ADDRESS_CLAMP_TO_EDGE |
		CLK_FILTER_NEAREST;
	int x = get_global_id(0);
	int y = get_global_id(1);

	float4 outColor[4];

	if (x * 4 < width && y < height)
	{
		float4 inColor = read_imagef(srcImg, sampler, (int2)(x, y));

		outColor[0] = (float4)(inColor.x, inColor.x, inColor.x, inColor.x);
		outColor[1] = (float4)(inColor.y, inColor.y, inColor.y, inColor.y);
		outColor[2] = (float4)(inColor.z, inColor.z, inColor.z, inColor.z);
		outColor[3] = (float4)(inColor.w, inColor.w, inColor.w, inColor.w);

		write_imagef(dstImg, (int2)(4 * x + 0, y), outColor[0]);
		write_imagef(dstImg, (int2)(4 * x + 1, y), outColor[1]);
		write_imagef(dstImg, (int2)(4 * x + 2, y), outColor[2]);
		write_imagef(dstImg, (int2)(4 * x + 3, y), outColor[3]);
	}

}

__kernel void copy_channel(__read_only image2d_t srcImg, __write_only image2d_t dstImg, int width, int height)
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
		CLK_ADDRESS_CLAMP_TO_EDGE |
		CLK_FILTER_NEAREST;

	int x = get_global_id(0);
	int y = get_global_id(1);

	if (x * 4 < width && y < height)
	{
		float4 inColor = read_imagef(srcImg, sampler, (int2)(x, y));

		write_imagef(dstImg, (int2)(x, y), inColor);

	}
}

__kernel void jnd_conv1_3x3_s1x1_n4_c1(__read_only image2d_t srcImg, __write_only image2d_t dstImg, __read_only image2d_t weightImg, int width, int height)
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
		CLK_ADDRESS_CLAMP |
		CLK_FILTER_NEAREST;

	int x = get_global_id(0);
	int y = get_global_id(1);


	DATA_TYPE4  output[4];

	if (x * 4 < width && y < height)
	{
		output[0] = READ_IMAGE(weightImg, sampler, (int2)(0, 9)); // bias
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

			weight[0] = READ_IMAGE(weightImg, sampler, (int2)(0, i * 3 + 0));
			weight[1] = READ_IMAGE(weightImg, sampler, (int2)(0, i * 3 + 1));
			weight[2] = READ_IMAGE(weightImg, sampler, (int2)(0, i * 3 + 2));

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

		output[0] = max(output[0], 0.0f);
		output[1] = max(output[1], 0.0f);
		output[2] = max(output[2], 0.0f);
		output[3] = max(output[3], 0.0f);

		WRITE_IMAGE(dstImg, (int2)(x * 4 + 0, y), output[0]);
		WRITE_IMAGE(dstImg, (int2)(x * 4 + 1, y), output[1]);
		WRITE_IMAGE(dstImg, (int2)(x * 4 + 2, y), output[2]);
		WRITE_IMAGE(dstImg, (int2)(x * 4 + 3, y), output[3]);
	}
}


__kernel void jnd_conv2_3x3_s1x1_n4_c4(__read_only image2d_t srcImg, __write_only image2d_t dstImg, __read_only image2d_t weightImg, int width, int height)
{

	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
		CLK_ADDRESS_CLAMP |
		CLK_FILTER_NEAREST;

	int x = get_global_id(0) * 4;
	int y = get_global_id(1);

	DATA_TYPE4  output[4];

	if (x < width && y < height)
	{

		output[0] = READ_IMAGE(weightImg, sampler, (int2)(0, 36)); // bias
		output[1] = output[0];
		output[2] = output[0];
		output[3] = output[0];

		for (int i = 0; i < 3; i++)
		{
			int pos_y = y + i - 1;

			for (int j = 0; j < 3; j++)
			{
				int pos_x = x + j - 1;

				DATA_TYPE4  input[4];
				DATA_TYPE4  weight[4];

				weight[0] = READ_IMAGE(weightImg, sampler, (int2)(0, (i * 3 + j) * 4 + 0));
				weight[1] = READ_IMAGE(weightImg, sampler, (int2)(0, (i * 3 + j) * 4 + 1));
				weight[2] = READ_IMAGE(weightImg, sampler, (int2)(0, (i * 3 + j) * 4 + 2));
				weight[3] = READ_IMAGE(weightImg, sampler, (int2)(0, (i * 3 + j) * 4 + 3));

				input[0] = READ_IMAGE(srcImg, sampler, (int2)(pos_x + 0, pos_y));
				input[1] = READ_IMAGE(srcImg, sampler, (int2)(pos_x + 1, pos_y));
				input[2] = READ_IMAGE(srcImg, sampler, (int2)(pos_x + 2, pos_y));
				input[3] = READ_IMAGE(srcImg, sampler, (int2)(pos_x + 3, pos_y));

				output[0] = mad(weight[0], input[0].x, output[0]);
				output[0] = mad(weight[1], input[0].y, output[0]);
				output[0] = mad(weight[2], input[0].z, output[0]);
				output[0] = mad(weight[3], input[0].w, output[0]);

				output[1] = mad(weight[0], input[1].x, output[1]);
				output[1] = mad(weight[1], input[1].y, output[1]);
				output[1] = mad(weight[2], input[1].z, output[1]);
				output[1] = mad(weight[3], input[1].w, output[1]);

				output[2] = mad(weight[0], input[2].x, output[2]);
				output[2] = mad(weight[1], input[2].y, output[2]);
				output[2] = mad(weight[2], input[2].z, output[2]);
				output[2] = mad(weight[3], input[2].w, output[2]);

				output[3] = mad(weight[0], input[3].x, output[3]);
				output[3] = mad(weight[1], input[3].y, output[3]);
				output[3] = mad(weight[2], input[3].z, output[3]);
				output[3] = mad(weight[3], input[3].w, output[3]);
			}
		}

		output[0] = max(output[0], 0.0f);
		output[1] = max(output[1], 0.0f);
		output[2] = max(output[2], 0.0f);
		output[3] = max(output[3], 0.0f);

		WRITE_IMAGE(dstImg, (int2)(x + 0, y), output[0]);
		WRITE_IMAGE(dstImg, (int2)(x + 1, y), output[1]);
		WRITE_IMAGE(dstImg, (int2)(x + 2, y), output[2]);
		WRITE_IMAGE(dstImg, (int2)(x + 3, y), output[3]);
	}
}

__kernel void jnd_conv3_3x3_s1x1_n1_c4(__read_only image2d_t srcImg, __write_only image2d_t dstImg, __read_only image2d_t weightImg, int width, int height)
{

	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
		CLK_ADDRESS_CLAMP |
		CLK_FILTER_NEAREST;

	int x = get_global_id(0) * 4;
	int y = get_global_id(1);

	DATA_TYPE4  output;

	if (x < width && y < height)
	{
		output = READ_IMAGE(weightImg, sampler, (int2)(0, 9)); // bias

		for (int i = 0; i < 3; i++)
		{
			int pos_y = y + i - 1;

			for (int j = 0; j < 3; j++)
			{
				int pos_x = x + j - 1;

				DATA_TYPE4  input[4];
				DATA_TYPE4  weight;

				weight = READ_IMAGE(weightImg, sampler, (int2)(0, (i * 3 + j)));

				input[0] = READ_IMAGE(srcImg, sampler, (int2)(pos_x + 0, pos_y));
				input[1] = READ_IMAGE(srcImg, sampler, (int2)(pos_x + 1, pos_y));
				input[2] = READ_IMAGE(srcImg, sampler, (int2)(pos_x + 2, pos_y));
				input[3] = READ_IMAGE(srcImg, sampler, (int2)(pos_x + 3, pos_y));

				output.x += dot(weight, input[0]);
				output.y += dot(weight, input[1]);
				output.z += dot(weight, input[2]);
				output.w += dot(weight, input[3]);
			}
		}

		WRITE_IMAGE(dstImg, (int2)(x / 4, y), output);
	}
}

__kernel void jnd_conv3_3x3_s1x1_n1_c4_x(__read_only image2d_t srcImg, __write_only image2d_t dstImg, __read_only image2d_t weightImg, int width, int height)
{

	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
		CLK_ADDRESS_CLAMP |
		CLK_FILTER_NEAREST;

	int x = get_global_id(0) * 4;
	int y = get_global_id(1);

	DATA_TYPE4  output[4];
	DATA_TYPE4  weight;

	if (x < width && y < height)
	{
		output[0] = (DATA_TYPE4)(0.0f, 0.0f, 0.0f, 0.0f);
		output[1] = output[0];
		output[2] = output[1];
		output[3] = output[2];

		for (int i = 0; i < 3; i++)
		{
			int pos_y = y + i - 1;

			DATA_TYPE4  input[6];

			int pos_x = x - 1;

			input[0] = READ_IMAGE(srcImg, sampler, (int2)(pos_x + 0, pos_y));
			input[1] = READ_IMAGE(srcImg, sampler, (int2)(pos_x + 1, pos_y));
			input[2] = READ_IMAGE(srcImg, sampler, (int2)(pos_x + 2, pos_y));
			input[3] = READ_IMAGE(srcImg, sampler, (int2)(pos_x + 3, pos_y));
			input[4] = READ_IMAGE(srcImg, sampler, (int2)(pos_x + 4, pos_y));
			input[5] = READ_IMAGE(srcImg, sampler, (int2)(pos_x + 5, pos_y));

			weight = READ_IMAGE(weightImg, sampler, (int2)(0, (i * 3 + 0)));
			output[0] = mad(weight, input[0], output[0]);
			output[1] = mad(weight, input[1], output[1]);
			output[2] = mad(weight, input[2], output[2]);
			output[3] = mad(weight, input[3], output[3]);

			weight = READ_IMAGE(weightImg, sampler, (int2)(0, (i * 3 + 1)));
			output[0] = mad(weight, input[1], output[0]);
			output[1] = mad(weight, input[2], output[1]);
			output[2] = mad(weight, input[3], output[2]);
			output[3] = mad(weight, input[4], output[3]);

			weight = READ_IMAGE(weightImg, sampler, (int2)(0, (i * 3 + 2)));
			output[0] = mad(weight, input[2], output[0]);
			output[1] = mad(weight, input[3], output[1]);
			output[2] = mad(weight, input[4], output[2]);
			output[3] = mad(weight, input[5], output[3]);

		}


		DATA_TYPE4 out = READ_IMAGE(weightImg, sampler, (int2)(0, 9)); // bias
		weight = (DATA_TYPE4)(1.0f, 1.0f, 1.0f, 1.0f);

		out.x += dot(weight, output[0]);
		out.y += dot(weight, output[1]);
		out.z += dot(weight, output[2]);
		out.w += dot(weight, output[3]);

		WRITE_IMAGE(dstImg, (int2)(x / 4, y), out);
	}
}