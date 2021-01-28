#pragma once

#include "core/common.h"
#include "core/utils.h"

template<int BLK_SZ_X, int BLK_SZ_Y> void blk_pad_to_float(unsigned char *in, float *blk, int img_w, int img_h, int stride, int blk_x, int blk_y, int num_blk_x, int num_blk_y, int pad)
{
	for (int y = -pad; y < BLK_SZ_Y + pad; y++)
	{
		int i_y = y + blk_y*BLK_SZ_Y;
		if (i_y < 0) i_y = 0;
		if (i_y >= img_h) i_y = img_h - 1;

		float *pblk = blk + (y + pad)*(BLK_SZ_X + 2 * pad);
		unsigned char *pin = in + i_y*stride;

		if (blk_x > 0 && blk_x < num_blk_x - 1)
		{
			int offset = blk_x*BLK_SZ_X - pad;

			for (int x = 0; x < BLK_SZ_X + pad * 2; x++)
			{
				pblk[x] = pin[x + offset] * (1.0f / 255.0f);
			}
		}
		else
		{
			for (int x = -pad; x < BLK_SZ_X + pad; x++)
			{
				int i_x = x + blk_x*BLK_SZ_X;
				if (i_x < 0) i_x = 0;
				if (i_x >= img_w) i_x = img_w - 1;

				pblk[pad + x] = pin[i_x] * (1.0f / 255.0f);
			}
		}
	}
}

template<int BLK_SZ_X, int BLK_SZ_Y> void blk_conv1_hwcn(float *in_data, float *out_data, float *weight, float *bias)
{
	for (int y = 0; y < BLK_SZ_Y + 4; y++)
	{
		for (int x = 0; x < BLK_SZ_X + 4; x++)
		{
			float *pOut = out_data + (y*(BLK_SZ_X + 4) + x) * 4;

			float sum0 = bias[0];
			float sum1 = bias[1];
			float sum2 = bias[2];
			float sum3 = bias[3];

			for (int kh = 0; kh < 3; kh++)
			{
				float *pW = weight + (kh * 3) * 4;
				float *pIn = in_data + ((y + kh)*(BLK_SZ_X + 6) + x);

				for (int kw = 0; kw < 3; kw++)
				{
					float pix = pIn[0];

					sum0 += pix * pW[0];
					sum1 += pix * pW[1];
					sum2 += pix * pW[2];
					sum3 += pix * pW[3];

					pW += 4;
					pIn++;
				}
			}

			if (sum0 < 0.0f) sum0 = 0.0f;
			if (sum1 < 0.0f) sum1 = 0.0f;
			if (sum2 < 0.0f) sum2 = 0.0f;
			if (sum3 < 0.0f) sum3 = 0.0f;

			pOut[0] = sum0;
			pOut[1] = sum1;
			pOut[2] = sum2;
			pOut[3] = sum3;
		}
	}
}

template<int BLK_SZ_X, int BLK_SZ_Y> void blk_conv2_hwcn(float *in_data, float *out_data, float *weight, float *bias)
{
	for (int y = 0; y < BLK_SZ_Y + 2; y++)
	{
		for (int x = 0; x < BLK_SZ_X + 2; x++)
		{
			float *pOut = out_data + (y*(BLK_SZ_X + 2) + x) * 4;
			float *pIn = in_data + (y*(BLK_SZ_X + 4) + x) * 4;

			float sum0 = bias[0];
			float sum1 = bias[1];
			float sum2 = bias[2];
			float sum3 = bias[3];

			for (int kh = 0; kh < 3; kh++)
			{
				float *pW = weight + (kh * 3) * 16;
				float *pInWin = pIn + kh*(BLK_SZ_X + 4) * 4;

				for (int kw = 0; kw < 3; kw++)
				{

					for (int c = 0; c < 4; c++)
					{
						float pix = pInWin[c];
						sum0 += pix * pW[4 * c + 0];
						sum1 += pix * pW[4 * c + 1];
						sum2 += pix * pW[4 * c + 2];
						sum3 += pix * pW[4 * c + 3];
					}

					pW += 16;
					pInWin += 4;
				}
			}

			if (sum0 < 0.0f) sum0 = 0.0f;
			if (sum1 < 0.0f) sum1 = 0.0f;
			if (sum2 < 0.0f) sum2 = 0.0f;
			if (sum3 < 0.0f) sum3 = 0.0f;

			pOut[0] = sum0;
			pOut[1] = sum1;
			pOut[2] = sum2;
			pOut[3] = sum3;
		}
	}
}

