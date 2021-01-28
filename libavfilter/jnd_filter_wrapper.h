#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void *CreateAndInitJndFilter(int qp, int threads);
int JndFilterProcess(void *filter, unsigned char *in, unsigned char *out, int width, int height, int stride);
int JndFilterUpdate(void *filter, int qp);
void DestoryJndFilter(void *filter);

#ifdef __cplusplus
}
#endif