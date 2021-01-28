#ifndef __KERNEL_MACRO_H__
#define __KERNEL_MACRO_H__
	
#define SetKernelArg1(kernel, arg0) \
	status = clSetKernelArg(kernel, 0, sizeof(arg0), &arg0); \

#define SetKernelArg2(kernel, arg0, arg1) \
	SetKernelArg1(kernel, arg0) \
	status |= clSetKernelArg(kernel, 1, sizeof(arg1), &arg1); \

#define SetKernelArg3(kernel, arg0, arg1, arg2) \
	SetKernelArg2(kernel, arg0, arg1) \
	status |= clSetKernelArg(kernel, 2, sizeof(arg2), &arg2); \

#define SetKernelArg4(kernel, arg0, arg1, arg2, arg3) \
	SetKernelArg3(kernel, arg0, arg1, arg2) \
	status |= clSetKernelArg(kernel, 3, sizeof(arg3), &arg3); \


#define SetKernelArg5(kernel, arg0, arg1, arg2, arg3, arg4) \
	SetKernelArg4(kernel, arg0, arg1, arg2, arg3) \
	status |= clSetKernelArg(kernel, 4, sizeof(arg3), &arg4); \


#define SetKernelArg6(kernel, arg0, arg1, arg2, arg3, arg4, arg5) \
	SetKernelArg5(kernel, arg0, arg1, arg2, arg3, arg4) \
	status |= clSetKernelArg(kernel, 5, sizeof(arg5), &arg5); \


#define SetKernelArg7(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6) \
	SetKernelArg6(kernel, arg0, arg1, arg2, arg3, arg4, arg5) \
	status |= clSetKernelArg(kernel, 6, sizeof(arg6), &arg6); \

#define SetKernelArg8(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
	SetKernelArg7(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6) \
	status |= clSetKernelArg(kernel, 7, sizeof(arg7), &arg7); \