template<int BLK_SZ_X, int BLK_SZ_Y> void blk_conv3_hwcn(float *in_data, unsigned char *out_data, float *weight, float *bias, int img_w, int img_h, int stride, int blk_x, int blk_y)
{
	//TimeProfiler t(__func__);

	for (int y = 0; y < BLK_SZ_Y; y++)
	{
		for (int x = 0; x < BLK_SZ_X; x += 4)
		{
			unsigned char *pOut = out_data + (blk_y*BLK_SZ_Y + y)*stride + blk_x*BLK_SZ_X + x;

			float sum0 = bias[0];
			float sum1 = bias[0];
			float sum2 = bias[0];
			float sum3 = bias[0];

			for (int kh = 0; kh < 3; kh++)
			{
				float *pW = weight + (kh * 3) * 4;
				float *pIn = in_data + ((y + kh)*(BLK_SZ_X + 2) + x) * 4;

				for (int kw = 0; kw < 3; kw++)
				{
					sum0 += pW[0] * pIn[0] + pW[1] * pIn[1] + pW[2] * pIn[2] + pW[3] * pIn[3];
					sum1 += pW[0] * pIn[4] + pW[1] * pIn[5] + pW[2] * pIn[6] + pW[3] * pIn[7];
					sum2 += pW[0] * pIn[8] + pW[1] * pIn[9] + pW[2] * pIn[10] + pW[3] * pIn[11];
					sum3 += pW[0] * pIn[12] + pW[1] * pIn[13] + pW[2] * pIn[14] + pW[3] * pIn[15];

					pW += 4;
					pIn += 4;
				}
			}

			pOut[0] = (unsigned char)(sum0*255.0f);
			pOut[1] = (unsigned char)(sum1*255.0f);
			pOut[2] = (unsigned char)(sum2*255.0f);
			pOut[3] = (unsigned char)(sum3*255.0f);

		}
	}

}


#ifdef USE_AVX

template<int BLK_SZ_X, int BLK_SZ_Y> void blk_pad_to_float_avx(unsigned char *in, float *blk, int img_w, int img_h, int stride, int blk_x, int blk_y, int num_blk_x, int num_blk_y, int pad)
{
	for (int y = -pad; y < BLK_SZ_Y + pad; y++)
	{
		int i_y = y + blk_y*BLK_SZ_Y;
		if (i_y < 0) i_y = 0;
		if (i_y >= img_h) i_y = img_h - 1;

		float *pblk = blk + (y + pad)*(BLK_SZ_X + 2 * pad);
		unsigned char *pin = in + i_y*stride;

		if (blk_x > 0 && blk_x < num_blk_x - 1)
		{
			int offset = blk_x*BLK_SZ_X - pad;

			__m128i vzero = _mm_setzero_si128();
			__m128 vrecp255 = _mm_set1_ps(1.0f / 255.0f);
			__m128i vpix, va, vb, vc;
			__m128 vfb, vfc;

			int x = 0;
			for (; x < (BLK_SZ_X + pad * 2) / 16 * 16; x += 16)
			{
				vpix = _mm_loadu_si128((__m128i *)(pin + x + offset));

				va = _mm_unpacklo_epi8(vpix, vzero);

				vb = _mm_unpacklo_epi16(va, vzero);
				vc = _mm_unpackhi_epi16(va, vzero);
				vfb = _mm_cvtepi32_ps(vb);
				vfc = _mm_cvtepi32_ps(vc);
				vfb = _mm_mul_ps(vfb, vrecp255);
				vfc = _mm_mul_ps(vfc, vrecp255);

				_mm_storeu_ps(pblk + x + 0, vfb);
				_mm_storeu_ps(pblk + x + 4, vfc);

				va = _mm_unpackhi_epi8(vpix, vzero);
				vb = _mm_unpacklo_epi16(va, vzero);
				vc = _mm_unpackhi_epi16(va, vzero);
				vfb = _mm_cvtepi32_ps(vb);
				vfc = _mm_cvtepi32_ps(vc);
				vfb = _mm_mul_ps(vfb, vrecp255);
				vfc = _mm_mul_ps(vfc, vrecp255);

				_mm_storeu_ps(pblk + x + 8, vfb);
				_mm_storeu_ps(pblk + x + 12, vfc);
			}
			for (; x < BLK_SZ_X + pad * 2; x++)
			{
				pblk[x] = pin[x + offset] * (1.0f / 255.0f);
			}
		}
		else
		{
			for (int x = -pad; x < BLK_SZ_X + pad; x++)
			{
				int i_x = x + blk_x*BLK_SZ_X;
				if (i_x < 0) i_x = 0;
				if (i_x >= img_w) i_x = img_w - 1;

				pblk[pad + x] = pin[i_x] * (1.0f / 255.0f);
			}
		}
	}

}


