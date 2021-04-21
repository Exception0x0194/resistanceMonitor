
#include <iostream>
using namespace std;

#define TUNE(r) (r < 0 ? 0 : (r > 255 ? 255 : r))

static int RGB_Y[256];
static int RGBR_V[256];
static int RGBG_U[256];
static int RGBG_V[256];
static int RGBB_U[256];

static int YUVY_R[256];
static int YUVY_G[256];
static int YUVY_B[256];

static int YUVU_R[256];
static int YUVU_G[256];
static int YUVU_B[256];

static int YUVV_R[256];
static int YUVV_G[256];
static int YUVV_B[256];

static int coff_rv[256];
static int coff_gu[256];
static int coff_gv[256];
static int coff_bu[256];

//直接采用公式浮点计算方式
//仅RGB2YUV采用了查表法，所以有一部分表是没有用到的
void InitTable()
{
    int i;
    for (i = 0; i < 256; i++)
    {
        //初始化表,放大256倍
        RGB_Y[i] = 298 * (i - 16);
        RGBR_V[i] = 408 * (i - 128);
        RGBG_U[i] = 100 * (128 - i);
        RGBG_V[i] = 208 * (128 - i);
        RGBB_U[i] = 517 * (i - 128);
        //y=0.257*r+0.504*g+0.098*b+16
        //u = -0.148*r - 0.291*g + 0.439*b + 128
        //0.439*r - 0.368*g - 0.071*b + 128
        YUVY_R[i] = 66 * i;
        YUVY_G[i] = 129 * i;
        YUVY_B[i] = 25 * i;
        YUVU_R[i] = -38 * i;
        YUVU_G[i] = -74 * i;
        YUVU_B[i] = 112 * i;
        YUVV_R[i] = 112 * i;
        YUVV_G[i] = -94 * i;
        YUVV_B[i] = -18 * i;
        /*所用公式(此公式不适用)
        *pRGB = (unsigned char)(1.0*y + 8 + 1.402*(v-128));    pRGB++;                 // r
        *pRGB = (unsigned char)(1.0*y - 0.34413*(u-128) - 0.71414*(v-128));  pRGB++;   // g
        *pRGB = (unsigned char)(1.0*y + 1.772*(u-128) + 0);    pRGB++ ;  
        */
        coff_rv[i] = (8 + 1.402 * (i - 128)) * 256;
        coff_gu[i] = -0.34413 * (i - 128) * 256;
        coff_gv[i] = -0.71414 * (i - 128) * 256;
        coff_bu[i] = 1.772 * (i - 128) * 256;

        /*应该使用如下公式：
Y’ = 0.257*R' + 0.504*G' + 0.098*B' + 16
Cb' = -0.148*R' - 0.291*G' + 0.439*B' + 128
Cr' = 0.439*R' - 0.368*G' - 0.071*B' + 128(标红的两组公式是可逆的转换)
R' = 1.164*(Y’-16) + 1.596*(Cr'-128)
G' = 1.164*(Y’-16) - 0.813*(Cr'-128) - 0.392*(Cb'-128)
B' = 1.164*(Y’-16) + 2.017*(Cb'-128)
*/
    }
}

void YUV2RGB422(unsigned char *pRGB, unsigned char *pYUV, int size)
{
    unsigned char y, u, v, y1;
    int r, g, b;
    unsigned int i = 0;
    unsigned int loop = size >> 1;
    while (loop-- > 0)
    {
        // if (!(loop / 10000))
        //     printf("%d\n", loop);
        y = *pYUV;
        pYUV++;
        u = *pYUV;
        pYUV++;
        y1 = *pYUV;
        pYUV++;
        v = *pYUV;
        pYUV++;

        r = 1.164 * (y - 16) + 1.596 * (v - 128);
        g = 1.164 * (y - 16) - 0.813 * (v - 128) - 0.392 * (u - 128);
        b = 1.164 * (y - 16) + 2.017 * (u - 128);

        *pRGB = TUNE(r);
        pRGB++;
        *pRGB = TUNE(g);
        pRGB++;
        *pRGB = TUNE(b);
        pRGB++;

        r = 1.164 * (y1 - 16) + 1.596 * (v - 128);
        g = 1.164 * (y1 - 16) - 0.813 * (v - 128) - 0.392 * (u - 128);
        b = 1.164 * (y1 - 16) + 2.017 * (u - 128);
        *pRGB = TUNE(r);
        pRGB++;
        *pRGB = TUNE(g);
        pRGB++;
        *pRGB = TUNE(b);
        pRGB++;
    }
}