#define SetKernelArg9(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
	SetKernelArg8(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
	status |= clSetKernelArg(kernel, 8, sizeof(arg8), &arg8); \


#define SetKernelArg10(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
	SetKernelArg9(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
	status |= clSetKernelArg(kernel, 9, sizeof(arg9), &arg9); \


#define SetKernelArg11(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10) \
	SetKernelArg10(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
	status |= clSetKernelArg(kernel, 10, sizeof(arg10), &arg10); \


#define SetKernelArg12(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11) \
	SetKernelArg11(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10) \
	status |= clSetKernelArg(kernel, 11, sizeof(arg11), &arg11); \


#define SetKernelArg13(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12) \
	SetKernelArg12(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11) \
	status |= clSetKernelArg(kernel, 12, sizeof(arg12), &arg12); \


#define SetKernelArg14(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13) \
	SetKernelArg13(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12) \
	status |= clSetKernelArg(kernel, 13, sizeof(arg13), &arg13); \


#define SetKernelArg15(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14) \
	SetKernelArg14(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13) \
	status |= clSetKernelArg(kernel, 14, sizeof(arg14), &arg14); \


#define SetKernelArg16(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15) \
	SetKernelArg15(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14) \
	status |= clSetKernelArg(kernel, 15, sizeof(arg15), &arg15); \


#define SetKernelArg17(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16) \
	SetKernelArg16(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15) \
	status |= clSetKernelArg(kernel, 16, sizeof(arg16), &arg16); \


#define SetKernelArg18(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17) \
	SetKernelArg17(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16) \
	status |= clSetKernelArg(kernel, 17, sizeof(arg17), &arg17); \


#define SetKernelArg19(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18) \
	SetKernelArg18(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17) \
	status |= clSetKernelArg(kernel, 18, sizeof(arg18), &arg18); \
	

#define SetKernelArg20(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19) \
	SetKernelArg19(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18) \
	status |= clSetKernelArg(kernel, 19, sizeof(arg19), &arg19); \


#define SetKernelArg21(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19, arg20) \
	SetKernelArg20(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19) \
	status |= clSetKernelArg(kernel, 20, sizeof(arg20), &arg20); \


#define SetKernelArg22(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19, arg20, arg21) \
	SetKernelArg21(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19, arg20) \
	status |= clSetKernelArg(kernel, 21, sizeof(arg21), &arg21); \

#define SetKernelArg23(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19, arg20, arg21, arg22) \
	SetKernelArg22(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19, arg20, arg21) \
	status |= clSetKernelArg(kernel, 22, sizeof(arg22), &arg22); \


#define SetKernelArg24(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19, arg20, arg21, arg22, arg23) \
	SetKernelArg23(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19, arg20, arg21, arg22) \
	status |= clSetKernelArg(kernel, 23, sizeof(arg23), &arg23); \


#define SetKernelArg25(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24) \
	SetKernelArg24(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19, arg20, arg21, arg22, arg23) \
	status |= clSetKernelArg(kernel, 24, sizeof(arg24), &arg24); \


#define SetKernelArg26(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25) \
	SetKernelArg25(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24) \
	status |= clSetKernelArg(kernel, 25, sizeof(arg25), &arg25); \

#define SetKernelArg27(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26) \
	SetKernelArg26(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25) \
	status |= clSetKernelArg(kernel, 26, sizeof(arg26), &arg26); \


#define SetKernelArg28(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27) \
	SetKernelArg27(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26) \
	status |= clSetKernelArg(kernel, 27, sizeof(arg27), &arg27); \

#define SetKernelArg29(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28) \
	SetKernelArg28(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27) \
	status |= clSetKernelArg(kernel, 28, sizeof(arg28), &arg28); \

#define SetKernelArg30(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29) \
	SetKernelArg29(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28) \
	status |= clSetKernelArg(kernel, 29, sizeof(arg29), &arg29); \

#define SetKernelArg71(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31, arg32, arg33, arg34, arg35, arg36, arg37, \
	arg38, arg39, arg40, arg41, arg42, arg43, arg44, arg45, arg46, arg47, arg48, arg49, arg50, arg51, arg52, arg53, arg54, arg55, arg56, arg57, arg58, arg59, arg60, arg61, arg62, arg63, arg64, arg65, arg66, arg67, arg68, arg69, arg70) \
	SetKernelArg30(kernel, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29) \
	status |= clSetKernelArg(kernel, 30, sizeof(arg30), &arg30); \
	status |= clSetKernelArg(kernel, 31, sizeof(arg31), &arg31); \
	status |= clSetKernelArg(kernel, 32, sizeof(arg32), &arg32); \
	status |= clSetKernelArg(kernel, 33, sizeof(arg33), &arg33); \
	status |= clSetKernelArg(kernel, 34, sizeof(arg34), &arg34); \
	status |= clSetKernelArg(kernel, 35, sizeof(arg35), &arg35); \
	status |= clSetKernelArg(kernel, 36, sizeof(arg36), &arg36); \
	status |= clSetKernelArg(kernel, 37, sizeof(arg37), &arg37); \
	status |= clSetKernelArg(kernel, 38, sizeof(arg38), &arg38); \
	status |= clSetKernelArg(kernel, 39, sizeof(arg39), &arg39); \
	status |= clSetKernelArg(kernel, 40, sizeof(arg40), &arg40); \
	status |= clSetKernelArg(kernel, 41, sizeof(arg41), &arg41); \
	status |= clSetKernelArg(kernel, 42, sizeof(arg42), &arg42); \
	status |= clSetKernelArg(kernel, 43, sizeof(arg43), &arg43); \
	status |= clSetKernelArg(kernel, 44, sizeof(arg44), &arg44); \
	status |= clSetKernelArg(kernel, 45, sizeof(arg45), &arg45); \
	status |= clSetKernelArg(kernel, 46, sizeof(arg46), &arg46); \
	status |= clSetKernelArg(kernel, 47, sizeof(arg47), &arg47); \
	status |= clSetKernelArg(kernel, 48, sizeof(arg48), &arg48); \
	status |= clSetKernelArg(kernel, 49, sizeof(arg49), &arg49); \
	status |= clSetKernelArg(kernel, 50, sizeof(arg50), &arg50); \
	status |= clSetKernelArg(kernel, 51, sizeof(arg51), &arg51); \
	status |= clSetKernelArg(kernel, 52, sizeof(arg52), &arg52); \
	status |= clSetKernelArg(kernel, 53, sizeof(arg53), &arg53); \
	status |= clSetKernelArg(kernel, 54, sizeof(arg54), &arg54); \
	status |= clSetKernelArg(kernel, 55, sizeof(arg55), &arg55); \
	status |= clSetKernelArg(kernel, 56, sizeof(arg56), &arg56); \
	status |= clSetKernelArg(kernel, 57, sizeof(arg57), &arg57); \
	status |= clSetKernelArg(kernel, 58, sizeof(arg58), &arg58); \
	status |= clSetKernelArg(kernel, 59, sizeof(arg59), &arg59); \
	status |= clSetKernelArg(kernel, 60, sizeof(arg60), &arg60); \
	status |= clSetKernelArg(kernel, 61, sizeof(arg61), &arg61); \
	status |= clSetKernelArg(kernel, 62, sizeof(arg62), &arg62); \
	status |= clSetKernelArg(kernel, 63, sizeof(arg63), &arg63); \
	status |= clSetKernelArg(kernel, 64, sizeof(arg64), &arg64); \
	status |= clSetKernelArg(kernel, 65, sizeof(arg65), &arg65); \
	status |= clSetKernelArg(kernel, 66, sizeof(arg66), &arg66); \
	status |= clSetKernelArg(kernel, 67, sizeof(arg67), &arg67); \
	status |= clSetKernelArg(kernel, 68, sizeof(arg68), &arg68); \
	status |= clSetKernelArg(kernel, 69, sizeof(arg69), &arg69); \
	status |= clSetKernelArg(kernel, 70, sizeof(arg70), &arg70); \

#endif