template<int BLK_SZ_X, int BLK_SZ_Y> void blk_conv1_hwcn_avx(float *in_data, float *out_data, float *weight, float *bias)
{
	__m256 vpix, vw0_2, vw1_2, vw2_2;
	__m256 puzzle0, puzzle1, puzzle2, puzzle3, puzzle4, puzzle5;
	__m256 vsum01, vsum23, vtemp;

	vtemp = _mm256_broadcast_ps((__m128*)bias);
	__m256 vzero = _mm256_set1_ps(0.0f);

	for (int y = 0; y < BLK_SZ_Y + 4; y++)
	{
		for (int x = 0; x < BLK_SZ_X + 4; x += 4)
		{
			float* pOut = out_data + (y * (BLK_SZ_X + 4) + x) * 4;
			float* pW = weight;
			float* pIn = in_data + (y * (BLK_SZ_X + 6) + x);

			vsum01 = vtemp;
			vsum23 = vtemp;

			for (int kh = 0; kh < 3; kh++)
			{
				vw0_2 = _mm256_broadcast_ps((__m128*)(pW + 4 * 0));
				vw1_2 = _mm256_broadcast_ps((__m128*)(pW + 4 * 1));
				vw2_2 = _mm256_broadcast_ps((__m128*)(pW + 4 * 2));

				puzzle0 = _mm256_broadcast_ss(pIn);
				puzzle1 = _mm256_broadcast_ss(pIn + 1);
				puzzle2 = _mm256_broadcast_ss(pIn + 2);
				puzzle3 = _mm256_broadcast_ss(pIn + 3);
				puzzle4 = _mm256_broadcast_ss(pIn + 4);
				puzzle5 = _mm256_broadcast_ss(pIn + 5);

				vpix = _mm256_blend_ps(puzzle0, puzzle1, 0xF0);
				vsum01 = _mm256_fmadd_ps(vpix, vw0_2, vsum01);
				vpix = _mm256_blend_ps(puzzle1, puzzle2, 0xF0);
				vsum01 = _mm256_fmadd_ps(vpix, vw1_2, vsum01);
				vpix = _mm256_blend_ps(puzzle2, puzzle3, 0xF0);
				vsum23 = _mm256_fmadd_ps(vpix, vw0_2, vsum23);
				vsum01 = _mm256_fmadd_ps(vpix, vw2_2, vsum01);
				vpix = _mm256_blend_ps(puzzle3, puzzle4, 0xF0);
				vsum23 = _mm256_fmadd_ps(vpix, vw1_2, vsum23);
				vpix = _mm256_blend_ps(puzzle4, puzzle5, 0xF0);
				vsum23 = _mm256_fmadd_ps(vpix, vw2_2, vsum23);

				pW += 12;
				pIn += (BLK_SZ_X + 6);
			}
			vsum01 = _mm256_max_ps(vsum01, vzero);
			vsum23 = _mm256_max_ps(vsum23, vzero);
			_mm256_storeu_ps(pOut + 0, vsum01);
			_mm256_storeu_ps(pOut + 8, vsum23);

		}
	}
}