//size 为图片的大小
void RGB2YUV422(unsigned char *pRGB, unsigned char *pYUV, int size)
{
    unsigned char r, g, b, u, v, u1, v1, r1, g1, b1;
    //unsigned char *YUVBuff;
    //unsigned char* p;
    //p = YUVBuff;//
    int loop = size / 2;
    int i;
    for (i = 0; i < loop; i++)
    {
        r = *pRGB;
        pRGB++;
        g = *pRGB;
        pRGB++;
        b = *pRGB;
        pRGB++;
        r1 = *pRGB;
        pRGB++;
        g1 = *pRGB;
        pRGB++;
        b1 = *pRGB;
        pRGB++;

        //new method ---  right
        int y = ((YUVY_R[r] + YUVY_G[g] + YUVY_B[b] + 128) >> 8) + 16;
        u = ((YUVU_R[r] + YUVU_G[g] + YUVU_B[b] + 128) >> 8) + 128;
        v = ((YUVV_R[r] + YUVV_G[g] + YUVV_B[b] + 128) >> 8) + 128;
        int y1 = ((YUVY_R[r1] + YUVY_G[g1] + YUVY_B[b1] + 128) >> 8) + 16;
        u1 = ((YUVU_R[r1] + YUVU_G[g1] + YUVU_B[b1] + 128) >> 8) + 128;
        v1 = ((YUVV_R[r1] + YUVV_G[g1] + YUVV_B[b1] + 128) >> 8) + 128;

        *pYUV++ = TUNE(y);
        *pYUV++ = (TUNE(u) + TUNE(u1)) >> 1;
        *pYUV++ = TUNE(y1);
        *pYUV++ = TUNE(v);
    }
}

void inline Yuv2RgbPixel(unsigned char y, unsigned char u, unsigned char v, unsigned char *rgbPixel)
{
    int r = (RGB_Y[y] + RGBR_V[v] + 128) >> 8;
    int g = ((RGB_Y[y] + RGBG_V[v] + RGBG_U[u] + 128) >> 8);
    int b = ((RGB_Y[y] + RGBB_U[u] + 128) >> 8);
    *rgbPixel = TUNE(r);
    rgbPixel++;
    *rgbPixel = TUNE(g);
    rgbPixel++;
    *rgbPixel = TUNE(b);
}

void YUV2RGB(unsigned char *pRGB, unsigned char *pYUV, int size) //444
{
    unsigned char y, u, v;

    for (int i = 0; i < size; i++)
    {
        y = *pYUV;
        pYUV++;
        u = *pYUV;
        pYUV++;
        v = *pYUV;
        pYUV++;

        Yuv2RgbPixel(y, u, v, pRGB);
        pRGB += 3;
    }
}

void inline Rgb2YuvPiexl(unsigned char r, unsigned char g, unsigned char b, unsigned char *pYUV)
{
    int y = ((YUVY_R[r] + YUVY_G[g] + YUVY_B[b] + 128) >> 8) + 16;
    int u = ((YUVU_R[r] + YUVU_G[g] + YUVU_B[b] + 128) >> 8) + 128;
    int v = ((YUVV_R[r] + YUVV_G[g] + YUVV_B[b] + 128) >> 8) + 128;
    *pYUV = TUNE(y);
    pYUV++;
    //*pYUV = u < 0 ? 0 : (u > 255 ? 255 : u);pYUV++;
    *pYUV = TUNE(u);
    pYUV++;
    *pYUV = TUNE(v);
}

void RGB2YUV(unsigned char *pRGB, unsigned char *pYUV, int size)
{
    unsigned char r, g, b, y, u, v;
    for (int i = 0; i < size; i++)
    {
        r = *pRGB;
        pRGB++;
        g = *pRGB;
        pRGB++;
        b = *pRGB;
        pRGB++;
        Rgb2YuvPiexl(r, g, b, pYUV);
        pYUV += 3;
    }
}