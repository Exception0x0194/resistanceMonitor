#ifndef _RGB2YUV__
#define _RGB2YUV__

void YUV2RGB422(unsigned char *pRGB, unsigned char *pYUV, int size);
void YUV2RGB(unsigned char *pRGB, unsigned char *pYUV, int size);
void InitTable();

#endif