template<int BLK_SZ_X, int BLK_SZ_Y> void blk_conv2_hwcn_avx(float *in_data, float *out_data, float *weight, float *bias)
{	
	__m256 vzero = _mm256_set1_ps(0.0f);
	__m256 v2w, vsum01, vsum23, vsum45;
	__m256 vtemp = _mm256_broadcast_ps((__m128*)bias);
	__m256 vIn0, vIn1, vIn2, vIn3;

	for (int y = 0; y < BLK_SZ_Y + 2; y++)
	{
		for (int x = 0; x < BLK_SZ_X + 2; x += 6)
		{
			int xpos = x;
			int extra = x + 6 - (BLK_SZ_X + 2);
			if (extra > 0)
			{
				xpos -= extra;
			}

			float* pOut = out_data + (y * (BLK_SZ_X + 2) + xpos) * 4;
			float* pIn = in_data + (y * (BLK_SZ_X + 4) + xpos) * 4;

			vsum01 = vtemp;
			vsum23 = vtemp;
			vsum45 = vtemp;

			
			float* pW = weight;

			for (int kh = 0; kh < 3; kh++)
			{
				float* pInWin = pIn + kh * (BLK_SZ_X + 4) * 4;
				for (int kw = 0; kw < 3; kw++) {
					vIn0 = _mm256_broadcast_ss(pInWin + 0);
					vIn1 = _mm256_broadcast_ss(pInWin + 4);
					vIn0 = _mm256_blend_ps(vIn1, vIn0, 0x0F);
					vIn1 = _mm256_broadcast_ss(pInWin + 8);
					vIn2 = _mm256_broadcast_ss(pInWin + 12);
					vIn1 = _mm256_blend_ps(vIn2, vIn1, 0x0F);
					vIn2 = _mm256_broadcast_ss(pInWin + 16);
					vIn3 = _mm256_broadcast_ss(pInWin + 20);
					vIn2 = _mm256_blend_ps(vIn3, vIn2, 0x0F);

					v2w = _mm256_broadcast_ps((__m128*)(pW + 4 * 0));

					vsum01 = _mm256_fmadd_ps(vIn0, v2w, vsum01);
					vsum23 = _mm256_fmadd_ps(vIn1, v2w, vsum23);
					vsum45 = _mm256_fmadd_ps(vIn2, v2w, vsum45);

					vIn0 = _mm256_broadcast_ss((pInWin + 1));
					vIn1 = _mm256_broadcast_ss((pInWin + 5));
					vIn0 = _mm256_blend_ps(vIn1, vIn0, 0x0F);
					vIn1 = _mm256_broadcast_ss((pInWin + 9));
					vIn2 = _mm256_broadcast_ss((pInWin + 13));
					vIn1 = _mm256_blend_ps(vIn2, vIn1, 0x0F);
					vIn2 = _mm256_broadcast_ss((pInWin + 17));
					vIn3 = _mm256_broadcast_ss((pInWin + 21));
					vIn2 = _mm256_blend_ps(vIn3, vIn2, 0x0F);

					v2w = _mm256_broadcast_ps((__m128 *) (pW + 4 * 1));
					vsum01 = _mm256_fmadd_ps(vIn0, v2w, vsum01);
					vsum23 = _mm256_fmadd_ps(vIn1, v2w, vsum23);
					vsum45 = _mm256_fmadd_ps(vIn2, v2w, vsum45);

					vIn0 = _mm256_broadcast_ss((pInWin + 2));
					vIn1 = _mm256_broadcast_ss((pInWin + 6));
					vIn0 = _mm256_blend_ps(vIn1, vIn0, 0x0F);
					vIn1 = _mm256_broadcast_ss((pInWin + 10));
					vIn2 = _mm256_broadcast_ss((pInWin + 14));
					vIn1 = _mm256_blend_ps(vIn2, vIn1, 0x0F);
					vIn2 = _mm256_broadcast_ss((pInWin + 18));
					vIn3 = _mm256_broadcast_ss((pInWin + 22));
					vIn2 = _mm256_blend_ps(vIn3, vIn2, 0x0F);
					v2w = _mm256_broadcast_ps((__m128 *) (pW + 4 * 2));
					vsum01 = _mm256_fmadd_ps(vIn0, v2w, vsum01);
					vsum23 = _mm256_fmadd_ps(vIn1, v2w, vsum23);
					vsum45 = _mm256_fmadd_ps(vIn2, v2w, vsum45);

					vIn0 = _mm256_broadcast_ss((pInWin + 3));
					vIn1 = _mm256_broadcast_ss((pInWin + 7));
					vIn0 = _mm256_blend_ps(vIn1, vIn0, 0x0F);
					vIn1 = _mm256_broadcast_ss((pInWin + 11));
					vIn2 = _mm256_broadcast_ss((pInWin + 15));
					vIn1 = _mm256_blend_ps(vIn2, vIn1, 0x0F);
					vIn2 = _mm256_broadcast_ss((pInWin + 19));
					vIn3 = _mm256_broadcast_ss((pInWin + 23));
					vIn2 = _mm256_blend_ps(vIn3, vIn2, 0x0F);

					v2w = _mm256_broadcast_ps((__m128 *) (pW + 4 * 3));
					vsum01 = _mm256_fmadd_ps(vIn0, v2w, vsum01);
					vsum23 = _mm256_fmadd_ps(vIn1, v2w, vsum23);
					vsum45 = _mm256_fmadd_ps(vIn2, v2w, vsum45);

					pW += 16;
					pInWin += 4;
				}
			}

			vsum01 = _mm256_max_ps(vzero, vsum01); 
			vsum23 = _mm256_max_ps(vzero, vsum23);
			vsum45 = _mm256_max_ps(vzero, vsum45);

			_mm256_storeu_ps(pOut + 0, vsum01);
			_mm256_storeu_ps(pOut + 8, vsum23);
			_mm256_storeu_ps(pOut + 16, vsum45);

		}
	}
}

