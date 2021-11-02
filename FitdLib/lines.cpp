#include "common.h"

#define ENTER
#define LEAVE

#include <math.h>
#include <stdlib.h>

unsigned char* polyBackBuffer = NULL;

void swapFunc(int* a, int* b)
{
    int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

#define MIN(a,b) (a<b?a:b)
#define MAX(a,b) (a>b?a:b)
#define SWAP(a,b) (swapFunc(&a,&b))

void pixel(int x,int y, unsigned char c)
{
    if(x>=0 && x<320 && y>=0 && y<200)
    {
        *(polyBackBuffer+y*320+x) = c;
    }
}

void hline(int x1, int x2, int y, unsigned char c)
{
    int i;

    for(i=x1; i < (x2 + 1);i++)
    {
        pixel(i,y,c);
    }
}

void vline(int x, int y1, int y2, unsigned char c)
{
    int i;

    for(i=y1; i< (y2 + 1);i++)
    {
        pixel(x,i,c);
    }
}

void bsubline_1(int x1, int y1, int x2, int y2, unsigned char c) {
    int x, y, ddx, ddy, e;
    ddx = abs(x2 - x1);
    ddy = abs(y2 - y1) << 1;
    e = ddx - ddy;
    ddx <<= 1;

    if (x1 > x2) {
        SWAP(x1, x2);
        SWAP(y1, y2);
    }

    ENTER;

    for (x = x1, y = y1; x <= x2; x++) {
        pixel(x, y, c);
        if (e < 0) {
            y++;
            e += ddx - ddy;
        } else {
            e -= ddy;
        }
    }

    LEAVE;
}

void bsubline_2(int x1, int y1, int x2, int y2, unsigned char c) {
    int x, y, ddx, ddy, e;
    ddx = abs(x2 - x1) << 1;
    ddy = abs(y2 - y1);
    e = ddy - ddx;
    ddy <<= 1;

    if (y1 > y2) {
        SWAP(x1, x2);
        SWAP(y1, y2);
    }

    ENTER;

    for (y = y1, x = x1; y <= y2; y++) {
        pixel(x, y, c);
        if (e < 0) {
            x++;
            e += ddy - ddx;
        } else {
            e -= ddx;
        }
    }

    LEAVE;
}

void bsubline_3(int x1, int y1, int x2, int y2, unsigned char c) {
    int x, y, ddx, ddy, e;
    ddx = abs(x1 - x2) << 1;
    ddy = abs(y2 - y1);
    e = ddy - ddx;
    ddy <<= 1;

    if (y1 > y2) {
        SWAP(x1, x2);
        SWAP(y1, y2);
    }

    ENTER;

    for (y = y1, x = x1; y <= y2; y++) {
        pixel(x, y, c);
        if (e < 0) {
            x--;
            e += ddy - ddx;
        } else {
            e -= ddx;
        }
    }

    LEAVE;
}

void bsubline_4(int x1, int y1, int x2, int y2, unsigned char c) {
    int x, y, ddx, ddy, e;
    ddy = abs(y2 - y1) << 1;
    ddx = abs(x1 - x2);
    e = ddx - ddy;
    ddx <<= 1;

    if (x1 > x2) {
        SWAP(x1, x2);
        SWAP(y1, y2);
    }

    for (x = x1, y = y1; x <= x2; x++) {
        pixel(x, y, c);
        if (e < 0) {
            y--;
            e += ddx - ddy;
        } else {
            e -= ddy;
        }
    }
}

void line(int x1, int y1, int x2, int y2, unsigned char c) {
    float k;
    if ((x1 == x2) && (y1 == y2)) {
        pixel(x1, y1, c);
        return;
    }

    if (x1 == x2) {
        vline(x1, MIN(y1, y2), MAX(y1, y2), c);
        return;
    }

    if (y1 == y2) {
        hline(MIN(x1, x2), MAX(x1, x2), y1, c);
        return;
    }

    k = (float)(y2 - y1) / (float)(x2 - x1);

    if ((k >= 0) && (k <= 1)) {
        bsubline_1(x1, y1, x2, y2, c);
    } else if (k > 1) {
        bsubline_2(x1, y1, x2, y2, c);
    } else if ((k < 0) && (k >= -1)) {
        bsubline_4(x1, y1, x2, y2, c);
    } else {
        bsubline_3(x1, y1, x2, y2, c);
    } 
}

#undef SWAP