template<int BLK_SZ_X, int BLK_SZ_Y> void blk_conv3_hwcn_avx(float *in_data, unsigned char *out_data, float *weight, float *bias, int img_w, int img_h, int stride, int blk_x, int blk_y)
{
#if defined(__GNUC__)
	__attribute__((__aligned__((128)))) float temp[4];
#else
	__declspec(align(128)) float temp[4];
#endif

	__m128 vsum0, vsum1, vsum2, vsum3;
	__m128 vw0, vw1, vw2;
	__m128 vpix;
	__m128	v255 = _mm_set1_ps(255.0f);
	__m128  vbias = _mm_set1_ps(bias[0]);

	for (int y = 0; y < BLK_SZ_Y; y++)
	{
		int img_y = blk_y*BLK_SZ_Y + y;
		if (img_y >= img_h) continue;

		for (int x = 0; x < BLK_SZ_X; x += 4)
		{
			int img_x = blk_x*BLK_SZ_X + x;

			unsigned char *pOut = out_data + img_y*stride + img_x;
			float *pInWin = in_data + (y*(BLK_SZ_X + 2) + x) * 4;
			float *pW = weight;

			vsum0 = _mm_set1_ps(0.0f);
			vsum1 = vsum0;
			vsum2 = vsum0;
			vsum3 = vsum0;

			for (int kh = 0; kh < 3; kh++)
			{
				vw0 = _mm_load_ps(pW + 0);
				vw1 = _mm_load_ps(pW + 4);
				vw2 = _mm_load_ps(pW + 8);

				vpix = _mm_load_ps(pInWin + 0);
				vsum0 = _mm_fmadd_ps(vpix, vw0, vsum0);
				vpix = _mm_load_ps(pInWin + 4);
				vsum1 = _mm_fmadd_ps(vpix, vw0, vsum1);
				vsum0 = _mm_fmadd_ps(vpix, vw1, vsum0);
				vpix = _mm_load_ps(pInWin + 8);
				vsum2 = _mm_fmadd_ps(vpix, vw0, vsum2);
				vsum1 = _mm_fmadd_ps(vpix, vw1, vsum1);
				vsum0 = _mm_fmadd_ps(vpix, vw2, vsum0);
				vpix = _mm_load_ps(pInWin + 12);
				vsum3 = _mm_fmadd_ps(vpix, vw0, vsum3);
				vsum2 = _mm_fmadd_ps(vpix, vw1, vsum2);
				vsum1 = _mm_fmadd_ps(vpix, vw2, vsum1);
				vpix = _mm_load_ps(pInWin + 16);
				vsum3 = _mm_fmadd_ps(vpix, vw1, vsum3);
				vsum2 = _mm_fmadd_ps(vpix, vw2, vsum2);
				vpix = _mm_load_ps(pInWin + 20);
				vsum3 = _mm_fmadd_ps(vpix, vw2, vsum3);

				pW += 12;
				pInWin += (BLK_SZ_X + 2) * 4;
			}

			vsum0 = _mm_hadd_ps(vsum0, vsum1);
			vsum2 = _mm_hadd_ps(vsum2, vsum3);
			vsum0 = _mm_hadd_ps(vsum0, vsum2);
			vsum0 = _mm_add_ps(vsum0, vbias);
			vsum0 = _mm_mul_ps(vsum0, v255);

			_mm_store_ps(temp, vsum0);

			if (img_x <= img_w - 4)
			{
				pOut[0] = (unsigned char)temp[0];
				pOut[1] = (unsigned char)temp[1];
				pOut[2] = (unsigned char)temp[2];
				pOut[3] = (unsigned char)temp[3];
			}
		}
	}

}

#endif

#ifdef USE_NEON

template<int BLK_SZ_X, int BLK_SZ_Y> void blk_pad_to_float_neon(unsigned char *in, float *blk, int img_w, int img_h, int stride, int blk_x, int blk_y, int num_blk_x, int num_blk_y, int pad)
{
	for (int y = -pad; y < BLK_SZ_Y + pad; y++)
	{
		int i_y = y + blk_y*BLK_SZ_Y;
		if (i_y < 0) i_y = 0;
		if (i_y >= img_h) i_y = img_h - 1;

		float *pblk = blk + (y + pad)*(BLK_SZ_X + 2 * pad);
		unsigned char *pin = in + i_y*stride;

		if (blk_x > 0 && blk_x < num_blk_x - 1)
		{
			int offset = blk_x*BLK_SZ_X - pad;

			uint16x8_t  vpix;
			float32x4_t vfb, vfc;

			int x = 0;
			for (; x < (BLK_SZ_X + pad * 2) / 8 * 8; x += 8)
			{
				vpix = vmovl_u8(vld1_u8((pin + x + offset)));
				vfb = vcvtq_f32_u32(vmovl_u16(vget_low_u16(vpix)));
				vfc = vcvtq_f32_u32(vmovl_u16(vget_high_u16(vpix)));
				vfb = vmulq_n_f32(vfb, 1.0f / 255.0f);
				vfc = vmulq_n_f32(vfc, 1.0f / 255.0f);

				vst1q_f32(pblk + x + 0, vfb);
				vst1q_f32(pblk + x + 4, vfc);

			}
			for (; x < BLK_SZ_X + pad * 2; x++)
			{
				pblk[x] = pin[x + offset] * (1.0f / 255.0f);
			}
		}
		else
		{
			for (int x = -pad; x < BLK_SZ_X + pad; x++)
			{
				int i_x = x + blk_x*BLK_SZ_X;
				if (i_x < 0) i_x = 0;
				if (i_x >= img_w) i_x = img_w - 1;

				pblk[pad + x] = pin[i_x] * (1.0f / 255.0f);
			}
		}
	}
}


template<int BLK_SZ_X, int BLK_SZ_Y> void blk_conv1_hwcn_neon(float *in_data, float *out_data, float *weight, float *bias)
{
	float32x4_t vsum0, vsum1, vsum2, vsum3;
	float32x4_t vw0, vw1, vw2;
	float32x4_t vin0, vin1;

	for (int y = 0; y < BLK_SZ_Y + 4; y++)
	{
		for (int x = 0; x < BLK_SZ_X + 4; x += 4)
		{
			float *pOut = out_data + (y*(BLK_SZ_X + 4) + x) * 4;
			float *pW = weight;
			float *pIn = in_data + (y*(BLK_SZ_X + 6) + x);

			vsum0 = vld1q_f32(bias);
			vsum1 = vsum0;
			vsum2 = vsum0;
			vsum3 = vsum0;

			for (int kh = 0; kh < 3; kh++)
			{
				vw0 = vld1q_f32(pW + 4 * 0);
				vw1 = vld1q_f32(pW + 4 * 1);
				vw2 = vld1q_f32(pW + 4 * 2);

				vin0 = vld1q_f32(pIn + 0);
				vin1 = vld1q_f32(pIn + 2);

				vsum0 = vmlaq_lane_f32(vsum0, vw0, vget_low_f32(vin0), 0);
				vsum1 = vmlaq_lane_f32(vsum1, vw0, vget_low_f32(vin0), 1);
				vsum2 = vmlaq_lane_f32(vsum2, vw0, vget_low_f32(vin1), 0);
				vsum3 = vmlaq_lane_f32(vsum3, vw0, vget_low_f32(vin1), 1);

				vsum0 = vmlaq_lane_f32(vsum0, vw1, vget_low_f32(vin0), 1);
				vsum1 = vmlaq_lane_f32(vsum1, vw1, vget_low_f32(vin1), 0);
				vsum2 = vmlaq_lane_f32(vsum2, vw1, vget_low_f32(vin1), 1);
				vsum3 = vmlaq_lane_f32(vsum3, vw1, vget_high_f32(vin1), 0);

				vsum0 = vmlaq_lane_f32(vsum0, vw2, vget_low_f32(vin1), 0);
				vsum1 = vmlaq_lane_f32(vsum1, vw2, vget_low_f32(vin1), 1);
				vsum2 = vmlaq_lane_f32(vsum2, vw2, vget_high_f32(vin1), 0);
				vsum3 = vmlaq_lane_f32(vsum3, vw2, vget_high_f32(vin1), 1);

				pW += 12;
				pIn += (BLK_SZ_X + 6);
			}

			float32x4_t	vzero = vdupq_n_f32(0.0f);

			vsum0 = vmaxq_f32(vsum0, vzero);
			vsum1 = vmaxq_f32(vsum1, vzero);
			vsum2 = vmaxq_f32(vsum2, vzero);
			vsum3 = vmaxq_f32(vsum3, vzero);

			vst1q_f32(pOut + 0, vsum0);
			vst1q_f32(pOut + 4, vsum1);
			vst1q_f32(pOut + 8, vsum2);
			vst1q_f32(pOut + 12, vsum3);
		}
	}
}


template<int BLK_SZ_X, int BLK_SZ_Y> void blk_conv2_hwcn_neon(float *in_data, float *out_data, float *weight, float *bias)
{

	float32x4_t vsum0, vsum1, vsum2, vsum3, vsum4, vsum5, vw;
	float32x4_t vIn0, vIn1, vIn2, vIn3, vIn4, vIn5;

#define CONV2_KERNEL_NEON(vin0, vin1, vin2, vin3, vin4, vin5) \
	vw = vld1q_f32(pW + 4 * 0); \
	vsum0 = vmlaq_lane_f32(vsum0, vw, vget_low_f32(vin0), 0); \
	vsum1 = vmlaq_lane_f32(vsum1, vw, vget_low_f32(vin1), 0); \
	vsum2 = vmlaq_lane_f32(vsum2, vw, vget_low_f32(vin2), 0); \
	vsum3 = vmlaq_lane_f32(vsum3, vw, vget_low_f32(vin3), 0); \
	vsum4 = vmlaq_lane_f32(vsum4, vw, vget_low_f32(vin4), 0); \
	vsum5 = vmlaq_lane_f32(vsum5, vw, vget_low_f32(vin5), 0); \
	vw = vld1q_f32(pW + 4 * 1); \
	vsum0 = vmlaq_lane_f32(vsum0, vw, vget_low_f32(vin0), 1); \
	vsum1 = vmlaq_lane_f32(vsum1, vw, vget_low_f32(vin1), 1); \
	vsum2 = vmlaq_lane_f32(vsum2, vw, vget_low_f32(vin2), 1); \
	vsum3 = vmlaq_lane_f32(vsum3, vw, vget_low_f32(vin3), 1); \
	vsum4 = vmlaq_lane_f32(vsum4, vw, vget_low_f32(vin4), 1); \
	vsum5 = vmlaq_lane_f32(vsum5, vw, vget_low_f32(vin5), 1); \
	vw = vld1q_f32(pW + 4 * 2); \
	vsum0 = vmlaq_lane_f32(vsum0, vw, vget_high_f32(vin0), 0); \
	vsum1 = vmlaq_lane_f32(vsum1, vw, vget_high_f32(vin1), 0); \
	vsum2 = vmlaq_lane_f32(vsum2, vw, vget_high_f32(vin2), 0); \
	vsum3 = vmlaq_lane_f32(vsum3, vw, vget_high_f32(vin3), 0); \
	vsum4 = vmlaq_lane_f32(vsum4, vw, vget_high_f32(vin4), 0); \
	vsum5 = vmlaq_lane_f32(vsum5, vw, vget_high_f32(vin5), 0); \
	vw = vld1q_f32(pW + 4 * 3); \
	vsum0 = vmlaq_lane_f32(vsum0, vw, vget_high_f32(vin0), 1); \
	vsum1 = vmlaq_lane_f32(vsum1, vw, vget_high_f32(vin1), 1); \
	vsum2 = vmlaq_lane_f32(vsum2, vw, vget_high_f32(vin2), 1); \
	vsum3 = vmlaq_lane_f32(vsum3, vw, vget_high_f32(vin3), 1); \
	vsum4 = vmlaq_lane_f32(vsum4, vw, vget_high_f32(vin4), 1); \
	vsum5 = vmlaq_lane_f32(vsum5, vw, vget_high_f32(vin5), 1); \

	for (int y = 0; y < BLK_SZ_Y + 2; y++)
	{
		for (int x = 0; x < BLK_SZ_X + 2; x += 6)
		{
			int xpos = x;
			int extra = x + 6 - (BLK_SZ_X + 2);
			if (extra > 0)
			{
				xpos -= extra;
			}

			float *pOut = out_data + (y*(BLK_SZ_X + 2) + xpos) * 4;
			float *pIn = in_data + (y*(BLK_SZ_X + 4) + xpos) * 4;

			vsum0 = vld1q_f32(bias);
			vsum1 = vsum0;
			vsum2 = vsum1;
			vsum3 = vsum2;
			vsum4 = vsum3;
			vsum5 = vsum4;

			float *pW = weight;

			for (int kh = 0; kh < 3; kh++)
			{
				float *pInWin = pIn + kh*(BLK_SZ_X + 4) * 4;

				vIn0 = vld1q_f32(pInWin + 0);
				vIn1 = vld1q_f32(pInWin + 4);
				vIn2 = vld1q_f32(pInWin + 8);
				vIn3 = vld1q_f32(pInWin + 12);
				vIn4 = vld1q_f32(pInWin + 16);
				vIn5 = vld1q_f32(pInWin + 20);

				CONV2_KERNEL_NEON(vIn0, vIn1, vIn2, vIn3, vIn4, vIn5);
				vIn0 = vld1q_f32(pInWin + 24);
				pW += 16;
				CONV2_KERNEL_NEON(vIn1, vIn2, vIn3, vIn4, vIn5, vIn0);
				vIn1 = vld1q_f32(pInWin + 28);
				pW += 16;
				CONV2_KERNEL_NEON(vIn2, vIn3, vIn4, vIn5, vIn0, vIn1);
				pW += 16;
			}

			float32x4_t vzero = vdupq_n_f32(0.0f);

			vsum0 = vmaxq_f32(vzero, vsum0);
			vsum1 = vmaxq_f32(vzero, vsum1);
			vsum2 = vmaxq_f32(vzero, vsum2);
			vsum3 = vmaxq_f32(vzero, vsum3);
			vsum4 = vmaxq_f32(vzero, vsum4);
			vsum5 = vmaxq_f32(vzero, vsum5);

			vst1q_f32(pOut + 0, vsum0);
			vst1q_f32(pOut + 4, vsum1);
			vst1q_f32(pOut + 8, vsum2);
			vst1q_f32(pOut + 12, vsum3);
			vst1q_f32(pOut + 16, vsum4);
			vst1q_f32(pOut + 20, vsum5);
		}
	}
}


template<int BLK_SZ_X, int BLK_SZ_Y> void blk_conv3_hwcn_neon(float *in_data, unsigned char *out_data, float *weight, float *bias, int img_w, int img_h, int stride, int blk_x, int blk_y)
{
	float32x4_t   vsum0, vsum1, vsum2, vsum3, vsum4, vsum5, vsum6, vsum7;
	float32x4_t   vw0, vw1, vw2;
	float32x4_t   vpix;

	for (int y = 0; y < BLK_SZ_Y; y++)
	{
		int img_y = blk_y*BLK_SZ_Y + y;
		if (img_y >= img_h) continue;

		for (int x = 0; x < BLK_SZ_X; x += 8)
		{
			int img_x = blk_x*BLK_SZ_X + x;

			unsigned char *pOut = out_data + img_y*stride + img_x;
			float *pInWin = in_data + (y*(BLK_SZ_X + 2) + x) * 4;
			float *pW = weight;

			vsum0 = vdupq_n_f32(0.0f);
			vsum1 = vsum0;
			vsum2 = vsum0;
			vsum3 = vsum0;
			vsum4 = vsum0;
			vsum5 = vsum0;
			vsum6 = vsum0;
			vsum7 = vsum0;

			for (int kh = 0; kh < 3; kh++)
			{
				vw0 = vld1q_f32(pW + 0);
				vw1 = vld1q_f32(pW + 4);
				vw2 = vld1q_f32(pW + 8);

				vpix = vld1q_f32(pInWin + 0);
				vsum0 = vmlaq_f32(vsum0, vpix, vw0);
				vpix = vld1q_f32(pInWin + 4);
				vsum1 = vmlaq_f32(vsum1, vpix, vw0);
				vsum0 = vmlaq_f32(vsum0, vpix, vw1);
				vpix = vld1q_f32(pInWin + 8);
				vsum2 = vmlaq_f32(vsum2, vpix, vw0);
				vsum1 = vmlaq_f32(vsum1, vpix, vw1);
				vsum0 = vmlaq_f32(vsum0, vpix, vw2);
				vpix = vld1q_f32(pInWin + 12);
				vsum3 = vmlaq_f32(vsum3, vpix, vw0);
				vsum2 = vmlaq_f32(vsum2, vpix, vw1);
				vsum1 = vmlaq_f32(vsum1, vpix, vw2);
				vpix = vld1q_f32(pInWin + 16);
				vsum4 = vmlaq_f32(vsum4, vpix, vw0);
				vsum3 = vmlaq_f32(vsum3, vpix, vw1);
				vsum2 = vmlaq_f32(vsum2, vpix, vw2);
				vpix = vld1q_f32(pInWin + 20);
				vsum5 = vmlaq_f32(vsum5, vpix, vw0);
				vsum4 = vmlaq_f32(vsum4, vpix, vw1);
				vsum3 = vmlaq_f32(vsum3, vpix, vw2);
				vpix = vld1q_f32(pInWin + 24);
				vsum6 = vmlaq_f32(vsum6, vpix, vw0);
				vsum5 = vmlaq_f32(vsum5, vpix, vw1);
				vsum4 = vmlaq_f32(vsum4, vpix, vw2);
				vpix = vld1q_f32(pInWin + 28);
				vsum7 = vmlaq_f32(vsum7, vpix, vw0);
				vsum6 = vmlaq_f32(vsum6, vpix, vw1);
				vsum5 = vmlaq_f32(vsum5, vpix, vw2);
				vpix = vld1q_f32(pInWin + 32);
				vsum7 = vmlaq_f32(vsum7, vpix, vw1);
				vsum6 = vmlaq_f32(vsum6, vpix, vw2);
				vpix = vld1q_f32(pInWin + 36);
				vsum7 = vmlaq_f32(vsum7, vpix, vw2);

				pW += 12;
				pInWin += (BLK_SZ_X + 2) * 4;
			}

			float32x4_t	v255 = vdupq_n_f32(255.0f);
			float32x4_t  vbias = vdupq_n_f32(bias[0]);

			float32x2_t vtemp0, vtemp1;
			vtemp0 = vpadd_f32(vpadd_f32(vget_low_f32(vsum0), vget_high_f32(vsum0)), vpadd_f32(vget_low_f32(vsum1), vget_high_f32(vsum1)));
			vtemp1 = vpadd_f32(vpadd_f32(vget_low_f32(vsum2), vget_high_f32(vsum2)), vpadd_f32(vget_low_f32(vsum3), vget_high_f32(vsum3)));
			vsum0 = vcombine_f32(vtemp0, vtemp1);

			vsum0 = vaddq_f32(vsum0, vbias);
			vsum0 = vmulq_f32(vsum0, v255);

			vtemp0 = vpadd_f32(vpadd_f32(vget_low_f32(vsum4), vget_high_f32(vsum4)), vpadd_f32(vget_low_f32(vsum5), vget_high_f32(vsum5)));
			vtemp1 = vpadd_f32(vpadd_f32(vget_low_f32(vsum6), vget_high_f32(vsum6)), vpadd_f32(vget_low_f32(vsum7), vget_high_f32(vsum7)));
			vsum4 = vcombine_f32(vtemp0, vtemp1);

			vsum4 = vaddq_f32(vsum4, vbias);
			vsum4 = vmulq_f32(vsum4, v255);

			if (img_x < img_w - 8)
			{
				uint8x8_t vout = vmovn_u16(vcombine_u16(vmovn_u32(vcvtq_u32_f32(vsum0)), vmovn_u32(vcvtq_u32_f32(vsum4))));
				vst1_u8(pOut, vout);
			}
			else
			{
				float temp[8];
				vst1q_f32(temp + 0, vsum0);
				vst1q_f32(temp + 4, vsum4);
				for (int i = 0; i < 8; i++)
				{
					if (img_x + i < img_w)
					{
						pOut[i] = (unsigned char)temp[i];
					}
				}
			}
		}
	}
}

#endif